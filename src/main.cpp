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
volatile bool AV1UP;
volatile bool AR1UP;
volatile bool AV2;
volatile bool AR2;

void setup() {
  Serial.begin(9600);

  SetupCapt();
  BtSetup();
  SetupAccel();
  SetupButtons();

  AV1UP = false;
  AR1UP = false;
  AV2 = false;
  AR2 = false;
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
  BtLoop(); // je laisse le BL actif même en veille pour la demo, ensuite on le désactivera ?

  if (SleepState == 0) { // ----- MODE ACTIF -----
    digitalWrite(8, HIGH); // Allume la LED intégrée pour indiquer le mode actif

    // Lire les valeurs des capteurs une seule fois par itération de la boucle principale
    float currentValAV = ReadCapt("AV");
    float currentValAR = ReadCapt("AR");

    // --- Capteur AV - Niveau 1 ---
    // Seuil de pression: currentValAV < 2.0
    if (currentValAV < 2.0) {
      digitalWrite(2, HIGH); // Allumer la LED IR AV
      if (!AV1UP) {          // Si l'état "pressé Niveau 1" n'était pas déjà actif (flanc montant)
        AV1UP = true;        // Marquer comme actif
        BtSend("AV1UP");
        Serial.println("AV1UP");
      }
    } else { // currentValAV >= 2.0 (pression relâchée ou non atteinte pour Niveau 1)
      digitalWrite(2, LOW);  // Éteindre la LED IR AV
      if (AV1UP) {           // Si l'état "pressé Niveau 1" était actif (flanc descendant)
        AV1UP = false;       // Marquer comme inactif
        BtSend("AV1DOWN");
        Serial.println("AV1DOWN");
      }
    }

    // --- Capteur AR - Niveau 1 ---
    // Seuil de pression: currentValAR > 2.5 (Attention: suppose que pour AR, une valeur PLUS GRANDE signifie plus de pression)
    if (currentValAR > 2.5) {
      digitalWrite(3, HIGH); // Allumer la LED IR AR
      if (!AR1UP) {          // Si l'état "pressé Niveau 1" n'était pas déjà actif (flanc montant)
        AR1UP = true;        // Marquer comme actif
        BtSend("AR1UP");
        Serial.println("AR1UP");
      }
    } else { // currentValAR <= 2.5 (pression relâchée ou non atteinte pour Niveau 1)
      digitalWrite(3, LOW);  // Éteindre la LED IR AR
      if (AR1UP) {           // Si l'état "pressé Niveau 1" était actif (flanc descendant)
        AR1UP = false;       // Marquer comme inactif
        BtSend("AR1DOWN");
        Serial.println("AR1DOWN");
      }
    }

    // --- Capteur AV - Niveau 2 (Pression Forte) ---
    // Seuil de pression: currentValAV < 1.4
    if (currentValAV < 1.4) {
      if (!AV2) {            // Si l'état "pressé Niveau 2" n'était pas déjà actif (flanc montant)
        AV2 = true;          // Marquer comme actif
        BtSend("AV2");
        Serial.println("AV2");
      }
    } else { // currentValAV >= 1.4 (pression relâchée ou non atteinte pour Niveau 2)
      if (AV2) {             // Si l'état "pressé Niveau 2" était actif
        AV2 = false;         // Réinitialiser l'indicateur (pas de message "DOWN" ici, comme demandé)
      }
    }

    // --- Capteur AR - Niveau 2 (Pression Forte) ---
    // Seuil de pression: currentValAR > 3.0 (Attention: suppose que pour AR, une valeur PLUS GRANDE signifie plus de pression)
    if (currentValAR > 3.0) {
      if (!AR2) {            // Si l'état "pressé Niveau 2" n'était pas déjà actif (flanc montant)
        AR2 = true;          // Marquer comme actif
        BtSend("AR2");
        Serial.println("AR2");
      }
    } else { // currentValAR <= 3.0 (pression relâchée ou non atteinte pour Niveau 2)
      if (AR2) {             // Si l'état "pressé Niveau 2" était actif
        AR2 = false;         // Réinitialiser l'indicateur (pas de message "DOWN" ici, comme demandé)
      }
    }

    // Affichage des valeurs lues (comme dans votre code original)
    Serial.print("AV: ");
    Serial.print(currentValAV);
    Serial.print(" | AR: ");
    Serial.println(currentValAR);
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
