#ifndef MATITONE_H  
#define MATITONE_H  

#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_BMI270_BMM150.h>

void BtSetup();
void BtLoop();
void BtSend(const char* message);
void SetupAccel();
void ReadAccel();
void SetupCapt();
float ReadCapt(const char * sensor);
#endif