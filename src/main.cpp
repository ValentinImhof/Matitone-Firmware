/* Main pour le premier test le 20/05/2025
 * Simplifié : suppression du Bluetooth, réveil par MOUVEMENT ACCELEROMETRE (delta) uniquement.
 * La condition de réveil est une différence suffisante entre la lecture actuelle et la précédente.
 * Supprime l'utilisation de la bibliothèque ArduinoLowPower.
 */

#include "matitone.h" // Contient les constantes comme MOVEMENT_DETECTION_THRESHOLD_DELTA

// Définitions pour les variables globales déclarées 'extern' dans matitone.h
int SleepState;
unsigned long lastSignificantMovementTime;
float lastProcessedAccelX, lastProcessedAccelY, lastProcessedAccelZ;
bool isFirstAccelReadCycle;

// Les variables Bluetooth (AV2UP, AR2UP) et les flags de boutons
// ne sont plus activement utilisés dans cette version simplifiée pour la logique de veille/réveil,
// mais sont conservés car déclarés extern dans matitone.h.
volatile bool AV2UP;
volatile bool AR2UP;

void setup() {
  Serial.begin(9600);

  SetupCapt(); // Conservation de la lecture des capteurs pour la logique active
  // BtSetup(); // Supprimé - Bluetooth non utilisé dans cette version
  SetupAccel();
  // SetupButtons(); // Supprimé - Les boutons ne sont plus utilisés pour le réveil

  // Initialisation des flags non pertinents pour cette version simplifiée
  AV2UP = false;
  AR2UP = false;
  button1Pressed = false;
  button2Pressed = false;
  button3Pressed = false;

  pinMode(2, OUTPUT); // LED IR AV
  pinMode(3, OUTPUT); // LED IR AR
  pinMode(8, OUTPUT);
  digitalWrite(2, LOW); // Éteint la LED IR AV
  digitalWrite(3, LOW); // Éteint la LED IR AR

  SleepState = 0; // Initialisation de l'état de veille à "Actif"
  lastSignificantMovementTime = millis();

  // Lecture initiale de l'accéléromètre pour définir lastProcessedAccelX, Y, Z
  if (ReadAccel(lastProcessedAccelX, lastProcessedAccelY, lastProcessedAccelZ)) {
    isFirstAccelReadCycle = false;
  } else {
    isFirstAccelReadCycle = true;
    lastProcessedAccelX = 0.0f; // Valeurs par défaut si la lecture échoue
    lastProcessedAccelY = 0.0f;
    lastProcessedAccelZ = 0.0f;
  }
}

