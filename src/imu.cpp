#include "matitone.h"

void SetupAccel() {
    if (!IMU.begin()) {  // Initialise le capteur IMU (accéléro + gyro)
        Serial.println("Échec de l'init de l'IMU !");
        while (1);
      }
      Serial.println("Accéléromètre prêt !");
}

void ReadAccel() {
    float x, y, z;
    if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);  // Lit les valeurs (en g)
    
    // Affiche dans le terminal série
    Serial.print("Accélération (g) => X: ");
    Serial.print(x);
    Serial.print(" | Y: ");
    Serial.print(y);
    Serial.print(" | Z: ");
    Serial.println(z);
  }
}