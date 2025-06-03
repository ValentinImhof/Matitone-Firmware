#include "matitone.h" 

// Configuration du pont diviseur
const float VOLTAGE_DIVIDER_R1 = 220000.0;  // Valeur de R1
const float VOLTAGE_DIVIDER_R2 = 220000.0;  // Valeur de R2
const float ADC_REFERENCE_VOLTAGE = 3.3;
const float ADC_MAX_READING = 1023.0;

void SetupBatteryLevel() {
  pinMode(A2, INPUT);
}

float ReadBatteryLevel() {
  int adcValue = analogRead(A2);
  float vout = adcValue * (ADC_REFERENCE_VOLTAGE / ADC_MAX_READING);
  
  // Calculer la tension réelle de la batterie (Vin) en utilisant la formule du pont diviseur
  // Vin = Vout * (R1 + R2) / R2
  float vin = vout * (VOLTAGE_DIVIDER_R1 + VOLTAGE_DIVIDER_R2) / VOLTAGE_DIVIDER_R2;

  // Retourner la tension en volts
  return vin;
}