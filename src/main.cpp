#include <Arduino.h>
#include <NextionControl.h>

constexpr uint32_t kMonitorBaud = 9600;
constexpr uint32_t kNextionBaud = 115200; 

// ====================================================================
// --- 1. GLOBALE KONSTANTEN ---
// ====================================================================

const int16_t MAX_WIDTH = 800; 
const int16_t MAX_HEIGHT = 480;


// ====================================================================
// --- 4. ARDUINO SETUP ---
// ====================================================================

void setup() {
    Serial.begin(kMonitorBaud);

    Serial1.begin(9600);
    delay(500);
    sendNextionCommand("baud=115200"); 
    delay(100);
    Serial1.begin(kNextionBaud);
    delay(1000);

    clearNextionGraphics();
}

// ====================================================================
// --- 5. MAIN LOOP ---
// ====================================================================
void loop() {
    char cmd[64];
    sprintf(cmd, "cirs %d,%d,2,31", 400 , 240);     // 400,240 = Mitte des Displays
    sendNextionCommand(cmd);
}
