#include "matitone.h"
// Test Noé Commit
void setup()
{
  Serial.begin(9600);  // Initialise le port série pour le débogage
  BtSetup();  // Appelle la fonction de configuration Bluetooth
  SetupCapt();  // Appelle la fonction de configuration du capteur
  SetupAccel();  // Appelle la fonction de configuration de l'accéléromètre
  pinMode(4, INPUT); // Configure la broche 4 comme entrée pour le bouton 1
  pinMode(5, INPUT); // Configure la broche 5 comme entrée pour le bouton 2
  pinMode(6, INPUT); // Configure la broche 6 comme entrée pour le bouton 3
}

void loop()
{
  BtLoop();  // Appelle la fonction de boucle Bluetooth

  float accelX, accelY, accelZ;
  if (ReadAccel(accelX, accelY, accelZ)) {
        if((accelX > 1.2) or (accelY > 1.2) or (accelZ > 1.2)) {  // Si l'accélération dépasse 1.2g
            Serial.println("Mouvement détecté");
            BtSend("Mouvement");
        }
    }

  if (ReadCapt("AV") > 1) {  // Si la valeur du capteur avant est supérieure à 1
    Serial.println("Capteur avant activé");
    BtSend("PressionAV"); 
  }
  if (ReadCapt("AR") > 1) {  // Si la valeur du capteur arrière est supérieure à 1
    Serial.println("Capteur arrière activé");
    BtSend("PressionAR");
  }

  if (digitalRead(4) == HIGH) {  // Si le bouton sur la broche 4 est pressé
    Serial.println("Bouton 1 pressé");
    BtSend("Bouton1");
  }
  if (digitalRead(5) == HIGH) {  // Si le bouton sur la broche 5 est pressé
    Serial.println("Bouton 2 pressé");
    BtSend("Bouton2");
  }
  if (digitalRead(6) == HIGH) {  // Si le bouton sur la broche 6 est pressé
    Serial.println("Bouton 3 pressé");
    BtSend("Bouton3");
  }
  
  delay(100);  // Délai d'une seconde entre les lectures
}