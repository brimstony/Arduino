/*************************************************** 
  Biometric Box Sketch for the optical Fingerprint sensor
  This sketch implements a two-level security scheme requiring the
  user to enter a passcode via the keypad before scanning a fingerprint
  for access.
 
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!
 
  Written by Bill Earl for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/
#include <Keypad.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
 
// Define the states for the lock state machine
#define LOCKED 2
#define PASSWORD_OK 1
#define UNLOCKED 0
 
// State Variables:   Initialize to the locked state
int LockState = LOCKED;
long StartTime = 0;
int position = 0;
 
// Define your password key sequence here
char* secretCode = "1423";
 
// Keypad key matrix:
const byte rows = 4; 
const byte cols = 3; 
char keys[rows][cols] = 
{
   {'1','2','3'},
   {'4','5','6'},
   {'7','8','9'},
   {'*','0','#'}
};
 
// Keypad pin definitions
byte rowPins[rows] = {2, 3, 4, 5}; 
byte colPins[cols] = {6, 7, 8};  
 
// Instantiate the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);
 
// More pin definitions:
int LedPin = 10;
int SolenoidPin = 11;
 
// Define a Fingerprint sensor on pins 12 & 13
int getFingerprintIDez();
SoftwareSerial mySerial(13, 12);
 
void setup()                    
{
   pinMode(LedPin, OUTPUT);
   pinMode(SolenoidPin, OUTPUT);
   
   // Flash hello
   for (int i = 0; i < 3; i++)
   {
     digitalWrite(LedPin, HIGH);
     delay(100);
     digitalWrite(LedPin, LOW);
     delay(100);
   }
   
   // Initialize state and communicatins
   setLockState(LOCKED); 
   Serial.begin(9600); 

}
 
 
void loop()                    
{
   // Run the state machine:
   
   // Locked State - Monitor keypad for valid Password code entry
   if (LockState == LOCKED)
   {
      char key = keypad.getKey();
 
      if (key == '*' || key == '#')
      {
         position = 0;
         setLockState(LOCKED);
      }
      if (key != 0)
      {
         if (key == secretCode[position])  // Valid key in Password sequence
         {
            Serial.print("Matched ");   
            Serial.print(key);   
            Serial.print("-at-");   
            Serial.println(position);   
            position ++;
         }
         else  // Invalid key - start all over again
         {
            Serial.println("Invalid Code!");   
            position = 0;
         }
      }
 
      // Let the LED 'breathe' while we wait
      analogWrite(LedPin, sin((millis() % 3142) / 1000.0) * 255);
 
      if (position == 4)  // Password successfully entered - advance state
      {
         setLockState(PASSWORD_OK);
         position = 0;
      }
      delay(100);
   }
 
   // PASSWORD_OK state - Now wait for a valid fingerprint reading
   else if (LockState == PASSWORD_OK)
   { 
      setLockState(UNLOCKED); // Valid fingerprint - advance state to UNLOCKED
     
      if (millis () - StartTime > 5000) 
      {
         setLockState (LOCKED); // Time-out - go back to the LOCKED state
      }
   }
 
   // UNLOCKED state - hold the solenoid open for a limited time
   else if (LockState == UNLOCKED)
   { 
      for (int i = 0; i < 30; i++)
      {
         // Flash the led to indicate the lock is open
         digitalWrite(LedPin, LOW);
         delay(50);
         digitalWrite(LedPin, HIGH);
         delay(50);
      }
      setLockState (LOCKED);  // Time-out - go back to the locked state.
   }
}
 
 
 
// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() 
{
   uint8_t p = finger.getImage();
   if (p != FINGERPRINT_OK)  return -1;
 
   p = finger.image2Tz();
   if (p != FINGERPRINT_OK)  return -1;
 
   p = finger.fingerFastSearch();
   if (p != FINGERPRINT_OK)  return -1;
 
   // found a match!
   Serial.print("Found ID #"); Serial.print(finger.fingerID); 
   Serial.print(" with confidence of "); Serial.println(finger.confidence);
   return finger.fingerID; 
}
 
 
// Set the state and the time of the state change
void setLockState(int state)
{
   LockState = state;
   StartTime = millis ();
   if (state == LOCKED)
   {
      Serial.println("Locked!");
      digitalWrite(LedPin, HIGH);
      digitalWrite(SolenoidPin, LOW);  
   }
 
   else if (state == PASSWORD_OK)
   {
      Serial.println("PASSWORD_OK!");
      digitalWrite(LedPin, LOW);  
   }    
   else if (state == UNLOCKED)
   {
      Serial.println("Unlocked!");
      digitalWrite(LedPin, LOW);
      digitalWrite(SolenoidPin, HIGH);      
   }
}
