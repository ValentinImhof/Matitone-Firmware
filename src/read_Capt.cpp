#include "matitone.h"

void SetupCapt() {
    pinMode(A0, INPUT);  // Configure la broche A0 comme entrée analogique, capteur avant
    pinMode(A1, INPUT);  // Configure la broche A1 comme entrée analogique, capteur arrière
}

float ReadCapt(const char * sensor) {
    int val;
    float VCC = 3.3; // Define VCC
    float VOUT;

    if (strcmp(sensor, "AV") == 0) {
        val = analogRead(A0);
    } else if (strcmp(sensor, "AR") == 0) {
        val = analogRead(A1);
    } else {
        Serial.println("Capteur non reconnu");
        return 0.0f;
    }

    // Convert the analog value to voltage (VOUT)
    VOUT = (val / 1023.0) * VCC;

    return VOUT; // Return the voltage value
}

