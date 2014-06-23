/* 
 Created by Andrew Payne
 Modified from original Firefly Firmata written by Jason Kelly Johnson and Andrew Payne
 Latest Update September 10th, 2012 
 Copyright 2012 | All Rights Reserved
 
 This Firmata allows you to control an Arduino board from Rhino/Grasshopper/Firefly.
 Updates, Questions, Suggestions visit: http://www.fireflyexperiments.com
 
 1. Plug Arduino boards into your USB port; confirm that your Arduino's green power LED in on
 2. Select your specific Arduino Board and Serial Port (Tools > Board; Tools > Serial Port) *Take note of your Serial Port COM #
 3. Verify (play button) and Upload (upload button) this program to your Arduino, close the Arduino program
 4. then open ... Rhino/Grasshopper/Firefly
 
 Note: The Firefly Firmata sets the following pins to perform these functions:
 
 *****ON STANDARD BOARDS (ie. Uno, Diecimila, Duemilanove, Lillypad, Mini, etc.)*****
 ANALOG IN pins 0-5 are set to return values (from 0 to 1023) for analog sensors
 DIGITAL IN pins 2,4,7 will return 0's or 1's; for 3 potential digital sensors (buttons, switches, on/off, true/false, etc.)
 DIGITAL/ANALOG OUT pins 3,5,6,11 (marked with a ~) can be used to digitalWrite, analogWrite, or Servo.write depending on the input status of that Firefly pin
 DIGITAL OUT pins 8,9,10,12,13 can be used to digitalWrite, Servo.write, or analogWrite depending on the input status of that Firefly pin
 
 *****ON MEGA BOARDS (ie. ATMEGA1280, ATMEGA2560)*****
 ANALOG IN pins 0-15 will return values (from 0 to 1023) for 16 potential analog sensors 
 DIGITAL IN pins 34-41 will return 0's or 1's; for 8 potential digital sensors (buttons, switches, on/off, true/false, etc.) 
 DIGITAL/ANALOG OUT pins 2-13 can be used to digitalWrite, analogWrite, or Servo.write depending on the input status of that Firefly pin
 DIGITAL OUT pins 22-33 can be used to digitalWrite, Servo.write, or analogWrite depending on the input status of that Firefly pin
 
 *****ON LEONARDO BOARDS*****
 ANALOG IN pins 0-5 are set to return values (from 0 to 1023) for analog sensors
 DIGITAL IN pins 2,4,7 will return 0's or 1's; for 3 potential digital sensors (buttons, switches, on/off, true/false, etc.)
 DIGITAL/ANALOG OUT pins 3,5,6,11 (marked with a ~) can be used to digitalWrite, analogWrite, or Servo.write depending on the input status of that Firefly pin
 DIGITAL OUT pins 8,9,10,12,13 can be used to digitalWrite, Servo.write, or analogWrite depending on the input status of that Firefly pin
 */

#include <Servo.h>            // attach Servo library (http://www.arduino.cc/playground/ComponentLib/Servo)
#include <pins_arduino.h>     // attach arduino pins header file to determine which board type is being used

#define BAUDRATE 115200       // Set the Baud Rate to an appropriate speed
#define BUFFSIZE 512          // buffer one command at a time, 12 bytes is longer than the max length

/*==============================================================================
 * GLOBAL VARIABLES
 *============================================================================*/

char buffer[BUFFSIZE];        // declare buffer
uint8_t bufferidx = 0;        // a type of unsigned integer of length 8 bits
char *parseptr;
char buffidx;

int counter = 0;
int numcycles = 1000;

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)                         // declare variables for STANDARD boards
  uint16_t APin0, APin1, APin2, APin3, APin4, APin5, DPin2, DPin4, DPin7;             // declare input variables
  uint16_t DPin3, DPin5, DPin6, DPin8, DPin9, DPin10, DPin11, DPin12, DPin13;         // declare output variables  
  Servo Servo3, Servo5, Servo6, Servo8, Servo9, Servo10, Servo11, Servo12, Servo13;   // declare Servo objects
#endif

#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)                        // declare variables for LEONARDO board
  uint16_t APin0, APin1, APin2, APin3, APin4, APin5, DPin2, DPin4, DPin7;             // declare input variables
  uint16_t DPin3, DPin5, DPin6, DPin8, DPin9, DPin10, DPin11, DPin12, DPin13;         // declare output variables  
  Servo Servo3, Servo5, Servo6, Servo8, Servo9, Servo10, Servo11, Servo12, Servo13;   // declare Servo objects
