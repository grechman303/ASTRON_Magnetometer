//#include "ArduinoLowPower.h"
#include <MKRWAN.h>
#include <AM2302-Sensor.h>
#include "arduino_secrets.h"

LoRaModem modem;
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;
//String deviceEui = modem.deviceEUI();
String deviceEui;
// Define the pin where the AM2302 sensor is connected
#define SENSOR_PIN 9

// Define the pins for the magnetometer sensors
const int magnetometerPins[] = {6, 7, 8};
unsigned long durations_high[3];
unsigned long durations_low[3];

// Create an instance of the sensor object
AM2302::AM2302_Sensor am2302{SENSOR_PIN};


void setup() {
  Serial.begin(9600);

  // Initialize the AM2302 sensor
  am2302.begin();
  
  // Initialize the magnetometer sensor pins
  for (int i = 0; i < 3; ++i) {
    pinMode(magnetometerPins[i], INPUT);
  } 

  while (!Serial);
  // change this to your regional band (eg. US915, AS923, ...)
  if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    while (1) {}
  };
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  deviceEui = modem.deviceEUI();
  Serial.println(deviceEui);

  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    while (1) {}
  }else{
    Serial.println("Connection established successfully");
  }

  // Set poll interval to 60 secs.
  modem.minPollInterval(60);
  // NOTE: independent of this setting, the modem will
  // not allow sending more than one message every 2 minutes,
  // this is enforced by firmware and can not be changed.
}

void loop() {
  // Read and print AM2302 sensor values every 2 seconds
  //delay(2000);

  // Read the AM2302 sensor status
  auto status = am2302.read();
  String read_status = AM2302::AM2302_Sensor::get_sensorState(status);
  float temperature = am2302.get_Temperature();
  float humidity = am2302.get_Humidity();

  // Read and print values from the three magnetometer sensors
  for (int i = 0; i < 3; ++i) {
    durations_high[i] = pulseIn(magnetometerPins[i], HIGH);
    durations_low[i] = pulseIn(magnetometerPins[i], LOW);
  }
  

  char csvString[128];
  snprintf(csvString, sizeof(csvString), "%s,%d,%s,%.2f,%.2f,%d,%lu,%lu,%d,%lu,%lu,%d,%lu,%lu", 
  deviceEui.c_str(), SENSOR_PIN, read_status.c_str(), temperature, humidity, 
  magnetometerPins[0], durations_high[0], durations_low[0], 
  magnetometerPins[1], durations_high[1], durations_low[1], 
  magnetometerPins[2], durations_high[2], durations_low[2]);

  send_lora_message(csvString);
  Serial.println(csvString);
  delay(60000); // Wait for 0.5 seconds before the next reading 
  // LowPower.deepSleep(10000);
}

/*
// Simple linear conversion based on graphs in manual
float period_to_micro_tesla(float period){
  float c = 15000;
  float new_period = 1e6 / ((1e6/period)+c);
  return ((new_period - 11.75)/7.5)*100; //Probably very innacurate
}

// Simple linear conversion based on graphs in manual
float period_to_micro_tesla_uncorrected(float period){
  return ((period - 14)/10)*100; //Probably very innacurate
}
*/

// Code to create message for database, encodes in JSON (more clear than just sending a CSV string)
/*
String createMessage(String deviceEUI, unsigned long duration_high, unsigned long duration_low, int sensor_id) {
  unsigned long timestamp = millis();     // Example timestamp

  // Create a JSON string
  String message = "{";
  message += "\"deviceEUI\":\"" + deviceEUI + "\",";
  message += "\"duration_high\":" + String(duration_high) + ",";
  message += "\"duration_low\":" + String(duration_low) + ",";
  message += "\"sensor_id\":" + String(sensor_id) + ",";
  message += "\"timestamp\":" + String(timestamp);
  message += "}";

  return message;
}
*/

void send_lora_message(String msg){
  int err;
  modem.beginPacket();
  modem.print(msg);
  err = modem.endPacket(true);
  if (err > 0) {
    Serial.println("Message sent correctly!");
  } else {
    Serial.println("Error sending message :(");
    Serial.println("(you may send a limited amount of messages per minute, depending on the signal strength");
    Serial.println("it may vary from 1 message every couple of seconds to 1 message every minute)");
  }
  delay(1000);
  if (!modem.available()) {
    Serial.println("No downlink message received at this time.");
    return;
  }
  char rcv[128];
  int i = 0;
  while (modem.available()) {
    rcv[i++] = (char)modem.read();
  }
  Serial.print("Received: ");
  for (unsigned int j = 0; j < i; j++) {
    Serial.print(rcv[j] >> 4, HEX);
    Serial.print(rcv[j] & 0xF, HEX);
    Serial.print(" ");
  }
  Serial.println();
}
// TODO: add a function which triggers if it detects a high measurement