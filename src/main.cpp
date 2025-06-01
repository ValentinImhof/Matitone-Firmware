/* Main pour le premier test le 20/05/2025
 * 
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

  SetupCapt();
  BtSetup();
  SetupAccel();
  SetupButtons();

  AV2UP = false;
  AR2UP = false;
  button1Pressed = false;
  button2Pressed = false;
  button3Pressed = false;

  pinMode(2, OUTPUT); // LED IR AV
  pinMode(3, OUTPUT); // LED IR AR
  pinMode(8, OUTPUT); // LED Rouge
  pinMode(A3, INPUT); // Lecture du niveau de batterie

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
  BtLoop(); // je laisse le BL actif même en veille pour la demo, ensuite on le désactivera

  if (SleepState == 0) { // ----- MODE ACTIF -----
    digitalWrite(8, HIGH); // Allume la LED intégrée pour indiquer le mode actif

    // Logique des capteurs de pression pour les LEDs IR (conservée)
    if (ReadCapt("AV") < 0.95) { digitalWrite(2, HIGH); } else { digitalWrite(2, LOW); }
    if (ReadCapt("AR") < 0.95) { digitalWrite(3, HIGH); } else { digitalWrite(3, LOW); }

    // Logique de l'envoi BL si forte pression flanc montant
    /*if (ReadCapt("AV") > 0.85 && !AV2UP) {
      AV2UP = true;
      BtSend("AV2UP");
      Serial.println("AV2UP envoyé");
    } else if (AV2UP) {
      AV2UP = false;
    }*/
    if (ReadCapt("AR") > 0.85 && !AR2UP) {
      AR2UP = true;
      BtSend("AR2UP");
      Serial.println("AR2UP envoyé");
    } else if (AR2UP && ReadCapt("AR") < 0.85) {
      AR2UP = false;
      BtSend("AR2DOWN");
    }

    // Logique des boutons
    if (button1Pressed) {
      BtSend("Button1");
      Serial.println("Button1 envoyé");
      button1Pressed = false;
    }
    if (button2Pressed) {
      BtSend("Button2");
      Serial.println("Button2 envoyé");
      button2Pressed = false;
    }
    if (button3Pressed) {
      BtSend("Button3");
      Serial.println("Button3 envoyé");
      button3Pressed = false;
    }

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
      Serial.println("Inactivité > 30s. Passage en mode veille...");
      BtSend("Sleep");
      SleepState = 1;
      
      digitalWrite(2, LOW); // S'assurer que les LEDs IR sont éteintes
      digitalWrite(3, LOW);

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

      if (deltaX > MOVEMENT_DETECTION_THRESHOLD_DELTA ||
          deltaY > MOVEMENT_DETECTION_THRESHOLD_DELTA ||
          deltaZ > MOVEMENT_DETECTION_THRESHOLD_DELTA) {
        Serial.println("Mouvement de réveil (delta) détecté ! Passage en mode actif.");
        BtSend("WakeUp");
        SleepState = 0;      // Changer l'état en Actif
        lastSignificantMovementTime = millis(); // Réinitialiser le timer d'inactivité
        
        // Mettre à jour lastProcessedAccelX, Y, Z avec les valeurs qui ont causé le réveil,
        lastProcessedAccelX = currentSleepAccelX;
        lastProcessedAccelY = currentSleepAccelY;
        lastProcessedAccelZ = currentSleepAccelZ;
        // La boucle reprendra en mode actif
      } else {
        // Pas de mouvement de réveil. Mettre à jour les valeurs "précédentes" pour la prochaine vérification en mode veille.
        lastProcessedAccelX = currentSleepAccelX;
        lastProcessedAccelY = currentSleepAccelY;
        lastProcessedAccelZ = currentSleepAccelZ;
      }
    }

    // Si toujours en mode veille (pas de mouvement de réveil détecté ci-dessus)
    if (SleepState == 1) {
      // On utilise delay() pour mettre le CPU en sommeil léger et économiser de l'énergie.
      delay(PERIODIC_SLEEP_INTERVAL_MS);
    }
  }
}
