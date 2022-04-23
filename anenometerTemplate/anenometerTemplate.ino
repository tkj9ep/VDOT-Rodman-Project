#include <Wire.h>
#include "RTClib.h"
#include <SD.h>
#include <SPI.h>
#define W5200_CS  10
#define SDCARD_CS 4
RTC_DS1307 RTC;

const int sensorPin = A0; //Defines the pin that the anemometer output is connected to
int CS = 4;
int SDPin = A1; //temp
int sensorValue = 0; //Variable stores the value direct from the analog pin
float sensorVoltage = 0; //Variable that stores the voltage (in Volts) from the anemometer being sent to the analog pin
float windSpeed = 0; // Wind speed in meters per second (m/s)
 
float voltageConversionConstant = .004882814; //This constant maps the value provided from the analog read function, which ranges from 0 to 1023, to actual voltage, which ranges from 0V to 5V
int sensorDelay = 1000; //Delay between sensor readings, measured in milliseconds (ms)
 
 
float voltageMin = .43; // Mininum output voltage from anemometer in mV.
float windSpeedMin = 0; // Wind speed in meters/sec corresponding to minimum voltage
int timeX = 5000;
float voltageMax = 2.0; // Maximum output voltage from anemometer in mV.
float windSpeedMax = 32; // Wind speed in meters/sec corresponding to maximum voltage
float sum = 0.0;
int count = 0;
int countThreshold = timeX/sensorDelay;
int currentTime;

File file;

void setup() 
{              
 currentTime = 0;
 Serial.begin(9600);
    Wire.begin();
    RTC.begin();
  // Check to see if the RTC is keeping time.  If it is, load the time from your computer.
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // This will reflect the time that your sketch was compiled
    //RTC.adjust(DateTime(__DATE__, __TIME__));
  }
 Serial.print("Initializing SD card...");
    pinMode(W5200_CS, OUTPUT);
    if (!SD.begin(SDCARD_CS)) {
        Serial.println("initialization failed!");
        return;
    }
    Serial.println("initialization done.");
    digitalWrite(SDCARD_CS, HIGH);
    writeToCSV();

 }
 
void loop() {

if(count >= countThreshold) {
  float avg = sum/count;
  Serial.print("Average Speed is: \t");
  Serial.println(avg);
  averageToSD(avg);
  count = 0;
  sum = 0;
}
sensorValue = analogRead(sensorPin); //Get a value between 0 and 1023 from the analog pin connected to the anemometer

sensorVoltage = sensorValue * voltageConversionConstant; //Convert sensor value to actual voltage
 
//Convert voltage value to wind speed using range of max and min voltages and wind speed for the anemometer
if (sensorVoltage <= voltageMin){
 windSpeed = 0; //Check if voltage is below minimum value. If so, set wind speed to zero.
}else {
  windSpeed = (sensorVoltage - voltageMin)*windSpeedMax/(voltageMax - voltageMin); //For voltages above minimum value, use the linear relationship to calculate wind speed.
}
 
 //Print voltage and windspeed to serial
  printTime();
  Serial.print("\tVoltage: ");
  Serial.print(sensorVoltage);
  Serial.print("\t"); 
  Serial.print("Wind speed: ");
  float mphSpeed = mphConversion(windSpeed);
  Serial.println(mphSpeed);
  sum += mphSpeed;
  
  count++;
  delay(sensorDelay);
  currentTime += 1;
  
}

void printTime() {
    DateTime now = RTC.now();
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print('/');
    Serial.print(now.year(), DEC);
    Serial.print(' ');
    if(now.hour()<10) Serial.print("0");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    if(now.minute()<10) Serial.print("0");
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    if(now.second()<10) Serial.print("0");
    Serial.print(now.second(), DEC);
}
void writeToCSV() {
    file.println("Date,Time,Speed (MPH),");
}

void printDateSD() {
    DateTime now = RTC.now(); 
    file.print(now.month(), DEC);
    file.print('/');
    file.print(now.day(), DEC);
    file.print('/');
    file.print(now.year(), DEC);
    file.print(',');
}

void printTimeSD() {
    DateTime now = RTC.now(); 
    
    if(now.hour()<10) file.print("0");
    file.print(now.hour(), DEC);
    file.print(':');
    if(now.minute()<10) file.print("0");
    file.print(now.minute(), DEC);
    file.print(':');
    if(now.second()<10) file.print("0");
    file.print(now.second(), DEC);  
    file.print(',');
}

void averageToSD(float mph) {
    file = SD.open("windData.txt");
    if (file) {
     String string1 = file.readStringUntil('\n');
        string1.trim();
        if(!string1.equals("Date,Time,Speed (MPH),")){
          SD.remove("windData.txt");
          file.close();
          file = SD.open("windData.txt", FILE_WRITE);
          writeToCSV();
        }
        file.close();
    }
  
    file = SD.open("windData.txt", FILE_WRITE);
    if (file) {
        printDateSD();
        printTimeSD();
        file.print(mph);
        file.println(',');
        file.close();
    } else 
        Serial.println("error opening test.txt");
}

void writeToSD(float mph) {
    file = SD.open("windData.txt", FILE_WRITE);
    if (file) {
        file.print("MPH: ");
        file.println(mph);
        file.close();
    } else 
        Serial.println("error opening test.txt");
}

float mphConversion(float windSpeed) {
  return windSpeed * 2.23694;
}
