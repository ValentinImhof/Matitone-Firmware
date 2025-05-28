/*Main pour le premier test le 20/05/2025
par interruption, les boutons envoient un message s'ils sont pressés
envoi en continu de deux messages différents selon niveau de pression des capts
led IR respective allumée dès le niveau de pression le plus faible*/

#include "matitone.h"

volatile bool AV2UP; // Variable pour le capteur avant
volatile bool AR2UP;

void setup() {
  Serial.begin(9600);
  BtSetup();
  SetupCapt();
  SetupButtons();
  pinMode(2, OUTPUT); // LED IR AV
  pinMode(3, OUTPUT); // LED IR AR
}

void loop() {
  BtLoop(); // Gère les communications Bluetooth

  if (ReadCapt("AV") < 0.85){
    Serial.println("Capteur avant 2");
    BtSend("AV2");
  } else if(ReadCapt("AV") < 0.95) {
    Serial.println("Capteur avant 1");
    digitalWrite(2, HIGH);
    BtSend("AV1");
  }
  else {
    digitalWrite(2, LOW);
  }

if (ReadCapt("AR") < 1){
    Serial.println("Capteur arrière 2");
    BtSend("AR2");
  } else if(ReadCapt("AR") < 1.3) {
    Serial.println("Capteur arrière 1");
    digitalWrite(3, HIGH);
    BtSend("AR1");
  }
  else {
    digitalWrite(3, LOW);
  }

  Serial.println("AR: " + String(ReadCapt("AR")) + "    AV: " + String(ReadCapt("AV")));

  // Gestion des boutons directement dans le main
  if(button1Pressed) {
    button1Pressed = false;
    Serial.println("Bouton 1 pressé"); 
    BtSend("S1");
  }

  if(button2Pressed) {
    button2Pressed = false;
    Serial.println("Bouton 2 pressé");
    BtSend("S2");
  }

  if(button3Pressed) {
    button3Pressed = false;
    Serial.println("Bouton 3 pressé");
    BtSend("S3");
  }

  delay(100);
}