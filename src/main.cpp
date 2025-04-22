#include "matitone.h"

void setup()
{
  Serial.begin(9600);  // Initialise le port série pour le débogage
  //while (!Serial);  // Attendre que le port série soit prêt
  BtSetup();  // Appelle la fonction de configuration du module Bluetooth
  //SetupCapt();  // Appelle la fonction de configuration du capteur
}

void loop()
{
  //LoopCapt();  // Appelle la fonction de lecture du capteur
  BtLoop();  // Appelle la fonction de boucle du module Bluetooth
  BtSend("Salut");
}