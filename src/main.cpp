/*
 * IMPORTANT : ERREURS DE LINKER PERSISTANTES (undefined reference to LowPower / deepSleep)
 * ---------------------------------------------------------------------------------------
 * Les erreurs "undefined reference to `ArduinoLowPowerClass::deepSleep`" ou "undefined reference to `LowPower`"
 * persistent malgré les vérifications indiquent un problème de liaison (linking) de la bibliothèque
 * ArduinoLowPower avec le cœur Mbed OS de la Nano 33 BLE dans l'environnement PlatformIO.
 * L'éditeur de liens ne trouve pas le code compilé de la bibliothèque.
 *
 * Pistes à explorer :
 * 1. Vérification de `platformio.ini` : Assurez-vous que `lib_deps` contient BIEN :
 * `arduino-libraries/ArduinoLowPower`
 * C'est la référence correcte pour la bibliothèque officielle.
 *
 * 2. Nettoyage en profondeur et Reconstruction :
 * - Utilisez l'option "Clean" de PlatformIO.
 * - Si cela ne suffit pas, essayez de supprimer manuellement le dossier `.pio` à la racine
 * de votre projet, puis laissez PlatformIO tout reconstruire (cela forcera le
 * retéléchargement et la reconstruction de toutes les dépendances).
 *
 * 3. Vérification de l'installation de la bibliothèque :
 * Après une tentative de compilation, inspectez le contenu du dossier
 * `.pio/libdeps/nano33ble/ArduinoLowPower` (ou le nom de votre environnement).
 * Vérifiez que les fichiers sources (.c, .cpp) et les en-têtes (.h) y sont présents.
 * La présence de `mbed` dans les sous-dossiers de la bibliothèque peut indiquer
 * la version compatible Mbed.
 *
 * 4. Version du Core Mbed :
 * Il pourrait y avoir une incompatibilité entre la version du framework `framework-arduino-mbed`
 * utilisée par PlatformIO (visible dans votre log : `framework-arduino-mbed @ 4.2.4`) et
 * la bibliothèque `ArduinoLowPower`. Vérifiez s'il existe des "issues" connues sur les
 * forums de PlatformIO ou d'Arduino concernant cette combinaison.
 *
 * 5. Minimal Test Case :
 * Créez un nouveau projet PlatformIO très simple pour la Nano 33 BLE qui ne fait
 * QU'inclure `<ArduinoLowPower.h>` et appeler `LowPower.deepSleep(1000);` dans le setup.
 * Si cela ne fonctionne pas non plus, cela isole le problème à l'interaction
 * PlatformIO <-> ArduinoLowPower <-> Mbed Core.
 *
 * Le code C++ ci-dessous pour l'utilisation de la bibliothèque est standard.
 * Le problème est très probablement lié à la configuration de l'environnement de build.
 */

/*Main pour le premier test le 20/05/2025
par interruption, les boutons envoient un message s'ils sont pressés
envoi en continu de deux messages différents selon niveau de pression des capts
led IR respective allumée dès le niveau de pression le plus faible*/

#include "matitone.h"

// Définitions pour les variables globales déclarées 'extern' dans matitone.h
// Ces variables sont maintenant définies ici, dans main.cpp.
int SleepState;
unsigned long lastSignificantMovementTime;
float lastProcessedAccelX, lastProcessedAccelY, lastProcessedAccelZ;
bool isFirstAccelReadCycle;

// Définitions pour les variables des boutons (doivent correspondre aux déclarations extern dans matitone.h)
// Si ces variables sont modifiées dans des interruptions, 'volatile' est important.
volatile bool AV2UP;
volatile bool AR2UP;

// Les autres variables globales pour les boutons, si elles existent et sont extern,
// devraient aussi être définies ici. Par exemple :
// volatile bool button1Pressed;
// volatile bool button2Pressed;
// volatile bool button3Pressed;
// Assurez-vous que ces définitions correspondent à ce qui est déclaré 'extern' dans matitone.h

void setup() {
  Serial.begin(9600); // Initialisation de la communication série pour le débogage

  // Initialisation des différents modules (capteurs, Bluetooth, accéléromètre)
  SetupCapt();
  BtSetup();
  SetupAccel();

  // Initialisation des états des variables de contrôle des LEDs/logique
  AV2UP = false;
  AR2UP = false;

  pinMode(2, OUTPUT); // LED IR AVANT
  pinMode(3, OUTPUT); // LED IR ARRIERE
  digitalWrite(2, LOW); // Éteint la LED IR AVANT initialement
  digitalWrite(3, LOW); // Éteint la LED IR ARRIERE initialement

  SleepState = 0; // Initialisation de l'état de veille à "Actif"
  lastSignificantMovementTime = millis(); // Initialise le temps du dernier mouvement significatif

  // Lecture initiale de l'accéléromètre pour établir une référence
  if (ReadAccel(lastProcessedAccelX, lastProcessedAccelY, lastProcessedAccelZ)) {
    isFirstAccelReadCycle = false; // La première lecture a réussi
  } else {
    isFirstAccelReadCycle = true; // Marquer qu'il faudra réessayer ou utiliser des valeurs par défaut
    // Initialiser les valeurs si la lecture échoue pour éviter des comportements indéfinis
    lastProcessedAccelX = 0.0f;
    lastProcessedAccelY = 0.0f;
    lastProcessedAccelZ = 0.0f;
    Serial.println("Erreur lecture Accel au setup, valeurs par défaut utilisées.");
  }
}

