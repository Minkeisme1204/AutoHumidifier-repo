#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define actPin 10 // pin for Actuator 
#define statePin 6 // pin for ON/OFF button 
#define dhtPin 8 // pin for DHT11
#define modePin 5 // pin for Mode button
#define waterPin A7  // pin for Water sensor
#define ledPin 3// pin for Caution LED 

#define NORMAL 0
#define AUTO 1
#define H_CEIL 60
#define H_FLOOR 50 
#define WLIMIT 700
#define T_ANCHOR 120000
#define ON true
#define OFF false 

bool state = ON; 
bool mode = NORMAL;

struct button {
  int32_t pin; 
  int32_t oldS = 0; 
  int32_t newS = 1; 
  button (int32_t P) {
    pin = P; 
  }
  void stateClick() {
    newS = digitalRead(pin);
    if(oldS == 1 && newS == 0) {
      state = (!state);
      oldS = newS;
    } 
    oldS = newS; 
  }
  void modeClick() {
    newS = digitalRead(pin);
    if(oldS == 1 && newS == 0) {
      mode = (!mode);
      oldS = newS;
    } 
    oldS = newS; 
  } 
};
 
bool caution = false; 
int32_t humid = 0; 
int32_t water; 
int32_t timer; 
DHT dht(dhtPin, DHT11);
struct button btn1(statePin); 
struct button btn2(modePin); 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Start to set up!");
  pinMode(actPin, OUTPUT);
  pinMode(btn1.pin, INPUT_PULLUP);
  pinMode(btn2.pin, INPUT_PULLUP);
  pinMode(waterPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.print("Set up done!");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(state);
  btn1.stateClick();
  if (state == ON) // When device is ON
  { 
    water = analogRead(waterPin);// Read the value of the Water stored in bottle
    btn2.modeClick();
    if (mode == NORMAL) // When the Mode is Normal 
    {
      digitalWrite(actPin, HIGH);// Actuator is ON
      timer = millis();
    } 
    else // When the Mode is Auto 
    { 
      if (millis() - timer == T_ANCHOR) // Set 2 min read Humidity once 
      {
        Serial.print(timer);
        humid = dht.readHumidity();
        timer = millis();
      }
      if (humid < H_FLOOR) // Actuator is On if the humidity is too LOW 
      {
        digitalWrite(actPin, HIGH);
        state = ON;
      } 
      else if (humid > H_CEIL) // Actuator is OFF if the humidity is acceptable
      {
        digitalWrite(actPin, LOW);
        state = OFF;
      }
    }

    if (water < WLIMIT) // Make the caution once The amount of Water is not Enough 
    {
      state = OFF;
      while (analogRead(waterPin) < WLIMIT) 
      {
          digitalWrite(ledPin, HIGH);
          delay(300);
          digitalWrite(ledPin, LOW);
          delay(300);
      }
    }
  }
  else 
  {
    digitalWrite(actPin, LOW);
    digitalWrite(ledPin, LOW);
  }
}



