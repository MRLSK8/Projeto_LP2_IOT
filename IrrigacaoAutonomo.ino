#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "9d462ce984b14daab55df57e418d1bed";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "MRLSK8";
char pass[] = "abcdefgh";

//Colocar a API Key para escrita neste campo
//Ela é fornecida no canal que foi criado na aba API Keys
String apiKey = "6HQ5OLHO8C3W35FT";       //trocar pela API Write
const char* server = "api.thingspeak.com";

WiFiClient client;
 

int PercentualSensorUmidadeChuva;
float slider;
int buttonValue;

void setup()
{
  // Debug console
  Serial.begin(9600);

  //Inicia o WiFi
  WiFi.begin(ssid, pass);

  //Espera a conexão no router
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //Logs na porta serial
  Serial.println("");
  Serial.print("Conectado na rede ");
  Serial.println(ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  
  pinMode(D3, OUTPUT);
  digitalWrite(D3, LOW);
  
  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  
   Blynk.run();
   FazLeituraUmidade();
   WidgetLED led(V1);
   
   if(buttonValue == 0){
      if (PercentualSensorUmidadeChuva > slider) { // If slider is OFF, make sure button is as well
        led.on();
        Blynk.setProperty(V1, "color", "#FF2401");
        digitalWrite(D3, LOW);
        Blynk.setProperty(V3,"offLabel","OFF");
      }else { // If slider is anything equal or higher than 1, set button ON
        led.on();
        Blynk.setProperty(V1, "color", "#00FF00");
        digitalWrite(D3, HIGH);
        Blynk.setProperty(V3,"onLabel","ON");
      }
   }else{
      led.on();
      Blynk.setProperty(V1, "color", "#00FF00");
      //Blynk.virtualWrite(V3,0);
      digitalWrite(D3, HIGH);
      Blynk.setProperty(V3,"onLabel","ON");
   }
   

   //Inicia um client TCP para o envio dos dados
  if (client.connect(server,80)) {
     String postStr = apiKey;
     postStr +="&field1=";
     postStr += String(PercentualSensorUmidadeChuva);
 
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(postStr.length());
     client.print("\n\n");
     client.println(postStr);
  }
   
   client.stop();

   delay (2000);
   
}
void FazLeituraUmidade(void)
{
    int ValorADC;
    
    ValorADC = analogRead(A0);  
    Serial.print("[Leitura ADC] ");
    Serial.println(ValorADC);
     
    PercentualSensorUmidadeChuva = 100 * ((1024-(float)ValorADC) / 1024);
    Serial.print("[Umidade Percentual] ");
    Serial.print(PercentualSensorUmidadeChuva);
    Blynk.virtualWrite(V5, PercentualSensorUmidadeChuva);
    Serial.println("%");
}    


// SLIDER
BLYNK_WRITE(V2) {  // reads value from slider
    float value = param.asInt();
    slider = value;
}
BLYNK_WRITE(V0) {  // reads value from slider
    int value = param.asInt();
    buttonValue = value;
}
