#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <RCSwitch.h>

#define WLAN_SSID       "WIFI_SSID"
#define WLAN_PASS       "WIFI_PW"

#define AIO_SERVER      "MQTT_HOST"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "MQTT_USER"
#define AIO_KEY         "MQTT_PW"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
RCSwitch mySwitch = RCSwitch();

int rf433Led = 15; //wemos led pin
int rf433Pin = 13; //wemos rf433 data pin

Adafruit_MQTT_Publish mqttState = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/state");

void MQTT_connect();


void setup() {
  pinMode(rf433Led, OUTPUT);
  Serial.begin(9600);
  
  mySwitch.enableReceive(rf433Pin);  // use only interrupt pins eg.: D3, D7

  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  MQTT_connect();

}

void loop() {

  if (mySwitch.available()) {
    
    int value = mySwitch.getReceivedValue();
    
    Serial.println( mySwitch.getReceivedValue() );
    
    //button A action
    if (value==12904354) {
       buttonAPress();
       }
    //button B action
    if (value==12904356) {
       buttonBPress();
       }       
      
    }
    
    mySwitch.resetAvailable();
  }




void buttonAPress() {
  blinkLED();
  Serial.println("Button A Pressed");

  Serial.print("Sending State ");
  MQTT_connect();
  if (! mqttState.publish("A")) {
    Serial.println(F(" Failed"));
  } else {
    Serial.println(F(" OK!"));
  }
  Serial.println("Waiting 2s");
  delay(2000);
  
}

void buttonBPress() {
  blinkLED();
  Serial.println("Button B Pressed");
    Serial.print("Sending State ");
  MQTT_connect();
  if (! mqttState.publish("B")) {
    Serial.println(F(" Failed"));
  } else {
    Serial.println(F(" OK!"));
  }
  Serial.println("Waiting 2s");
  delay(2000);
}


void blinkLED() {
digitalWrite(rf433Led,HIGH);
delay(250);
digitalWrite(rf433Led,LOW);

}


void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 1 seconds...");
       mqtt.disconnect();
       delay(1000);  // wait 1 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
