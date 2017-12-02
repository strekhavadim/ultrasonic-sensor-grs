#include "wiring_private.h"
#include "pins_arduino.h"

int echoPin1 = 9; 
int trigPin1 = 8; 
int echoPin2 = 13; 
int trigPin2 = 12; 
int echoPin3 = 6; 
int trigPin3 = 5; 
int n = 3;
uint8_t pins[3] = {echoPin1, echoPin2, echoPin3};

void setup() { 
  Serial.begin (9600); 
  pinMode(trigPin1, OUTPUT); 
  pinMode(trigPin2, OUTPUT); 
  pinMode(trigPin3, OUTPUT); 
  pinMode(echoPin1, INPUT); 
  pinMode(echoPin2, INPUT); 
  pinMode(echoPin3, INPUT); 
} 

void loop() { 
  int durationX1, durationX2, cm, cmX1, cmX2; 
  
  digitalWrite(trigPin1, LOW); 
  digitalWrite(trigPin2, LOW); 
  digitalWrite(trigPin3, LOW); 
  delayMicroseconds(2); 
  
  digitalWrite(trigPin1, HIGH); 
  digitalWrite(trigPin2, HIGH); 
  digitalWrite(trigPin3, HIGH); 
  delayMicroseconds(10); 
  
  digitalWrite(trigPin1, LOW); 
  digitalWrite(trigPin2, LOW); 
  digitalWrite(trigPin3, LOW); 

  //durationX1 = pulseInLong(echoPin1, HIGH, 100000);
  multiPulseIn(pins, n, HIGH, 10000);
  Serial.println();
  delay(100);
}

void multiPulseIn(uint8_t pins[], int n, uint8_t state, unsigned long timeout){
    bool flags[n];      // n flags for each pin
    uint8_t ports[n];   // n ports for each pin
    uint8_t bits[n];    // n bits for each pin
    uint8_t states[n];    // n states for each pin
    unsigned long width[n];
    unsigned long maxloops = microsecondsToClockCycles(timeout)/16;
    
    // Initial data setup
    // Cache the port and bit of the pin in order to speed up the pulse width measuring loop and achieve finer resolution.  
    // Calling digitalRead() instead yields much coarser resolution.
    for(int i = 0; i < n; i++){
      /*Serial.print("st: ");
      Serial.println(state);
      Serial.print("pi");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(pins[i]);*/
      flags[i] = 0;     // Setting all flags to 0 
      width[i] = 0;     // Setting all flags to 0 
      /*Serial.print("f");
      Serial.print(i);
      Serial.println(": 0");*/
      bits[i] = digitalPinToBitMask(pins[i]); // Setting bitmask for each pin
      /*Serial.print("b");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(bits[i]);*/
      ports[i] = digitalPinToPort(pins[i]);   // Setting port for each pin
      /*Serial.print("po");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(ports[i]);*/
      states[i] = (state ? bits[i] : 0);    // Setting state for each pin
      /*Serial.print("s");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(states[i]);*/
    }
  
    // Wait for any previous pulse to end
    while(true){
      bool ready = true;
      for(int i = 0; i < n; i++){
        if(!flags[i] && ((*portInputRegister(ports[i]) & bits[i]) != states[i])){
          flags[i] = true;
        }
        if(flags[i]) continue;
        ready = false;
      }
      if (--maxloops <= 0){
        for(int i = 0; i < n; i++){
          width[i] = 1;
        }
        Serial.println("TIMEOUT1");
        arrayToSerial(width, n);
        return;
      }
      if(ready) break;
    }
    
    // Reset all flags to 0 
    for(int i = 0; i < n; i++){
      flags[i] = 0;     
    }
  
    // Wait for all pulses to start
    while(true){
      bool ready = true;
      for(int i = 0; i < n; i++){
        if(!flags[i] && (*portInputRegister(ports[i]) & bits[i]) == states[i]){
          flags[i] = true;
        }
        if(flags[i]) continue;
        ready = false;
      }
      if (--maxloops <= 0){
        for(int i = 0; i < n; i++){
          width[i] = 2;
        }
        Serial.println("TIMEOUT2");
        arrayToSerial(width, n);
        return;
      }
      if(ready) break;
    }
    
    // Reset all flags to 0 
    for(int i = 0; i < n; i++){
      flags[i] = false;
    }
  
    // Wait for all pulses to stop
    while(true){
      bool ready = true;
      int maxWidth = 0;
      for(int i = 0; i < n; i++){
        if(!flags[i]){
          if((*portInputRegister(ports[i]) & bits[i]) != states[i]){
            flags[i] = true;
          }else{
            ready = false;
            ++width[i];
          } 
          if(width[i] > maxWidth) maxWidth = width[i];
        }
      }
      if (maxWidth >= maxloops){
        for(int i = 0; i < n; i++){
          if(!flags[i]){
            width[i] = 0;
          }
        }
        arrayToSerial(width, n);
        return;
      }
      if(ready) break;
    }
  arrayToSerial(width, n);
  return;
}

void arrayToSerial(unsigned long width[], int n){
  for(int i = 0; i < n; i++){
    Serial.print(width[i]);
    Serial.print(" ");
  }
}

