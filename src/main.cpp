#include "matitone.h"

void setup()
{
  Serial.begin(9600);  // Initialise le port série pour le débogage
  SetupCapt();  // Appelle la fonction de configuration du capteur
}

void loop()
{
  /* Serial.print(ReadCapt("AV"));  // Appelle la fonction de lecture du capteur avant
  Serial.print("  ");  // Ajoute un espace pour la lisibilité
  Serial.println(ReadCapt("AR"));  // Appelle la fonction de lecture du capteur arrière */

  if (ReadCapt("AV") > 1) {  // Si la valeur du capteur avant est supérieure à 0.5
    Serial.println("Capteur avant activé");  // Affiche un message sur le port série
  }
  if (ReadCapt("AR") > 1) {  // Si la valeur du capteur arrière est supérieure à 0.5
    Serial.println("Capteur arrière activé");  // Affiche un message sur le port série
  }
  
  delay(100);  // Délai d'une seconde entre les lectures
}