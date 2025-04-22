#include "matitone.h"

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLECharacteristic txCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 20);

// Variable globale pour suivre l'état de connexion
bool connected = false;

void BtSetup() {
  if (!BLE.begin()) {
    Serial.println("Échec BLE !");
    printf("Échec BLE !\n");
    while (1);
  }

  BLE.setLocalName("Nano33BLE");
  BLE.setAdvertisedService(ledService);
  ledService.addCharacteristic(txCharacteristic);
  BLE.addService(ledService);
  
  BLE.advertise();
  Serial.println("BLE prêt. Scannez pour 'Nano33BLE'");
  printf("BLE prêt");
}

void BtLoop() {
  // Gestion robuste de la connexion
  BLEDevice central = BLE.central();
  
  if (central && !connected) {
    connected = true;
    Serial.print("Connecté à : ");
    Serial.println(central.address());
    printf("Connecté");
    
    // Désactive l'advertissement pour économiser l'énergie
    BLE.stopAdvertise();
  }
  
  if (!central && connected) {
    connected = false;
    Serial.println("Déconnecté. Redémarrage de l'advertissement...");
    BLE.advertise(); // Re-devient visible
  }
}

void BtSend(const char* message) {
  // Envoi de données SEULEMENT si connecté
  static unsigned long lastSend = 0;
  if (connected && millis() - lastSend > 2000) {
    const char* buffer = message;
    txCharacteristic.writeValue(buffer, strlen(buffer));
    Serial.println("Message envoyé");
    lastSend = millis();
  }
  // Si pas connecté, affiche un message d'erreur
  else if (!connected) {
    Serial.println("Erreur : pas de connexion BLE !");
    printf("Erreur : pas de connexion BLE !\n");
  }
}