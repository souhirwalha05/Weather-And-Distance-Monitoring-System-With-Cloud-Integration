//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C LCD = LiquidCrystal_I2C(0x27, 20, 4);
const char* ssid = "Wokwi-GUEST";
const char* password = "";

String payload;
String channel="TP2"; 
const char* topic1="TP2/led";
const char* topic2="TP2/temp";
const char* topic3="TP2/hum";
const char* topicdistance="TP2/distance";

const char* server = "mqtt.beebotte.com";
const char* username ="token:token_BJuKXbM3FEmeANnM";


char ClientID[]="";
const int port = 1883;
int trigP = 14;
int echoP = 35;


WiFiClient client;
PubSubClient clientmqtt(server,1883,client);

// Handle a received message
void callback(char* topic, byte* payload, unsigned int length)
{ 

  LCD.setCursor(0, 0);
  LCD.println("Callback executed");
  Serial.println("MQTT callback executed");
  Serial.print("topic:");
  Serial.println(topic);

  char message[length+1];
  for(int i=0;i<length;i++)
  message[i]=(char)payload[i];

 Serial.print("received Json:");
   Serial.println(message);
//Votre traitement............................;

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, message);

  

  if (strcmp(topic, topic1) == 0 ){
    boolean data = doc["data"];
    digitalWrite(32, data);
    LCD.setCursor(0, 1);
    LCD.print(" led =");
    LCD.print(data);
    LCD.print("            ");
    if (data == true ){
      PubBeebotte( ) ;

    }
  }

  if (strcmp(topic, topic2) == 0 ){
    float data = doc["data"];
      LCD.setCursor(0, 2);
      LCD.print(" temp=");
      LCD.print(data);
      Serial.println(data);
  }

  if (strcmp(topic, topic3) == 0 ){
    int data = doc["data"];
    LCD.setCursor(0, 3);
    LCD.print(" hum=");
    LCD.print(data);
    Serial.println(data);
  }


}
void PubBeebotte( ){

  long duration;
          // Clear the trigPin
          digitalWrite(trigP, LOW);
          delayMicroseconds(2);
          // Sets the trigPin on HIGH state for 10 micro seconds
          digitalWrite(trigP, HIGH);
          delayMicroseconds(10);
          digitalWrite(trigP, LOW);
            // Reads the echoPin, returns the sound wave travel time in microseconds
          duration = pulseIn(echoP, HIGH);
            // Calculate the distance
          float distance = duration * 0.034/2;
          LCD.setCursor(0, 2);
          LCD.print("");
          LCD.setCursor(0, 3);
          LCD.print("");

        Serial.print("distance=");
        Serial.println(String(distance));
        

        String d;
        StaticJsonDocument <1024> T;
        T["channel"]= channel;
        T["resource"]= "led"; 
        T["write"]= true; 
        T["data"]= false; 
        serializeJson (T, d);

        const char * data= d.c_str();

        clientmqtt.publish(topic1 , data);

        String dist;
        T["resource"]= "distance"; 
        T["data"]= distance; 
        serializeJson (T, dist);

        data= dist.c_str();

        clientmqtt.publish(topicdistance , data);
        
        LCD.setCursor(0, 1);
        LCD.print(" distance =");
        LCD.print(distance);
}


void setup() {
   LCD.init();
  pinMode(32,OUTPUT); // pinMode(33,OUTPUT);   pinMode(34,OUTPUT);
  pinMode(trigP, OUTPUT);
  pinMode(echoP, INPUT); 
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
clientmqtt.setServer(server,port);
 Serial.println("Server ready");
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.println("System Online");

  /*
  long duration;
  // Clear the trigPin
  digitalWrite(trigP, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigP, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigP, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoP, HIGH);
    // Calculate the distance
  float distance = duration * 0.034/2;
Serial.print("distance=");
Serial.println(String(distance));
*/
}


void loop() {
    if(!clientmqtt.connected()){
    Serial.println("MQTT not connected");
    sprintf(ClientID, "%s%s", "kamelsub", String(random(10)));
    if(clientmqtt.connect(ClientID, username,NULL)){
    
    Serial.println("MQTT Connected");
     clientmqtt.subscribe(topic1);
     clientmqtt.subscribe(topic2);
     clientmqtt.subscribe(topic3);
     clientmqtt.setCallback(callback);
    
  }
    }
  else  // client connected
  {     
   
     clientmqtt.loop();  
     
  }
    
}