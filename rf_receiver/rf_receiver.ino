#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

/*

ON

1 - 1135923
2 - 1136067
3 - 1136387
4 - 1137923
5 - 1144067

OFF

1 - 1135932
2 - 1136076
3 - 1136396
4 - 1137932
5 - 1144076

 
 */
void setup() {
  Serial.begin(9600);
  Serial.println("setup");
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
}

void loop() {
  if (mySwitch.available()) {
    
    int value = mySwitch.getReceivedValue();
    
    if (value == 0) {
      Serial.print("Unknown encoding");
    } else {
      Serial.print("Received ");
      Serial.print( mySwitch.getReceivedValue() );
      Serial.print(" / ");
      Serial.print( mySwitch.getReceivedBitlength() );
      Serial.print("bit ");
      Serial.print("Protocol: ");
      Serial.println( mySwitch.getReceivedProtocol() );
    }

    mySwitch.resetAvailable();
  }
}
