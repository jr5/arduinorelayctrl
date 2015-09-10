
// Basic 4 Realy board connection
// Each RelayControl is turned on for 2 seconds and then off.
// You can here them click as there state changes from off to on and on to
// off.
// You will also see the corresponding Red LED on the 4 RelayControl board
// light up when the RelayControl is on.
 
 //  define names for the 4 Digital pins On the Arduino 7,8,9,10
 //  These data pins link to 4 RelayControl board pins IN1, IN2, IN3, IN4
#include "SerialCommand.h"

//Switch Definitions - Pins OUT to Relay Board IN Pins
#define RelayControl0  6                        
#define RelayControl1  7                        
#define RelayControl2  8                        
#define RelayControl3  9

#define ButtonReadPin0 5   // Input pin for pushbotton Relay 0
#define ButtonReadPin1 4   // Input pin for pushbotton Relay 1
#define ButtonReadPin2 3   // Input pin for pushbotton Relay 2
#define ButtonReadPin3 2   // Input pin for pushbotton Relay 3

//Push Button


byte m_relay_indexes[ ] = {RelayControl0, RelayControl1, RelayControl2, RelayControl3};
byte m_button_indexes[] = {ButtonReadPin0, ButtonReadPin1,ButtonReadPin2,ButtonReadPin3};

SerialCommand sCmd;     // The demo SerialCommand object
byte m_switch_states[] = { 0,0,0,0};

//Button Definitions
//int ledPin = 13; // choose the pin for the LED


int val = 0;     // private: variable for reading the pin status


//int lastButtonState = LOW;   // the previous reading from the input pin
int m_last_button_state[] =  { LOW, LOW, LOW, LOW };
// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
//long lastDebounceTime = 0;  // the last time the output pin was toggled
int m_last_debounce_time[] = { 0,0,0,0};
int m_button_state[] = {0,0,0,0};// the current reading from the input pin (per switch)
long debounceDelay = 200;    // the debounce time; increase if the output flickers

void setup()
{    
  Serial.begin(9600); 
  // Initialise the Arduino data pins for OUTPUT
  pinMode(RelayControl0, OUTPUT);       
  pinMode(RelayControl1, OUTPUT);
  pinMode(RelayControl2, OUTPUT);
  pinMode(RelayControl3, OUTPUT);
    
  
    // Setup callbacks for SerialCommand commands
  sCmd.addCommand("get",    get_handler);          // Turns LED on
  sCmd.addCommand("ON",    LED_on);          // Turns LED on
  sCmd.addCommand("OFF",   LED_off);         // Turns LED off
  sCmd.addCommand("help", help_handler);        // Echos the string argument back
  sCmd.addCommand("switch", switch_handler); 
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
  Serial.println("Ready");
}
 
 void loop()
{
  
    sCmd.readSerial();     // We don't do much, just process serial commands
    monitorButton(0);
    monitorButton(1);
}
 
void monitorButton(int pSwitchIndex)
{
 // read the state of the switch into a local variable:
  int reading = digitalRead(m_button_indexes[pSwitchIndex]);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != m_last_button_state[pSwitchIndex]) {
    // reset the debouncing timer
    m_last_debounce_time[pSwitchIndex] = millis();
  }
 
  if ((millis() - m_last_debounce_time[pSwitchIndex]) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != m_button_state[pSwitchIndex]) {
      m_button_state[pSwitchIndex] = reading;

      // only toggle the LED if the new button state is HIGH
      if (m_button_state[pSwitchIndex] == HIGH) {
        SetSwitchState(pSwitchIndex,!m_switch_states[pSwitchIndex]);

      }

    }
  }
 
  // set the LED:

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  //lastButtonState = reading;
  m_last_button_state[pSwitchIndex] = reading;
}
 
 void SetSwitchState(int pSwitchIndex, int pSwitchState)
 {
   m_switch_states[pSwitchIndex] = pSwitchState;
   digitalWrite(m_relay_indexes[pSwitchIndex],m_switch_states[pSwitchIndex]);  
 }
 
 
 
 //Serial Command Callback functions
 
 //Qucky ON Switch 0
 void LED_on() {
  Serial.println("Turn On."); 
//  m_switch_states[0] = LOW;
  SetSwitchState(0,LOW);
}

 //Qucky OFF Switch 1
void LED_off() {
  Serial.println("Turn Off."); 
//  m_switch_states[0] = HIGH;
  SetSwitchState(0,HIGH);
}

void get_handler() {
  Serial.print("{");
  for(int i=0;i<4;i++)
  {
    if(i!=0){Serial.print(",");}
    
    Serial.print(m_switch_states[i]);
  }
  Serial.println("}");
}

void help_handler() {
    Serial.println("Hello, whoever you are");
}

void switch_handler(){
  int pSwitchNumber, pCmdSwitchState;
  char *arg;
  arg = sCmd.next();
  Serial.print("switch ");
  if (arg != NULL) {
    pSwitchNumber = atoi(arg);    // Converts a char string to an integer
    Serial.print(pSwitchNumber);
    Serial.print(" ");
    switch(pSwitchNumber)
    {
      case 0:
      case 1:
      case 2:
      case 3:
          arg = sCmd.next();    // Get the next argument from the SerialCommand object buffer
          if (arg != NULL) {    // As long as it existed, take it
            pCmdSwitchState = atoi(arg);    // Converts a char string to an integer
            switch(pCmdSwitchState)
            {
              case 0:
               digitalWrite(m_relay_indexes[pSwitchNumber],LOW);          // Turns RelayControl Off 
               m_relay_indexes[pSwitchNumber] = LOW;
              break;
              case 1:
               digitalWrite(m_relay_indexes[pSwitchNumber],HIGH);          // Turns RelayControl On 
               m_relay_indexes[pSwitchNumber] = HIGH;
              break;
              default:
                  Serial.print("error: bad switch command 0-1 for switch state ");
              break;
            }
            Serial.println(pCmdSwitchState);
          }
          else {
            Serial.println("error: no state");
          }
          break;
      
      default:
        Serial.println("error: switch must be 0-3");
      return;
    }
  }
  else {
    Serial.println("error: no switch specified");
  }
}

// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("What? try help");
}
