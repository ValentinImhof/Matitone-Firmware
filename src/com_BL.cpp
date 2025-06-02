#include "matitone.h"

// UUIDs
BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214");

// Caractéristique existante pour les données
BLECharacteristic dataCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 20); // Renommée txCharacteristic en dataCharacteristic pour plus de clarté

// NOUVELLE caractéristique pour les notifications de déclenchement
BLECharacteristic triggerCharacteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 4); // Peut contenir un compteur sur 4 octets (uint32_t)
static uint32_t eventNotificationCounter = 0; // Compteur pour la caractéristique de déclenchement

bool connected = false;

void BtSetup() {
  if (!BLE.begin()) {
    Serial.println("Échec BLE !");
    while (1);
  }

  BLE.setLocalName("Nano33BLE");
  BLE.setAdvertisedService(ledService);
  
  ledService.addCharacteristic(dataCharacteristic); // Ajout de la caractéristique de données
  ledService.addCharacteristic(triggerCharacteristic); // Ajout de la NOUVELLE caractéristique de déclenchement

  BLE.addService(ledService);

  BLE.advertise();
  Serial.println("BLE prêt. Scannez pour 'Nano33BLE'");
}

void BtLoop() {
  BLEDevice central = BLE.central();

  if (central && !connected) {
    connected = true;
    Serial.print("Connecté à : ");
    Serial.println(central.address());
    BLE.stopAdvertise();

    // Initialiser la dataCharacteristic avec une valeur par défaut si besoin (déjà géré)
    const char* initialMessage = "INIT";
    dataCharacteristic.writeValue(initialMessage, strlen(initialMessage));
    Serial.println("Valeur initiale (data) envoyée.");

    // Optionnel: initialiser aussi le trigger si vous voulez une notif au démarrage
    // eventNotificationCounter++;
    // triggerCharacteristic.writeValue(eventNotificationCounter);
  }
  
  if (!central && connected) {
    connected = false;
    Serial.println("Déconnecté. Redémarrage de l'advertissement...");
    BLE.advertise();
  }
}

void BtSend(const char* baseMessage) {
  static unsigned long lastSendTime = 0;

  if (connected && millis() - lastSendTime > 1000) {
    // 1. Écrire le message réel dans la caractéristique de données
    dataCharacteristic.writeValue((const uint8_t*)baseMessage, strlen(baseMessage));
    Serial.print("Data Characteristic MAJ: ");
    Serial.println(baseMessage);

    // 2. Mettre à jour la caractéristique de déclenchement pour forcer une notification
    eventNotificationCounter++;
    triggerCharacteristic.writeValue(eventNotificationCounter); // ArduinoBLE gère l'envoi pour les types numériques
    Serial.print("Trigger Characteristic MAJ (notification envoyée): ");
    Serial.println(eventNotificationCounter);

    lastSendTime = millis();
  }
  else if (!connected) {
    Serial.println("Erreur : pas de connexion BLE !");
  }
}