#endif

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)                        // declare variables for MEGA boards
  uint16_t APin0, APin1, APin2, APin3, APin4, APin5, APin6, APin7, APin8, APin9, APin10, APin11, APin12, APin13, APin14, APin15, DPin22, DPin23, DPin24, DPin25, DPin26, DPin27, DPin28, DPin29, DPin30, DPin31;  // declare input variables
  uint16_t DPin2, DPin3, DPin4, DPin5, DPin6, DPin7, DPin8, DPin9, DPin10, DPin11, DPin12, DPin13, DPin32, DPin33, DPin34, DPin35, DPin36, DPin37, DPin38, DPin39, DPin40, DPin41, DPin42, DPin43, DPin44, DPin45, DPin46, DPin47, DPin48, DPin49, DPin50, DPin51, DPin52, DPin53;  // declare output variables  
  Servo Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9, Servo10, Servo11, Servo12, Servo13, Servo32, Servo33, Servo34, Servo35, Servo36, Servo37, Servo38, Servo39, Servo40, Servo41, Servo42, Servo43, Servo44, Servo45, Servo46, Servo47, Servo48, Servo49, Servo50, Servo51, Servo52, Servo53;  // declare Servo objects
#endif


/*==============================================================================
 * SETUP() This code runs once
 *============================================================================*/
void setup()
{ 
  Init();                  //set initial pinmodes
  Serial.begin(BAUDRATE);  // Start Serial communication
}

/*==============================================================================
 * LOOP() This code loops
 *============================================================================*/
void loop()
{
  if(Serial){
    ReadSerial();                       // read and parse string from serial port and write to pins
    if (counter >= numcycles){          // Wait every nth loop 
      ReadInputs();                     // get input data
      PrintToPort();                    // print data to serial port 
      counter = 0;                      // reset the counter
    }
    counter ++;                         // increment the writecounter
  }
}

/*==============================================================================
 * FUNCTIONS()
 *============================================================================*/

void Init(){
  #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)    //set pin mode for STANDARD boards
    pinMode(2, INPUT); 
    pinMode(4, INPUT); 
    pinMode(7, INPUT);
  #endif
  
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)   //set pin mode for LEONARDO board
    pinMode(2, INPUT); 
    pinMode(4, INPUT); 
    pinMode(7, INPUT);
  #endif
  
  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)  //set pin mode for MEGA boards
    pinMode(22, INPUT); 
    pinMode(23, INPUT);
    pinMode(24, INPUT);
    pinMode(25, INPUT);
    pinMode(26, INPUT); 
    pinMode(27, INPUT);
    pinMode(28, INPUT); 
    pinMode(29, INPUT);
    pinMode(30, INPUT);
    pinMode(31, INPUT); 
  #endif
}

void ReadInputs(){ 
  #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)    //read pins on STANDARD boards
    APin0 = analogRead(0);   
    APin1 = analogRead(1);  
    APin2 = analogRead(2); 
    APin3 = analogRead(3); 
    APin4 = analogRead(4); 
    APin5 = analogRead(5); 
    DPin2 = digitalRead(2);  
    DPin4 = digitalRead(4);  
    DPin7 = digitalRead(7); 
  #endif
  
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)   //read pins on LEONARDO board
    APin0 = analogRead(0);   
    APin1 = analogRead(1);  
    APin2 = analogRead(2); 
    APin3 = analogRead(3); 
    APin4 = analogRead(4); 
    APin5 = analogRead(5); 
    DPin2 = digitalRead(2);  
    DPin4 = digitalRead(4);  
    DPin7 = digitalRead(7); 
  #endif
  
  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)  //read pins on MEGA boards
    APin0 = analogRead(0); 
    APin1 = analogRead(1); 
    APin2 = analogRead(2); 
    APin3 = analogRead(3); 
    APin4 = analogRead(4); 
    APin5 = analogRead(5); 
    APin6 = analogRead(6); 
    APin7 = analogRead(7); 
    APin8 = analogRead(8); 
    APin9 = analogRead(9); 
    APin10 = analogRead(10); 
    APin11 = analogRead(11); 
    APin12 = analogRead(12); 
    APin13 = analogRead(13); 
    APin14 = analogRead(14); 
    APin15 = analogRead(15); 
    DPin22 = digitalRead(22);  
    DPin23 = digitalRead(23);  
    DPin24 = digitalRead(24);  
    DPin25 = digitalRead(25);  
    DPin26 = digitalRead(26); 
    DPin27 = digitalRead(27); 
    DPin28 = digitalRead(28);  
    DPin29 = digitalRead(29);  
    DPin30 = digitalRead(30);  
    DPin31 = digitalRead(31);
  #endif
}

