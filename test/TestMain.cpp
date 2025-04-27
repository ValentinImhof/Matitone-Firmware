//Juste faire une code qui envoie 'S1' toutes les secondes s'il est connecté

#include "TestMatitone.h"

void setup() {
    Serial.begin(9600);  // Initialise le port série pour le débogage
    TestBtSetup();  // Appelle la fonction de configuration Bluetooth
}

void loop() {
    TestBtLoop();
    TestBtSend("S1");
    delay(100);
}