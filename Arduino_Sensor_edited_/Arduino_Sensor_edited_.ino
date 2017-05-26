#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "AddicoreRFID.h"
#include <SPI.h>

#define  uchar unsigned char
#define uint  unsigned int

uchar fifobytes;
uchar fifoValue;

AddicoreRFID myRFID;

const int chipSelectPin = 10;
const int NRSTPD = 5;

//Maximum length of the array
#define MAX_LEN 16

LiquidCrystal_I2C lcd(0x27,16,2);

int led = 3;

int tim = 10;
int value_roof;
int percent_roof;
int value_water;
int percent_water;
int value_humidity;
int percent_humidity;

int a=0, b=0, receivedata=0;

const int analogInPin0 = A0;
const int analogInPin1 = A1;
const int analogInPin2 = A2;
 
float sensorValue[3] = {0,0,0};
float voltageValue[3] = {0,0,0};
 
char inbyte = 0,btnbyte;

int roofSensor, waterSensor;

/*
 * ::::::::::::::::::::::::::::::::::::::::::::::::::::::
 * ::::::::::::::::::::::::::::::::::::::::::::::::::::::
 * ::::::::::::::::::::::::::::::::::::::::::::::::::::::
 */


void setup(){
  Serial.begin(9600);
  initLCD();
  initRFID();
  
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
}
 
void loop() {
  
  readSensors();
  getVoltageValue();
  sendAndroidValues();
  workLCD();
  workRFID();
  btnbyte = Serial.read();

//  if(receivedata==1){
//    sendAndroidValues();
//  }
}

/*
 * ::::::::::::::::::::::::::::::::::::::::::::::::::::::
 * ::::::::::::::::::::::::::::::::::::::::::::::::::::::
 * ::::::::::::::::::::::::::::::::::::::::::::::::::::::
 */

//::::::::::::::::::::::::::RFID-COMMAND::::::::::::::::::::::::::::::::::
void withSecurity(void){
  if (btnbyte == '0'){
  //LED off
    digitalWrite(led, LOW);
    }
  if (btnbyte == '1'){
  //LED on
    digitalWrite(led, HIGH);
  }
//  if(btnbyte == 'd'){
//      digitalWrite(led, HIGH);
//      delay(500);
//      digitalWrite(led, LOW);
//      delay(500);
//      digitalWrite(led, HIGH);
//      delay(500);
//      digitalWrite(led, LOW);
//      delay(500);
//  }
}

void withoutSecurity(void){
//  Serial.print('T');
  if (btnbyte == '0'){
    digitalWrite(led, HIGH);
  }
  if (btnbyte == '1'){
    digitalWrite(led, LOW);
  }
//  if(btnbyte == 'd'){
//    digitalWrite(led, HIGH);
//    delay(2000);
//    digitalWrite(led, LOW);
//    delay(500);
//    digitalWrite(led, HIGH);
//    delay(2000);
//    digitalWrite(led, LOW);
//    delay(500);
//  }
//  if (btnbyte == 't'){
//    Serial.print('Y');
//  }
//  if (btnbyte == 'w'){
//    receivedata = receivedata+1;
//    Serial.print('U');
//  }
}
//::::::::::^^^^^^::::::::::RFID-COMMAND::::::::::::::^^^^^^::::::::::::::

//::::::::::::::::::::::::::SENSOR::::::::::::::::::::::::::::::::::
void readSensors(){
  // read the analog in value to the sensor array
  sensorValue[0] = analogRead(analogInPin0);
  sensorValue[1] = analogRead(analogInPin1);
  sensorValue[2] = analogRead(analogInPin2);
}

void getVoltageValue(){
  for (int x = 0; x < 3; x++){
    voltageValue[x] = ((sensorValue[x]/1023)*100);
  }
}

