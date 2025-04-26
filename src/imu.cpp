#include "matitone.h"

void SetupAccel() {
    if (!IMU.begin()) {  // Initialise le capteur IMU (accéléro + gyro)
        Serial.println("Échec de l'init de l'IMU !");
        while (1);
      }
      Serial.println("Accéléromètre prêt !");
}

bool ReadAccel(float& x, float& y, float& z) {
  if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(x, y, z);  // Lit les valeurs (en g)
      return true; // Lecture réussie
  }
  return false; // Lecture échouée
}