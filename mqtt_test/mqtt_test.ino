/*
 Basic MQTT example

 This sketch demonstrates the basic capabilities of the library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 
*/

#include <PubSubClient.h>
#include <ESP8266WiFi.h>

const char* mqtt_server = "192.168.1.10";

void callback(char* topic, byte* payload, long unsigned int length) {
  Particle.publish(topic);
  char data[length];
  for (int i=0;i<length;i++) {
    data[i] = (char)payload[i];
  }
  Particle.publish("message", (char*)data);
}

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Particle.publish("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Particle.publish("connected");
      // Once connected, publish an announcement...
      client.publish("test_topic","hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Particle.publish("failed, rc=");
//      Particle.publish(client.state());
      Particle.publish(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(57600);
  Particle.publish("setup");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  delay(1500);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
