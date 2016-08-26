/*
  Example for different sending methods

  https://github.com/sui77/rc-switch/

*/

#include <RCSwitch.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>



const char* mqtt_server = "192.168.1.10";
WiFiClient espClient;
PubSubClient client(espClient);
RCSwitch sendSwitch = RCSwitch();
bool outletOn = false;

// Array of ON/OFF codes. You must replace these with your codes obtained using a sniffer. This matches the 5 outlet remote.
unsigned long rc_codes[5][2] = {
  // ON     //OFF
  {1135923, 1135932}, /* Outlet 1 */
  {1136067, 1136076}, /* Outlet 2 */
  {1136387, 1136396}, /* Outlet 3 */
  {1137923, 1137932}, /* Outlet 4 */
  {1144067, 1144076}, /* Outlet 5 */
};

// The physical Arduino PIN (this will be called with pinMode()). Change this according to your board layout
#define RC_PIN_TX 0

#define RC_PROTOCOL 1
#define RC_PULSE_LENGTH 190 // 'Delay', if you got the right codes and this isn't working, check that the delay/pulse length from the sniffer matches this
#define RC_BIT_LENGTH 24

void callback(char* topic, byte* payload, long unsigned int length) {
  triggerOutlet(3, outletOn);
  outletOn = !outletOn;
  //Particle.publish(outletOn);
  char data[length];
  for (int i=0;i<length;i++) {
    data[i] = (char)payload[i];
  }
  Particle.publish("message", (char*)data);
}

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


void setup() {
  Particle.publish("setup");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  sendSwitch.enableTransmit(RC_PIN_TX);
  sendSwitch.setProtocol(RC_PROTOCOL); // defaults to 1 anyway
  sendSwitch.setPulseLength(RC_PULSE_LENGTH); // this is critical
}

void loop() {

  /* Same switch as above, but using decimal code */


  triggerOutlet(3, outletOn);
  outletOn = !outletOn;
    delay(10000);
}

void triggerOutlet(int outletNumber, bool turnOn) 
{
    if (outletNumber < 1 || outletNumber > 5)
  {
    Serial.println("Invalid outlet number");
    return;
  }
  unsigned long *onOffCodes = rc_codes[outletNumber - 1];
  unsigned long codeToSend = onOffCodes[turnOn ? 0 : 1];

  sendSwitch.send(codeToSend, RC_BIT_LENGTH);

}
