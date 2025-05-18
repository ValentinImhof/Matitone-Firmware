#ifndef MATITONE_H  
#define MATITONE_H  

#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_BMI270_BMM150.h>

// Déclaration des fonctions Bluetooth
void BtSetup();
void BtLoop();
void BtSend(const char* message);

// Déclaration des fonctions capteurs
void SetupCapt();
float ReadCapt(const char* sensor);

// Déclaration des fonctions boutons
void SetupButtons();
extern volatile bool button1Pressed;
extern volatile bool button2Pressed;

#endif