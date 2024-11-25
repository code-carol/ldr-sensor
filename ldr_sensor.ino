
// Include Libraries
#include <SPI.h>                        // SD card communication
#include <SD.h>                         // Interacting with SD card
#include <Wire.h>                       // I2C communication
#include <Adafruit_GFX.h>               // Basic graphics functions for displays
#include <Adafruit_SSD1306.h>           // Communication with SSD1306 OLED display
#include <RTCZero.h>                    // Functionality for display time


// Define Constants
#define SCREEN_WIDTH 128                 // OLED display width, in pixels
#define SCREEN_HEIGHT 64                 // OLED display height, in pixels
#define OLED_RESET -1                    // Reset pin not used for I2C
#define SCREEN_ADDRESS 0x3C              // Address 0x3C for 128x32 OLED


// Create Display Object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 

// Pin configurations
const int chipSelect = 28;              // SD Card MKR ZERO SS pin 
const int ldrPin = A0;                  // Arduino pin connected to LDR
const int statusLedPin = 2;             // Arduino pin connected to Status LED 
const int brightnessLedPin = 3;         // Arduino pin connected to Brightness LED

// Brightness variable
String lastBrightnessLevel;

// Create an rtc object 
RTCZero rtc;

// Change these values to set the current initial time 
const byte seconds = 0;
const byte minutes = 0;
const byte hours = 9;

// Change these values to set the current initial date 
const byte day = 18;
const byte month = 11;
const byte year = 24;


void setup() {


  // Pin Mode Configuration
  pinMode(ldrPin, INPUT);                 // Read LDR input
  pinMode(statusLedPin, OUTPUT);          // Control Status LED 
  pinMode(brightnessLedPin, OUTPUT);      // Control Brightness LED

  // Initialise serial communication
  Serial.begin(9600);

  // Initialise SD Card
  Serial.print("Initialising SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
  while (1);
  }
  Serial.println("Card initialised.");

  // Initialise OLED display
  Serial.print("Initialising display...");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { 
    Serial.println(F("SSD1306 allocation failed"));
  while (1);
  }
  Serial.println("SSD1306 allocation successfully.");

  display.clearDisplay();                // Clear any previous content on the screen 
  display.setRotation(1);                // Rotate display 90 degrees left
  display.display();                     // Make the changes happen

  // Initialise RTC
  rtc.begin(); 

  // Set the time
  rtc.setHours(hours);
  rtc.setMinutes(minutes);
  rtc.setSeconds(seconds);

  // Set the date
  rtc.setDay(day);
  rtc.setMonth(month);
  rtc.setYear(year);

}


void loop() {

  // Reading LDR Value and Adjusting LED Brightness 
  int ldrValue = analogRead(ldrPin); // Reads the current light level

  // int brightness = map(ldrValue, 100, 1023, 0, 255);   // Scales LDR value to a brightness range of 0-255
  // brightness = constrain(brightness, 0, 255);          // Ensures value stays within 0-255 range
  // analogWrite(brightnessLedPin, brightness);           // Sets the brightnessLedPin to the calculated brightness


  // Determine and Display Brightness Level
  String brightnessLevel;

  if (ldrValue < 300) { 
    brightnessLevel = "Dark";
  } else if (ldrValue < 600) {
    brightnessLevel = "Dim";
  } else if (ldrValue < 900) {
    brightnessLevel = "Bright";
  } else {
    brightnessLevel = "Radiant";
  }


  // Print brightness level to serial monitor
  Serial.print("LDR Value: ");
  Serial.print(ldrValue);
  Serial.print(" | Brightness Level: ");
  Serial.print(brightnessLevel);


  // Display brightness level and date/time on OLED screen
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print("Brightness ");
  display.setCursor(0, 10);
  display.print("Level:");
  display.setCursor(0, 30);
  display.print(brightnessLevel);

  display.setCursor(0, 60);
  display.print("Date:");
  display.setCursor(0, 70);
  print2digitsDisplay(rtc.getDay());          //retrieve day 
  display.print("/");
  print2digitsDisplay(rtc.getMonth());        //retrieve month
  display.print("/");
  print2digitsDisplay(rtc.getYear());         //retrieve year

  display.setCursor(0, 80);
  display.print("Time:");
  display.setCursor(0, 90);
  print2digitsDisplay(rtc.getHours());         //retrieve day 
  display.print(":");
  print2digitsDisplay(rtc.getMinutes());       //retrieve month
  display.print(":");
  print2digitsDisplay(rtc.getSeconds());       //retrieve year

  display.display();


  // Log data to SD card only if the brightness level has changed 
  if (brightnessLevel != lastBrightnessLevel) {
    ile dataFile = SD.open("datalog.txt", FILE_WRITE);  


    dataFile.print("Date ");
    print2digitsFile(dataFile, rtc.getDay());     //retrieve day 
    dataFile.print("/");
    print2digitsFile(dataFile, rtc.getMonth());   //retrieve month
    dataFile.print("/");
    print2digitsFile(dataFile, rtc.getYear());    //retrieve year
    dataFile.println(" ");


    dataFile.print("Time ");
    print2digitsFile(dataFile, rtc.getHours());      //retrieve hours
    dataFile.print(":");
    print2digitsFile(dataFile, rtc.getMinutes());   //retrieve minutes
    dataFile.print(":");
    print2digitsFile(dataFile, rtc.getSeconds());   //retrieve seconds
    dataFile.println(" ");

    dataFile.print("LDR Value: ");
    dataFile.print(ldrValue);
    dataFile.print(" | Brightness Level: ");
    dataFile.print(brightnessLevel);
    dataFile.println(" ");

    dataFile.close();


  // Update lastBrightnessLevel with the current level after logging

  lastBrightnessLevel = brightnessLevel;
  Serial.println("Test Brightness Level After");
  Serial.println(lastBrightnessLevel);
  Serial.println("Data added to SD card");
  } else {
  Serial.println("No data added to SD card");
  }


  // Blink status LED to show the system is active
  digitalWrite(statusLedPin, HIGH);
  delay(500);
  digitalWrite(statusLedPin, LOW);
  delay(500);


}


void print2digitsDisplay(int number) {
    if (number < 10) {
      display.print("0"); // print a 0 before if the number is < than 10
    }
    display.print(number);
  }


void print2digitsFile(File &file, int number) {
    if (number < 10) {
      file.print("0"); // print a 0 before if the number is < than 10
    }
    file.print(number);

  }