#include "matitone.h"

const int bouton1 = 6;   // Bouton 1
const int bouton2 = 5;   // Bouton 2

const int LedIRAV = 2;      // LED 1
const int LedIRAR = 3;      // LED 2

void setup()
{
  Serial.begin(9600);  // Initialise le port série pour le débogage
  //while (!Serial);  // Attendre que le port série soit prêt
  //BtSetup();  // Appelle la fonction de configuration du module Bluetooth
  //SetupCapt();  // Appelle la fonction de configuration du capteur

  pinMode(bouton1, INPUT);  // Bouton 1 sur D6
  pinMode(bouton2, INPUT);  // Bouton 2 sur D5

  pinMode(LedIRAV, OUTPUT);
  pinMode(LedIRAR, OUTPUT);
}

void loop()
{
  //LoopCapt();  // Appelle la fonction de lecture du capteur
  //BtLoop();  // Appelle la fonction de boucle du module Bluetooth
  //BtSend("Babinks");

  if (digitalRead(bouton1) == HIGH) {
    Serial.println("Bouton 1 (D6) appuyé !");
    digitalWrite(LedIRAV, HIGH);
  } else {
    digitalWrite(LedIRAV, LOW);
  }

  if (digitalRead(bouton2) == HIGH) {
    Serial.println("Bouton 2 (D5) appuyé !");
    digitalWrite(LedIRAR, HIGH);
  } else {
    digitalWrite(LedIRAR, LOW);
  }
  

  delay(100);  // Attendre 1 seconde entre les envois
}