void loop() {
  // BtLoop();

  if (SleepState == 0) { // ----- MODE ACTIF -----
    digitalWrite(8, HIGH); // Allume la LED intégrée pour indiquer le mode actif

    // Logique des capteurs de pression pour les LEDs IR (conservée)
    if (ReadCapt("AV") < 0.95) { digitalWrite(2, HIGH); } else { digitalWrite(2, LOW); }
    if (ReadCapt("AR") < 0.95) { digitalWrite(3, HIGH); } else { digitalWrite(3, LOW); }


    // Détection de mouvement pour réinitialiser le timer d'inactivité (conservée)
    float currentAccelX, currentAccelY, currentAccelZ;
    if (ReadAccel(currentAccelX, currentAccelY, currentAccelZ)) {
      float deltaX = abs(currentAccelX - lastProcessedAccelX);
      float deltaY = abs(currentAccelY - lastProcessedAccelY);
      float deltaZ = abs(currentAccelZ - lastProcessedAccelZ);

      if (deltaX > MOVEMENT_DETECTION_THRESHOLD_DELTA ||
          deltaY > MOVEMENT_DETECTION_THRESHOLD_DELTA ||
          deltaZ > MOVEMENT_DETECTION_THRESHOLD_DELTA) {
        lastSignificantMovementTime = millis(); // Mouvement détecté, réinitialiser le timer
      }
      // Mettre à jour les valeurs de référence pour la prochaine itération
      lastProcessedAccelX = currentAccelX;
      lastProcessedAccelY = currentAccelY;
      lastProcessedAccelZ = currentAccelZ;
    }

    // Vérifier l'inactivité pour passer en mode veille
    if (millis() - lastSignificantMovementTime > INACTIVITY_DURATION_MS) {
      Serial.println("Inactivité > 1 min. Passage en mode veille...");
      // BtSend("Sleep"); // Supprimé
      SleepState = 1;
      
      digitalWrite(2, LOW); // S'assurer que les LEDs IR sont éteintes
      digitalWrite(3, LOW);
      // digitalWrite(LED_BUILTIN, LOW); // Optionnel
      Serial.println("Entrée en mode veille (attente de delta de mouvement pour réveil).");
      // lastProcessedAccelX, Y, Z conservent les dernières valeurs du mode actif
      // pour la première comparaison en mode veille.
    } else {
      // Délai court pendant le mode actif normal
      delay(100); // Ajustez selon la réactivité souhaitée et autres tâches
    }

  } else { // ----- MODE VEILLE (SleepState == 1) -----
    // En mode veille, on vérifie périodiquement si un mouvement (delta) est détecté.
    digitalWrite(8, LOW); // Éteint la LED intégrée pour indiquer le mode veille

    float currentSleepAccelX, currentSleepAccelY, currentSleepAccelZ;
    if (ReadAccel(currentSleepAccelX, currentSleepAccelY, currentSleepAccelZ)) {
      // Calculer la différence par rapport à la dernière lecture traitée (avant d'entrer en veille ou lors du dernier cycle de veille)
      float deltaX = abs(currentSleepAccelX - lastProcessedAccelX);
      float deltaY = abs(currentSleepAccelY - lastProcessedAccelY);
      float deltaZ = abs(currentSleepAccelZ - lastProcessedAccelZ);

      // MOVEMENT_DETECTION_THRESHOLD_DELTA est défini dans matitone.h (ex: 0.15f)
      if (deltaX > MOVEMENT_DETECTION_THRESHOLD_DELTA ||
          deltaY > MOVEMENT_DETECTION_THRESHOLD_DELTA ||
          deltaZ > MOVEMENT_DETECTION_THRESHOLD_DELTA) {
        Serial.println("Mouvement de réveil (delta) détecté ! Passage en mode actif.");
        // BtSend("WakeUp"); // Supprimé
        SleepState = 0;      // Changer l'état en Actif
        lastSignificantMovementTime = millis(); // Réinitialiser le timer d'inactivité
        
        // Mettre à jour lastProcessedAccelX, Y, Z avec les valeurs qui ont causé le réveil,
        // pour que le mode actif commence avec une référence correcte.
        lastProcessedAccelX = currentSleepAccelX;
        lastProcessedAccelY = currentSleepAccelY;
        lastProcessedAccelZ = currentSleepAccelZ;
        // La boucle reprendra en mode actif
      } else {
        // Pas de mouvement de réveil. Mettre à jour les valeurs "précédentes" pour la prochaine vérification en mode veille.
        // Cela garantit que le delta est toujours par rapport à la dernière lecture "stable" en veille.
        lastProcessedAccelX = currentSleepAccelX;
        lastProcessedAccelY = currentSleepAccelY;
        lastProcessedAccelZ = currentSleepAccelZ;
      }
    }

    // Si toujours en mode veille (pas de mouvement de réveil détecté ci-dessus)
    if (SleepState == 1) {
      // On utilise delay() pour mettre le CPU en sommeil léger et économiser de l'énergie.
      // La durée de ce delay détermine la fréquence à laquelle on vérifie les conditions de réveil.
      // Serial.println("En veille, attente du prochain cycle de vérification de mouvement (delta)..."); // Pour debug
      delay(PERIODIC_SLEEP_INTERVAL_MS); // Ou une valeur plus courte pour plus de réactivité au mouvement
    }
  }
}
