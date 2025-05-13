#include "matitone.h"
#include "ArduinoLowPower.h"

unsigned long previousMillis = 0;
const unsigned long interval = 100;  // lecture toutes les 100 ms

void setup() {
  Serial.begin(9600);
  SetupAccel();  // initialisation de l'IMU
  pinMode(LED_BUILTIN, OUTPUT);

  // Désactive le port série quand inactif pour réduire la conso
  LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, []{}, CHANGE);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    float accelX, accelY, accelZ;
    if (ReadAccel(accelX, accelY, accelZ)) {
      if ((accelX > 1.2) || (accelY > 1.2) || (accelZ > 1.2)) {
        Serial.println("Mouvement détecté");
        digitalWrite(LED_BUILTIN, HIGH);
      } else {
        digitalWrite(LED_BUILTIN, LOW);
      }
    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  // Mise en veille "léger" entre chaque boucle
  LowPower.idle();
}