void PrintToPort(){
  #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)    //print formatted string for STANDARD boards
    Serial.print(APin0); Serial.print(",");  
    Serial.print(APin1); Serial.print(","); 
    Serial.print(APin2); Serial.print(","); 
    Serial.print(APin3); Serial.print(","); 
    Serial.print(APin4); Serial.print(","); 
    Serial.print(APin5); Serial.print(","); 
    Serial.print(DPin2); Serial.print(","); 
    Serial.print(DPin4); Serial.print(","); 
    Serial.print(DPin7); Serial.print(","); 
    Serial.println("eol");  //end of line marker
  #endif
  
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)   //print formatted string for LEONARDO boards
    Serial.print(APin0); Serial.print(",");  
    Serial.print(APin1); Serial.print(","); 
    Serial.print(APin2); Serial.print(","); 
    Serial.print(APin3); Serial.print(","); 
    Serial.print(APin4); Serial.print(","); 
    Serial.print(APin5); Serial.print(","); 
    Serial.print(DPin2); Serial.print(","); 
    Serial.print(DPin4); Serial.print(","); 
    Serial.print(DPin7); Serial.print(","); 
    Serial.println("eol");  //end of line marker
  #endif
  
  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)  //print formatted string for MEGA boards
    Serial.print(APin0); Serial.print(","); 
    Serial.print(APin1); Serial.print(","); 
    Serial.print(APin2); Serial.print(","); 
    Serial.print(APin3); Serial.print(",");   
    Serial.print(APin4); Serial.print(","); 
    Serial.print(APin5); Serial.print(",");   
    Serial.print(APin6); Serial.print(",");
    Serial.print(APin7); Serial.print(",");  
    Serial.print(APin8); Serial.print(",");  
    Serial.print(APin9); Serial.print(","); 
    Serial.print(APin10); Serial.print(",");  
    Serial.print(APin11); Serial.print(","); 
    Serial.print(APin12); Serial.print(","); 
    Serial.print(APin13); Serial.print(","); 
    Serial.print(APin14); Serial.print(",");  
    Serial.print(APin15); Serial.print(","); 
    Serial.print(DPin22); Serial.print(","); 
    Serial.print(DPin23); Serial.print(","); 
    Serial.print(DPin24); Serial.print(",");
    Serial.print(DPin25); Serial.print(",");   
    Serial.print(DPin26); Serial.print(","); 
    Serial.print(DPin27); Serial.print(",");  
    Serial.print(DPin28); Serial.print(","); 
    Serial.print(DPin29); Serial.print(","); 
    Serial.print(DPin30); Serial.print(","); 
    Serial.print(DPin31); Serial.print(",");  
    Serial.println("eol");  //end of line marker
  #endif
}

