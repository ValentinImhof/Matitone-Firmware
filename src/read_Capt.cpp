#include "matitone.h"

void SetupCapt() {
    pinMode(A0, INPUT);  // Configure la broche A0 comme entrée analogique
}

void LoopCapt() {
    int val;
    float VCC = 3.3; // Define VCC
    float VOUT;
    float RC;

    // Read the analog value
    val = analogRead(A0);
    // Convert the analog value to voltage (VOUT)
    VOUT = (val / 1023.0) * VCC;
    // Calculate resistance (RC) RC = (510 * VCC / VOUT) - 510
    if (VOUT > 0) { // Avoid division by zero
    RC = (510.0 * VCC / VOUT) - 510.0;
    } else {
    RC = -1; // Indicate invalid resistance for VOUT = 0
    }
    // Output VOUT and RC over serial
    Serial.print("VOUT: ");
    Serial.print(VOUT, 3);
    Serial.print(" V RC: ");
    if (RC >= 0) {
    Serial.print(RC, 3);
    Serial.println(" kOhm");
    } else {
    Serial.println("Invalid (VOUT = 0)");
    }

}