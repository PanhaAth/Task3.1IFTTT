#include <WiFiNINA.h>
#include <PubSubClient.h>  // MQTT library
#include <BH1750FVI.h>

// WiFi Credentials
#define WIFI_SSID "Main Wifi"
#define WIFI_PASSWORD "Taing003"

// MQTT Broker Credentials
#define MQTT_SERVER "de14cfa721004843887935458c8bf7fe.s1.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_TOPIC "LightSensor"
#define MQTT_USER "hivemq.webclient.1742814873435"  // Replace with your HiveMQ username
#define MQTT_PASSWORD "8N!x%R25COjno0$z,YBv"  // Replace with your HiveMQ password

// Use SSL Client instead of regular WiFiClient
WiFiSSLClient wifiSSLClient;
PubSubClient mqttClient(wifiSSLClient);

// BH1750 Light Sensor
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);

// Function to Connect to MQTT Broker
void connectMQTT() {
   while (!mqttClient.connected()) {
       Serial.println("Connecting to MQTT...");
       if (mqttClient.connect("ArduinoNanoIoT", MQTT_USER, MQTT_PASSWORD)) {
           Serial.println("Connected to MQTT Broker!");
       } else {
           Serial.print("Failed, rc=");
           Serial.print(mqttClient.state());
           Serial.println(" Retrying in 5 seconds...");
           delay(5000);
       }
   }
}

void setup() {
   Serial.begin(115200);
   
   // Initialize Light Sensor
   LightSensor.begin();

   // Connect to WiFi
   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
   while (WiFi.status() != WL_CONNECTED) {
       delay(1000);
       Serial.println("Connecting to WiFi...");
   }
   Serial.println("Connected to WiFi");

   // Set MQTT Server
   mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  
   // Connect to MQTT
   connectMQTT();
}

void loop() {
   if (!mqttClient.connected()) {
       connectMQTT();
   }
   mqttClient.loop(); // Keep MQTT connection alive

   uint16_t lux = LightSensor.GetLightIntensity();
   Serial.print("Light: ");
   Serial.println(lux);

   if(lux >= 1000) {
       Serial.println("light detected");
       mqttClient.publish(MQTT_TOPIC, "light detected");
       while(lux >= 1000){
      lux = LightSensor.GetLightIntensity();
      delay(15000);
       }
   } else {
       Serial.println("No light");
       mqttClient.publish(MQTT_TOPIC, "No light detected");
       while(lux < 1000){
      lux = LightSensor.GetLightIntensity();
       delay(10000);
       }
   }
   delay(10000); // Send data every 10 seconds
}