void ReadSerial(){

  char c;    // holds one character from the serial port
  if (Serial.available()) {
    c = Serial.read();         // read one character
    buffer[bufferidx] = c;     // add to buffer

    if (c == '\n') {  
      buffer[bufferidx+1] = 0; // terminate it
      parseptr = buffer;       // offload the buffer into temp variable

      #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)  //parse string for STANDARD boards
        DPin13 = parsedecimal(parseptr);     // parse the first number
        parseptr = strchr(parseptr, ',')+1;  // move past the ","
        
        DPin12 = parsedecimal(parseptr);     // parse the second number
        parseptr = strchr(parseptr, ',')+1;  // move past the ","
  
        DPin11 = parsedecimal(parseptr);     // parse the third number
        parseptr = strchr(parseptr, ',')+1;  // move past the ","
  
        DPin10 = parsedecimal(parseptr);     // parse the fourth number
        parseptr = strchr(parseptr, ',')+1;  // move past the ","
  
        DPin9 = parsedecimal(parseptr);      // parse the fifth number
        parseptr = strchr(parseptr, ',')+1;  // move past the ","
  
        DPin8 = parsedecimal(parseptr);      // parse the sixth number
        parseptr = strchr(parseptr, ',')+1;  // move past the ","
  
        DPin6 = parsedecimal(parseptr);      // parse the seventh number
        parseptr = strchr(parseptr, ',')+1;  // move past the ","
  
        DPin5 = parsedecimal(parseptr);      // parse the eighth number
        parseptr = strchr(parseptr, ',')+1;  // move past the ","
  
        DPin3 = parsedecimal(parseptr);      // parse the ninth number
        
        WriteToPin(13, DPin13, Servo13);     //send value out to pin on arduino board
        WriteToPin(12, DPin12, Servo12);
        WriteToPin(11, DPin11, Servo11);
        WriteToPin(10, DPin10, Servo10);
        WriteToPin(9, DPin9, Servo9);
        WriteToPin(8, DPin8, Servo8);
        WriteToPin(6, DPin6, Servo6);
        WriteToPin(5, DPin5, Servo5);
        WriteToPin(3, DPin3, Servo3);
         
      #endif
      
      #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)  //parse string for LEONARDO boards
        DPin13 = parsedecimal(parseptr);     // parse the first number
        parseptr = strchr(parseptr, ',')+1;  // move past the ","
  
        DPin12 = parsedecimal(parseptr);     // parse the second number
        parseptr = strchr(parseptr, ',')+1;  // move past the ","
  
        DPin11 = parsedecimal(parseptr);     // parse the third number
        parseptr = strchr(parseptr, ',')+1;  // move past the ","
  
        DPin10 = parsedecimal(parseptr);     // parse the fourth number
        parseptr = strchr(parseptr, ',')+1;  // move past the ","
  
        DPin9 = parsedecimal(parseptr);      // parse the fifth number
        parseptr = strchr(parseptr, ',')+1;  // move past the ","
  
        DPin8 = parsedecimal(parseptr);      // parse the sixth number
        parseptr = strchr(parseptr, ',')+1;  // move past the ","
  
        DPin6 = parsedecimal(parseptr);      // parse the seventh number
        parseptr = strchr(parseptr, ',')+1;  // move past the ","
  
        DPin5 = parsedecimal(parseptr);      // parse the eighth number
        parseptr = strchr(parseptr, ',')+1;  // move past the ","
  
        DPin3 = parsedecimal(parseptr);      // parse the ninth number
        
        WriteToPin(13, DPin13, Servo13);     //send value out to pin on arduino board
        WriteToPin(12, DPin12, Servo12);
        WriteToPin(11, DPin11, Servo11);
        WriteToPin(10, DPin10, Servo10);
        WriteToPin(9, DPin9, Servo9);
        WriteToPin(8, DPin8, Servo8);
        WriteToPin(6, DPin6, Servo6);
        WriteToPin(5, DPin5, Servo5);
        WriteToPin(3, DPin3, Servo3);
        
      #endif
      
      #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)  //parse string for MEGA boards
        DPin2 = parsedecimal(parseptr);       // parse the first number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
  
        DPin3 = parsedecimal(parseptr);       // parse the second number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
  
        DPin4 = parsedecimal(parseptr);       // parse the third number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
  
        DPin5 = parsedecimal(parseptr);       // parse the fourth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
  
        DPin6 = parsedecimal(parseptr);       // parse the fifth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
  
        DPin7 = parsedecimal(parseptr);       // parse the sixth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
  
        DPin8 = parsedecimal(parseptr);       // parse the seventh number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
  
        DPin9 = parsedecimal(parseptr);       // parse the eighth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
  
        DPin10 = parsedecimal(parseptr);      // parse the ninth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin11 = parsedecimal(parseptr);      // parse the tenth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin12 = parsedecimal(parseptr);      // parse the eleventh number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin13 = parsedecimal(parseptr);      // parse the twelvth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin32 = parsedecimal(parseptr);      // parse the thirteenth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
  
        DPin33 = parsedecimal(parseptr);      // parse the fourteenth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin34 = parsedecimal(parseptr);      // parse the fifthteenth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin35 = parsedecimal(parseptr);      // parse the sixteenth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin36 = parsedecimal(parseptr);      // parse the seventeenth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin37 = parsedecimal(parseptr);      // parse the eightteenth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin38 = parsedecimal(parseptr);      // parse the nineteenth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin39 = parsedecimal(parseptr);      // parse the twentieth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin40 = parsedecimal(parseptr);      // parse the twenty first number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin41 = parsedecimal(parseptr);      // parse the twenty second number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin42 = parsedecimal(parseptr);      // parse the twenty third number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin43 = parsedecimal(parseptr);      // parse the twenty fourth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin44 = parsedecimal(parseptr);      // parse the twenty fifth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin45 = parsedecimal(parseptr);      // parse the twenty sixth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin46 = parsedecimal(parseptr);      // parse the twenty seventh number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin47 = parsedecimal(parseptr);      // parse the twenty eigth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin48 = parsedecimal(parseptr);      // parse the twenty ninth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin49 = parsedecimal(parseptr);      // parse the thirtieth number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin50 = parsedecimal(parseptr);      // parse the thirty one number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin51 = parsedecimal(parseptr);      // parse the thirty second number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin52 = parsedecimal(parseptr);      // parse the thirty third number
        parseptr = strchr(parseptr, ',')+1;   // move past the ","
        
        DPin53 = parsedecimal(parseptr);      // parse the thirty fourth number
        
        WriteToPin(2, DPin2, Servo2);         //send value out to pin on arduino board
        WriteToPin(3, DPin3, Servo3);
        WriteToPin(4, DPin4, Servo4);
        WriteToPin(5, DPin5, Servo5);
        WriteToPin(6, DPin6, Servo6);
        WriteToPin(7, DPin7, Servo7);
        WriteToPin(8, DPin8, Servo8);
        WriteToPin(9, DPin9, Servo9);
        WriteToPin(10, DPin10, Servo10);
        WriteToPin(11, DPin11, Servo11);
        WriteToPin(12, DPin12, Servo12);
        WriteToPin(13, DPin13, Servo13);
        WriteToPin(32, DPin32, Servo32);
        WriteToPin(33, DPin33, Servo33);
        WriteToPin(34, DPin34, Servo34);
        WriteToPin(35, DPin35, Servo35);
        WriteToPin(36, DPin36, Servo36);
        WriteToPin(37, DPin37, Servo37);
        WriteToPin(38, DPin38, Servo38);
        WriteToPin(39, DPin39, Servo39);
        WriteToPin(40, DPin40, Servo40);
        WriteToPin(41, DPin41, Servo41);
        WriteToPin(42, DPin42, Servo42);
        WriteToPin(43, DPin43, Servo43);
        WriteToPin(44, DPin44, Servo44);
        WriteToPin(45, DPin45, Servo45);
        WriteToPin(46, DPin46, Servo46);
        WriteToPin(47, DPin47, Servo47);
        WriteToPin(48, DPin48, Servo48);
        WriteToPin(49, DPin49, Servo49);
        WriteToPin(50, DPin50, Servo50);
        WriteToPin(51, DPin51, Servo51);
        WriteToPin(52, DPin52, Servo52);
        WriteToPin(53, DPin53, Servo53);
        
      #endif
      
      bufferidx = 0;                             // reset the buffer for the next read
      return;                                    // return so that we don't trigger the index increment below
    }                                            // didn't get newline, need to read more from the buffer
    bufferidx++;                                 // increment the index for the next character
    if (bufferidx == BUFFSIZE-1) bufferidx = 0;  // if we get to the end of the buffer reset for safety
  }
}