//sends the values from the sensor over serial to BT module
void sendAndroidValues(){
//  if(receivedata >= 1){
      //puts # before the values so our app knows what to do with the data
    Serial.print('#');
    //for loop cycles through 4 sensors and sends values via serial
    for(int k=0; k<3; k++){
      Serial.print(voltageValue[k]);
      Serial.print('+');
      //technically not needed but I prefer to break up data values
      //so they are easier to see when debugging
    }
    Serial.print('~'); //used as an end of transmission character - used in app for string length
    Serial.println();
    delay(10);        //added a delay to eliminate missed transmissions
//  }
}
//::::::::::^^^^^^::::::::::SENSOR::::::::::::::^^^^^^::::::::::::::
 
//::::::::::::::::::::::::::LCDD::::::::::::::::::::::::::::::::::
void initLCD(void) {
  lcd.init(); //initialize the lcd
  lcd.backlight(); //open the backlight 

  lcd.setCursor(0,0); // set the cursor to column 15, line 0
  lcd.print("Roof : "); // Print a message to the LCD.
  lcd.setCursor(0,1); // set the cursor to column 15, line 0
  lcd.print("Water: "); // Print a message to the LCD.
}

void workLCD(void) {
  value_roof=sensorValue[0];
  percent_roof=((value_roof/1023.0)*100);
  value_water=sensorValue[1];
  percent_water=((value_water/1023.0)*100);

  //Serial.print("Value Discrete = ");
  //Serial.print(value);
  //Serial.print("\t\t Percent = ");
  //Serial.println(percent);

  lcd.setCursor(7,0); // set the cursor to column 15, line 0
  lcd.print(percent_roof);// Print value of percent_roof
  lcd.print("%");

  lcd.setCursor(7,1); // set the cursor to column 15, line 0
  lcd.print(percent_water);// Print value of percent_water
  lcd.print("%");
  
  delay(tim); //wait for 250 microseconds
  
//  lcd.clear(); //Clears the LCD screen
  //delay(tim);

   if(percent_roof==30)//data roof dari fon
  {
    if(a == 0){
      lcd.setCursor(12,0);
      lcd.print("Done");
      a = 1;
    }
  }
  if(percent_water==60)//data water dari fon
  {
    if(b == 0){
      lcd.setCursor(12,1);
      lcd.print("Done");
      b = 1;
    }
  }
  else{
    lcd.setCursor(12,0);
    lcd.print("    ");
    lcd.setCursor(12,1);
    lcd.print("    ");
    a = 0;
    b = 0;
  }
}
//::::::::::^^^^^^::::::::::LCDD::::::::::::::^^^^^^::::::::::::::

//::::::::::::::::::::::::::RFID::::::::::::::::::::::::::::::::::
void initRFID(void){
  // start the SPI library:
  SPI.begin();
  
  pinMode(chipSelectPin,OUTPUT);              // Set digital pin 10 as OUTPUT to connect it to the RFID /ENABLE pin 
  digitalWrite(chipSelectPin, LOW);         // Activate the RFID reader
  pinMode(NRSTPD,OUTPUT);                     // Set digital pin 10 , Not Reset and Power-down
  digitalWrite(NRSTPD, HIGH);

  myRFID.AddicoreRFID_Init();  
}

void workRFID(void){
  uchar i, tmp, checksum1;
  uchar status;
  uchar str[MAX_LEN];
  uchar RC_size;
  uchar blockAddr;  //Selection operation block address 0 to 63
  String mynum = "";

  str[1] = 0x4400;
    
  //Find tags, return tag type
  status = myRFID.AddicoreRFID_Request(PICC_REQIDL, str); 

  //Anti-collision, return tag serial number 4 bytes
  status = myRFID.AddicoreRFID_Anticoll(str);
  if (status == MI_OK)
  {
    if(str[0] == 64)                      //You can change this to the first byte of your tag by finding the card's ID through the Serial Monitor
    {
      withSecurity();
    }
  }
  else if(status == MI_NO_TAG_ERR){
    withoutSecurity();
  }
  myRFID.AddicoreRFID_Halt();      //Command tag into hibernation          
}

//::::::::::^^^^^^::::::::::RFID::::::::::::::^^^^^^::::::::::::::


