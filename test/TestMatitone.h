#ifndef MATITONE_H  
#define MATITONE_H  

#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_BMI270_BMM150.h>

void TestBtSetup();
void TestBtLoop();
void TestBtSend(const char* message);

#endif