void WriteToPin(int _pin, int _value, Servo _servo){
if (_value >= 1000 && _value < 2000)             // check if value should be used for Digital Write (HIGH/LOW)
{      
  if (_servo.attached()) _servo.detach();        // detach servo is one is attached to pin
  pinMode(_pin, OUTPUT);                         // sets the pin for output 
  _value -=1000;                                 // subtract 1000 from the value sent from Firefly 
  if (_value == 1) digitalWrite(_pin, HIGH);     // Digital Write to pin
  else digitalWrite(_pin, LOW);   
}   
else if (_value >= 2000 && _value < 3000)        // check if value should be used for Analog Write (0-255)
{
  if (_servo.attached()) _servo.detach();        // detach servo is one is attached to pin
  pinMode(_pin, OUTPUT);                         // sets the pin for output 
  _value -= 2000;                                // subtract 2000 from the value sent from Firefly
  analogWrite(_pin, _value);                     // Analog Write to pin
}
else if (_value >= 3000 && _value < 4000)        // check if value should be used for Servo Write (0-180)
{
  _value -= 3000;                                // subtract 3000 from the value sent from Firefly
  if (!_servo.attached())_servo.attach(_pin);    // attaches a Servo to the PWM pin (180 degree standard servos)                                    
  _servo.write(_value);                          // Servo Write to the pin
  }
}

uint32_t parsedecimal(char *str){
  uint32_t d = 0;
  while (str[0] != 0) {
    if ((str[0] > '50') || (str[0] < '0'))
      return d;
    d *= 10;
    d += str[0] - '0';
    str++;
  }
  return d;
}





