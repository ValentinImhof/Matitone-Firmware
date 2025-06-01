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
extern volatile bool button3Pressed;
extern volatile bool AV2UP;
extern volatile bool AR2UP;

// Déclaration des fonctions accéléromètre
void SetupAccel();
bool ReadAccel(float& x, float& y, float& z);

// Déclaration des fonctions pour la lecture du niveau de batterie
void SetupBatteryLevel(); // À appeler dans le setup() principal
float ReadBatteryLevel();  // Retourne la tension de la batterie en volts (valeur entière)

// Configuration pour la Logique de Veille
const unsigned long INACTIVITY_DURATION_MS = 30000;    // 30 secondes (en millisecondes)
const unsigned long PERIODIC_SLEEP_INTERVAL_MS = 5000; // 5 secondes (en millisecondes)
const float ACCEL_WAKE_UP_THRESHOLD = 1.1f;            // Seuil d'accélération (en G) pour le réveil
const float MOVEMENT_DETECTION_THRESHOLD_DELTA = 0.10f; // Sensibilité pour l'absence de mouvement

// Variables Globales pour la Logique de Veille (déclarées extern)
extern int SleepState;
extern unsigned long lastSignificantMovementTime;
extern float lastProcessedAccelX, lastProcessedAccelY, lastProcessedAccelZ;
extern bool isFirstAccelReadCycle;

#endif