void loop() {
  BtLoop(); // Gérer la communication Bluetooth en continu

  if (SleepState == 0) { // ----- MODE ACTIF -----
    // Optionnel: Indiquer l'état actif avec la LED intégrée
    // digitalWrite(LED_BUILTIN, HIGH);

    // Logique de contrôle de la LED IR basée sur le capteur de pression AVANT
    if (ReadCapt("AV") < 0.95) { // Seuil pour allumer la LED
      digitalWrite(2, HIGH); // Allumer LED IR AVANT
    } else {
      digitalWrite(2, LOW);  // Éteindre LED IR AVANT
    }
    // Vous pourriez ajouter une logique similaire pour le capteur ARRIERE et la LED 3 si nécessaire

    // Détection de mouvement pour réinitialiser le timer d'inactivité
    float currentAccelX, currentAccelY, currentAccelZ;
    if (ReadAccel(currentAccelX, currentAccelY, currentAccelZ)) {
      // Calculer la différence absolue par rapport à la dernière lecture traitée
      float deltaX = abs(currentAccelX - lastProcessedAccelX);
      float deltaY = abs(currentAccelY - lastProcessedAccelY);
      float deltaZ = abs(currentAccelZ - lastProcessedAccelZ);

      // Si un mouvement au-delà du seuil est détecté
      if (deltaX > MOVEMENT_DETECTION_THRESHOLD_DELTA ||
          deltaY > MOVEMENT_DETECTION_THRESHOLD_DELTA ||
          deltaZ > MOVEMENT_DETECTION_THRESHOLD_DELTA) {
        lastSignificantMovementTime = millis(); // Mouvement détecté, réinitialiser le timer
        // Serial.println("Mouvement actif."); // Pour débogage
      }
      // Mettre à jour les valeurs de référence pour la prochaine itération
      lastProcessedAccelX = currentAccelX;
      lastProcessedAccelY = currentAccelY;
      lastProcessedAccelZ = currentAccelZ;
    }

    // Vérifier l'inactivité pour passer en mode veille
    if (millis() - lastSignificantMovementTime > INACTIVITY_DURATION_MS) {
      Serial.println("Inactivité > 1 min. Passage en veille...");
      BtSend("Sleep"); // Envoyer un message Bluetooth pour indiquer le passage en veille
      SleepState = 1;  // Changer l'état pour la veille
      
      digitalWrite(2, LOW); // S'assurer que la LED IR AVANT est éteinte
      digitalWrite(3, LOW); // S'assurer que la LED IR ARRIERE est éteinte
      // digitalWrite(LED_BUILTIN, LOW); // Optionnel: éteindre la LED intégrée

      Serial.println("Entrée en deepSleep pour 5s (vérification périodique)...");
      // Mettre l'Arduino en mode de sommeil profond pour économiser l'énergie
      LowPower.deepSleep(PERIODIC_SLEEP_INTERVAL_MS); 
      // Au réveil (par timer ou interruption externe), la loop() reprendra et SleepState sera à 1.
    } else {
      delay(100); // Délai court pendant le mode actif pour ne pas surcharger le processeur
    }

  } else { // ----- MODE VEILLE (SleepState == 1) -----
    // L'appareil vient de se réveiller du deepSleep.
    // Optionnel: S'assurer que la LED intégrée est éteinte
    // digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(2, LOW); // S'assurer que la LED IR AVANT est éteinte
    digitalWrite(3, LOW); // S'assurer que la LED IR ARRIERE est éteinte

    Serial.println("Sortie de deepSleep (état veille), vérification mouvement pour réveil...");
    float accelX, accelY, accelZ;
    if (ReadAccel(accelX, accelY, accelZ)) {
      // Vérifier si un mouvement significatif (au-delà du seuil de réveil) justifie le réveil complet
      if (abs(accelX) > ACCEL_WAKE_UP_THRESHOLD ||
          abs(accelY) > ACCEL_WAKE_UP_THRESHOLD ||
          abs(accelZ) > ACCEL_WAKE_UP_THRESHOLD) {
        
        Serial.println("Mouvement de réveil détecté ! Passage en mode actif.");
        BtSend("WakeUp");    // Envoyer un message Bluetooth pour indiquer le réveil
        SleepState = 0;      // Changer l'état en Actif

        // Réinitialiser les références de l'accéléromètre pour la nouvelle période active
        if (!ReadAccel(lastProcessedAccelX, lastProcessedAccelY, lastProcessedAccelZ)) {
            // En cas d'échec de lecture, initialiser à zéro pour éviter des problèmes
            lastProcessedAccelX = 0.0f; 
            lastProcessedAccelY = 0.0f; 
            lastProcessedAccelZ = 0.0f;
            Serial.println("Erreur lecture Accel au réveil, valeurs par défaut utilisées.");
        }
        lastSignificantMovementTime = millis(); // Le système est maintenant actif, réinitialiser le timer d'inactivité
        // La prochaine itération de loop() exécutera la logique du mode Actif.
      } else {
        // Pas de mouvement suffisant, retourner en deepSleep
        Serial.println("Aucun mouvement de réveil. Retour en deepSleep pour 5s...");
        LowPower.deepSleep(PERIODIC_SLEEP_INTERVAL_MS);
      }
    } else {
      // Si la lecture de l'IMU échoue en mode veille, retourner en deepSleep par sécurité
      Serial.println("Erreur lecture IMU en mode veille. Retour en deepSleep...");
      LowPower.deepSleep(PERIODIC_SLEEP_INTERVAL_MS);
    }
  }
}
