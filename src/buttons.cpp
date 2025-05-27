#include "matitone.h"

// Définition des variables boutons
volatile bool button1Pressed = false;
volatile bool button2Pressed = false;
volatile bool button3Pressed = false;

// Fonctions d'interruption
static void handleButton1() {
  button1Pressed = true;
}

static void handleButton2() {
  button2Pressed = true;
}

static void handleButton3() {
  button3Pressed = true;
}

void SetupButtons() {
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  attachInterrupt(digitalPinToInterrupt(4), handleButton1, RISING);
  attachInterrupt(digitalPinToInterrupt(5), handleButton2, RISING);
  attachInterrupt(digitalPinToInterrupt(6), handleButton3, RISING);
}