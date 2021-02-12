#include  <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "DHT.h"

#define FIREBASE_HOST "sensor-99172.firebaseio.com"
#define FIREBASE_AUTH "W5NfuEPtAiQLIuphiXxuXfL7qN6EzjLDyqDTli9M"
#define WIFI_SSID "Wireless6201" // Change the name of your WIFI
#define WIFI_PASSWORD "networld" // Change the password of your WIFI

int analogPin=A0;
int analogval;
int soilpercentage;
int moisturepercentage;

#define DHTPIN 14    // Data Pin of DHT 11 , for NodeMCU D5 GPIO no. is 14
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

int Relay1=2;  //D4
int rel1;

void setup() {

   Serial.begin(9600);
   pinMode(Relay1, OUTPUT);
   digitalWrite(Relay1,HIGH);
   WiFi.begin (WIFI_SSID, WIFI_PASSWORD);
   while (WiFi.status() != WL_CONNECTED) {
    delay(10000);
    Serial.print(".");
  }
  dht.begin();
  Serial.println ("");
  Serial.println ("WiFi Connected!");
  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);
  Firebase.setInt("MOTORSTATUS",0);  
}
void firebasereconnect()
{
  Serial.println("Trying to reconnect");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
   
  if (Firebase.failed()){
    Serial.print("setting number failed:");
    Serial.println(Firebase.error());
    firebasereconnect();
    return;
  }
  //DHT11
  
  int h = dht.readHumidity();
  int t = dht.readTemperature();  // Reading temperature as Celsius (the default)
  Serial.print("temp:  ");
  Serial.print(t);
  Serial.print("         ");
  Serial.print("Humidity:  ");
  Serial.println(h);
  Firebase.setInt ("Temp",t);
  Firebase.setInt ("Humidity",h);
  
  //MOISTURE SENSOR
  
  analogval=analogRead(analogPin);
  moisturepercentage=map(analogval,900,0,0,100);
  soilpercentage=100-moisturepercentage;
  
  
  if(soilpercentage>=0 && soilpercentage<=100){
    Serial.print("SOIL LEVEL: ");
    Serial.print(soilpercentage);
    Serial.print("         ");
    Firebase.setInt ("SOIL LEVEL",soilpercentage);
  }
  else if(soilpercentage>100){
    Serial.print("SOIL LEVEL:  0");
    Serial.print("         ");
    Firebase.setInt ("SOIL LEVEL",0); 
  }
  else if(soilpercentage<0){
    Serial.print("SOIL LEVEL:  0");
    Serial.print("         ");
    Firebase.setInt ("SOIL LEVEL",0);
  }
  if(moisturepercentage>0 && moisturepercentage<100){
    Serial.print("MOISTURE LEVEL: ");
    Serial.print(moisturepercentage);
    Serial.println();
    Firebase.setInt ("MOISTURE LEVEL",moisturepercentage);
  }
  else if(moisturepercentage>100){
    Serial.print("MOISTURE LEVEL: 100");
    Serial.println();
    Firebase.setInt ("MOISTURE LEVEL",100); 
  }
  else if(moisturepercentage<0){
    Serial.print("MOISTURE LEVEL:  0");
    Serial.println();
    Firebase.setInt ("MOISTURE LEVEL",0); 
  }

  //MOTOR

  rel1=Firebase.getString("MOTORSTATUS").toInt(); 
  if(rel1==1){
  // If, the Status is 1, turn on the MOTOR                             
    digitalWrite(Relay1,LOW);
  }
  if(rel1==0){
  // If, the Status is 0, turn Off the Relay1
                                    
    digitalWrite(Relay1,HIGH);  
  }
  
  delay(1000);
}
