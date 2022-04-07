#include <SD.h>

const int sensorPin = A0; //Defines the pin that the anemometer output is connected to
int CS = 4;
int SDPin = A1; //temp
int sensorValue = 0; //Variable stores the value direct from the analog pin
float sensorVoltage = 0; //Variable that stores the voltage (in Volts) from the anemometer being sent to the analog pin
float windSpeed = 0; // Wind speed in meters per second (m/s)
 
float voltageConversionConstant = .004882814; //This constant maps the value provided from the analog read function, which ranges from 0 to 1023, to actual voltage, which ranges from 0V to 5V
int sensorDelay = 1000; //Delay between sensor readings, measured in milliseconds (ms)
 
//Anemometer Technical Variables
//The following variables correspond to the anemometer sold by Adafruit, but could be modified to fit other anemometers.
 
float voltageMin = .43; // Mininum output voltage from anemometer in mV.
float windSpeedMin = 0; // Wind speed in meters/sec corresponding to minimum voltage
int timeX = 30000;
float voltageMax = 2.0; // Maximum output voltage from anemometer in mV.
float windSpeedMax = 32; // Wind speed in meters/sec corresponding to maximum voltage
float sum = 0.0;
int count = 0;
int countThreshold = timeX/sensorDelay;
File file;


void setup() 
{              
  Serial.begin(9600);  //Start the serial connection

  //SD initialization
  pinMode(CS, OUTPUT);

 if (!SD.begin(CS)) {
    Serial.println("SD card not initialized");  
 }

 if(SD.exists("data.txt")) {
  Serial.println("File exists.");

  if(SD.remove("data.txt") == true) {
    Serial.println("Removed File");
  } else {
    Serial.println("Unsuccessfully removed file");
  }
  }
 }
 
void loop() {

if(count >= countThreshold) {
  //writeFile(sum/count);
  Serial.print("Average Speed is: \t");
  Serial.println(sum/count);
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
  Serial.print("Voltage: ");
  Serial.print(sensorVoltage);
  Serial.print("\t"); 
  Serial.print("Wind speed: ");
  float mphSpeed = mphConversion(windSpeed);
  Serial.println(mphSpeed);
  sum += mphSpeed;
  
  count++;
  delay(sensorDelay);
  writeFile(mphSpeed);
  
}

void writeFile(float mphSpeed) {
 //write file
 file = SD.open("temp.txt", FILE_WRITE);
 if(file) {
  file.println(mphSpeed);
  file.close();
  
 } else {
  Serial.println("Failed");
 }
}

void readFile() {
  
}

float mphConversion(float windSpeed) {
  return windSpeed * 2.23694;
}
