#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoMqttClient.h>
const int HX711_dout = 16; //mcu > HX711 dout pin
const int HX711_sck = 17;//mcu > HX711 sck pin
const int relay_pin = 25;
LiquidCrystal_I2C lcd(0x27,20,4);
HX711_ADC LoadCell(HX711_dout, HX711_sck);
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
const int calVal_eepromAdress = 0;
int sendingInterval = 0;
unsigned long t = 0;
static boolean newDataReady = 0;
float i = 0;
float amountOfFood = 0;


void setup() {
  Serial.begin(115200); delay(10);
  lcd.init();
  WiFiConnection();
  Serial.println();
  mqttConnection();
  subscribe();
  Serial.println("Starting...");
  LoadCell.setSamplesInUse(1);
  startConfiguration();
  calibrate(); //start calibration procedure
  pinMode(relay_pin, OUTPUT);
  digitalWrite(relay_pin, HIGH);
  lcd.clear();
}

void loop() {
  i = LoadCell.getData();
  displayOnLcd();
  mqttClient.poll();
  if(amountOfFood > 0){
  if(i < amountOfFood){
    digitalWrite(relay_pin, LOW); 
    sendingInterval = 0;
  }
  else if(i>= amountOfFood){
     digitalWrite(relay_pin, HIGH);
     if(sendingInterval  == 70){
     sendMessage(i);
     amountOfFood = 0;
     } 
    else{
    Serial.println(sendingInterval);
    sendingInterval++;
    delay(10);
    } 
    }
    }
     
  // receive command from serial terminal
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay(); //tare
    else if (inByte == 'r') calibrate(); //calibrate
  }

  // check if last tare operation is complete
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }
}

void calibrate() {
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("empty load cell");
    lcd.setCursor(0,1);
    lcd.print("send t");

  boolean _resume = false;
  while (_resume == false) {
    LoadCell.update();
    if (Serial.available() > 0) {
      if (Serial.available() > 0) {
        char inByte = Serial.read();
        if (inByte == 't') LoadCell.tareNoDelay();
      }
    }
    if (LoadCell.getTareStatus() == true) {
      Serial.println("Tare complete");
      _resume = true;
    }
  }
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("know mass object");
  lcd.setCursor(0,1);
  lcd.print("send weight");

  float known_mass = 0;
  _resume = false;
  while (_resume == false) {
    LoadCell.update();
    if (Serial.available() > 0) {
      known_mass = Serial.parseFloat();
      if (known_mass != 0) {
        Serial.print("Known mass is: ");
        Serial.println(known_mass);
        _resume = true;
      }
    }
  }
  LoadCell.refreshDataSet(); //refresh the dataset to be sure that the known mass is measured correct
  float newCalibrationValue = LoadCell.getNewCalibration(known_mass); //get the new calibration value
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("save to EEPROM");
  lcd.setCursor(0,1);
  lcd.print("send y/n");

  _resume = false;
  while (_resume == false) {
    if (Serial.available() > 0) {
      char inByte = Serial.read();
      if (inByte == 'y') {
#if defined(ESP8266)|| defined(ESP32)
        EEPROM.begin(512);
#endif
        EEPROM.put(calVal_eepromAdress, newCalibrationValue);
#if defined(ESP8266)|| defined(ESP32)
        EEPROM.commit();
#endif
        EEPROM.get(calVal_eepromAdress, newCalibrationValue);
        _resume = true;
      }
      else if (inByte == 'n') {
        _resume = true;
      }
    }
  }
}
void startConfiguration()
{
  LoadCell.begin();
  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag() || LoadCell.getSignalTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(1.0); // user set calibration value (float), initial value 1.0 may be used for this sketch
    Serial.println("Startup is complete");
  }
  while (!LoadCell.update());
}

void displayOnLcd()
{
  const int serialPrintInterval = 0;
  if (LoadCell.update()) newDataReady = true;  // check for new data/start next conversion:
 if (newDataReady) { // get smoothed value from the dataset:
    if (millis() > t + serialPrintInterval) {
      lcd.backlight();
      lcd.setCursor(3,0);
      lcd.print(i);
      lcd.setCursor(3,1);
      lcd.print("Grams");

Serial.println(i);
}
  newDataReady = 0;
      t = millis()+40;
}
}
