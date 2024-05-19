#include "secrets.h"  // Include secrets file for WiFi credentials
#include <WiFiNINA.h> // include library for handling WiFi with NINA modules
#include <PubSubClient.h> // MQTT library for handling MQTT communications
#include <HCSR04.h> // library for handling the HC-SR04 ultrasonic sensor

// Pins for ultrasonic sensor
byte triggerPin = 2; // Pin used to trigger the sensor
byte echoPin = 3; // Pin used to receive the echo
double threshold = 50; // Distance threshold in centimeters

int LEDPin = 10; // LED pin

// WiFi credentials from the secrets file
char ssid[] = SECRET_SSID; 
char pass[] = SECRET_PASS;

// MQTT Broker information
const char *mqtt_broker = "broker.emqx.io"; // Address of the MQTT broker
const char *topic = "SIT210"; // MQTT topic for subscribing or publishing
const char *mqtt_username = "emqx"; // MQTT broker username
const char *mqtt_password = "public"; // MQTT broker password
const int mqtt_port = 1883; // MQTT port

WiFiClient wifiClient; // WiFi client instance for Internet access
PubSubClient client(wifiClient); // MQTT client instance

void MQTT_Connect() {
  // Ensure the MQTT client is connected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Try to connect with the MQTT broker
    if (client.connect("arduinoNano33", mqtt_username, mqtt_password)) {
      Serial.println("connected");
      // Once connected, subscribe to the specified MQTT topic
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // If connection fails, retry after 5 seconds
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LEDPin, OUTPUT); // Set LED pin as output
  digitalWrite(LEDPin, LOW); // Turn off LED initially
  Serial.begin(115200); // Begin serial communication at 115200 baud rate
  while (!Serial) {
    ; // Wait for the serial port to connect. Necessary for boards like Leonardo
  }
  
  // Connecting to WiFi
  int status = WiFi.begin(ssid, pass); // Connect to WiFi network
  while (status != WL_CONNECTED) { // Retry until connected
    Serial.print("Connecting to ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(5000); // Retry every 5 seconds
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // Print the assigned IP address
  
  client.setServer(mqtt_broker, mqtt_port); // Set the MQTT server
  client.setCallback(callback); // Set the callback function for MQTT messages
  MQTT_Connect(); // Connect to MQTT broker

  // Ultrasonic sensor setup
  HCSR04.begin(triggerPin, echoPin);
}

void loop() {
  double* distances = HCSR04.measureDistanceCm(); // Measure distance
  double distance = distances[0]; // Get the first measured distance
  if (distance < threshold && distance > 0){
    delay(100); // Debounce delay
    double* second_distances = HCSR04.measureDistanceCm(); // Measure distance again
    double second_distance = distances[0];
    if (second_distance > threshold){ Wave(); } // Trigger 'Wave' action
    else if (distance > second_distance) { Pat(); } // Trigger 'Pat' action
  }

  client.loop(); // Handle MQTT client loop
}

void Wave (){
  // Function to control LED in a 'wave' pattern and publish a message
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

  client.publish(topic, "MOSTAFA NOURI"); // Publish a name to the MQTT topic
  delay(2500);
}

void Pat(){
  // Function to control LED in a 'pat' pattern
  digitalWrite(LEDPin, HIGH);
  delay(1000);
  digitalWrite(LEDPin, LOW);
  delay(1000);

  digitalWrite(LEDPin, HIGH);
  delay(1000);
  digitalWrite(LEDPin, LOW);
  delay(1000);

  client.publish(topic, "MOSTAFA NOURI"); // Publish a name to the MQTT topic
  delay(2500);
}

void callback(char* topic, byte* payload, unsigned int length) {
    // Callback function for received MQTT messages
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}
