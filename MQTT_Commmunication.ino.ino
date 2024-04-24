#include "secrets.h"
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <HCSR04.h>

// For ultrosonic sensor
byte triggerPin = 14;
byte echoPin = 15;
double threshold;

int LEDPin = 10;


// WiFi NINA info
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "SIT210";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void MQTT_Connect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoNano33", mqtt_username, mqtt_password)) {
      Serial.println("connected");
      // Ssubscribe
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LEDPin, OUTPUT);
  digitalWrite(LEDPin, LOW);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  // Connecting to the WIFI
  int status = WiFi.begin(ssid, pass);
  while (status != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  MQTT_Connect(); // Connect to MQTT on startup

  // Ultrosonic sensor set up
  HCSR04.begin(triggerPin, echoPin);
}

void loop() {
  double* distances = HCSR04.measureDistanceCm();
  if (distances[0] < threshold){
    SensorTrigger();
  }

  client.loop();
}

void SensorTrigger (){
  digitalWrite(LEDPin, HIGH);
  delay(250);
  digitalWrite(LEDPin, LOW);
  delay(250);

  digitalWrite(LEDPin, HIGH);
  delay(250);
  digitalWrite(LEDPin, LOW);
  delay(250);

  digitalWrite(LEDPin, HIGH);
  delay(250);
  digitalWrite(LEDPin, LOW);
  delay(250);


  client.publish(topic, "MOSTAFA NOURI");
  delay(2500);
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}