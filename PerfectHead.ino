/**
 * Perfect Head Backend
 * (c) 2017 Team Nyan
 */

//PIN NOTES: When we add a second tap, the pins will be assigned for the second
//tap starting with 12 (i.e. 'lift_contact_2' will be 12, 'tilt_contact_2' 13, etc.)
//This means that we can simply use an id number to specify which sensor or motor
//to use, for example:
//    digitalWrite(tilt_out + 10*id, HIGH);
//The code for the analog sensor will be similar:
//    int value = analogRead(glass_sensor + id);

//Contact pins
const int lift_contact = 2;
const int tilt_contact = 3;

//Motor pins
const int lift_up = 4;
const int lift_down = 5;
const int tilt_out = 6;
const int tilt_in = 7;
const int tap_on = 8;
const int tap_off = 9;

//Sensor pins
const int tilt_sensor = 10;
const int off_sensor = 11;

//Proximity sensor (analog)
const int glass_sensor = 0;

//Input from Bluetooth
int input;

void setup() {
  //Set motor pins as output
  pinMode(lift_up, OUTPUT);
  pinMode(lift_down, OUTPUT);
  pinMode(tilt_out, OUTPUT);
  pinMode(tilt_in, OUTPUT);
  pinMode(tap_on, OUTPUT);
  pinMode(tap_off, OUTPUT);

  //Set liquid level sensors
  pinMode(tilt_sensor, INPUT);
  pinMode(off_sensor, INPUT);

  //Initialize serial communications (Bluetooth module)
  Serial.begin(9600);
}

void loop() {
  //Check for Bluetooth message
  input = checkMessages();

  switch(input)
    //If the message says pour a beer
    case 0:
    case 1: 
      //Lift until glass comes into range
      if(analogRead(glass_sensor+input) > 0){
        digitalWrite(lift_up + 10*input, HIGH);
        while(analogRead(glass_sensor+input) > 0){
          if(checkMessages >= 10){
            reset(input);
            break;
          }
        }
        digitalWrite(lift_up + 10*input, LOW);
      }
      
      //Tilt out
      digitalWrite(tilt_out + 10*input, HIGH);
      //TODO: Determine tilt time
      digitalWrite(tilt_out + 10*input, LOW);
      
      //Tap on
      digitalWrite(tap_on + 10*input, HIGH);
      delay(500);
      if(checkMessages() >= 10){ reset(input); break; }
      digitalWrite(tap_on + 10*input, LOW);
      
      //When tilt sensor is triggered, tilt in
      while(!digitalRead(tilt_sensor + 10*input)){ if(checkMessages() >= 10){ reset(input); break; }}
      digitalWrite(tilt_in, HIGH);
      while(!digitalRead(tilt_contact + 10*input)){ if(checkMessages() >= 10){ reset(input); break; }}
      digitalWrite(tilt_in, LOW);
      
      //When off sensor is triggered, tap off
      while(!digitalRead(off_sensor + 10*input)){ if(checkMessages() >= 10){ reset(input); break; }}
      digitalWrite(tap_off + 10*input, HIGH);
      delay(500);
      if(checkMessages() == 9){ reset(input); break; }
      digitalWrite(tap_off + 10*input, LOW);
      
      //Lower until lift at bottom
      digitalWrite(lift_down, HIGH);
      while(!digitalRead(lift_contact)){ if(checkMessages() >= 10){ reset(input); break; }}
      digitalWrite(lift_down, LOW);
    case 10:
    case 11:
      reset(input);
      break;
}

//Returns 0 for the first beer, 1 for the second, 10 to reset the first beer, and 11 to reset the second
int checkMessages(){
  //TODO: Check for bluetooth messages
}

void reset(int id){
  //Tap off
  digitalWrite(tap_off + 10*id, HIGH);
  delay(500);
  digitalWrite(tap_off + 10*id, LOW);
  
  //Tilt in
  digitalWrite(tilt_in + 10*id, HIGH);
  while(!digitalRead(tilt_contact + 10*id)){ /* wait */ }
  digitalWrite(tilt_in + 10*id, LOW);

  //Lower lift
  digitalWrite(lift_down + 10*id, HIGH);
  while(!digitalRead(lift_contact + 10*id)){ /* wait */ }
  digitalWrite(lift_down + 10*id, LOW);
}

