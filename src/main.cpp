/*Main pour le premier test le 20/05/2025
par interruption, les boutons envoient un message s'ils sont pressés
envoi en continu de deux messages différents selon niveau de pression des capts
led IR respective allumée dès le niveau de pression le plus faible*/

#include "matitone.h"

volatile bool AV2UP;
volatile bool AR2UP;

void setup() {
  Serial.begin(9600);
  BtSetup();
  SetupCapt();
  SetupButtons();
  
  pinMode(2, OUTPUT); // LED IR AV
  pinMode(3, OUTPUT); // LED IR AR

  AV2UP = false;
  AR2UP = false;
}

void loop() {
  BtLoop();
  
  // Gestion capteurs de pression
  if (ReadCapt("AV") > 3){
    //Serial.println("Capteur avant 2");
    if (AV2UP == false)  {
      BtSend("AV2UP");
      Serial.println("AV2UP");
      AV2UP = true;
    }
  }
  else{
    if (AV2UP == true) {
      BtSend("AV2DOWN");
      Serial.println("AV2DOWN");
      AV2UP = false;
    }
  }
  if(ReadCapt("AV") > 2) {
    //Serial.println("Capteur avant 1");
    digitalWrite(2, HIGH);
  }
  else {
    digitalWrite(2, LOW);
  }
  //Serial.println(ReadCapt("AV"));

  if (ReadCapt("AR") < 1){
    Serial.println("Capteur arrrière 2");
    BtSend("AR2");
  } else if(ReadCapt("AR") < 1.4) {
    Serial.println("Capteur arrière 1");
    digitalWrite(3, HIGH);
    BtSend("AR1");
  }
  else {
    digitalWrite(3, LOW);
  }
  
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

  delay(300);
}