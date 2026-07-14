#include <Arduino.h>
#include <NextionControl.h>
#include <jiggle.h>

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
    static uint8_t state = 0;
    static uint8_t data[7]; 
    static uint8_t index = 0;
    static int lastx = -1;
    static int lasty = -1;
    char cmd[64];

    if (Serial.available()) {
        char input = Serial.read();
        
        if (input == 'j' || input == 'J') {
            clearNextionGraphics();

            Serial.println("-> Fuehre Jiggle aus...");
            jiggle(); 
            Serial.println("-> Jiggle beendet.");
            
            while (Serial1.available()) Serial1.read();
        }
    }
    while (Serial1.available()) {
    // Nächstes Byte aus dem Nextion-Stream lesen.
    uint8_t b = static_cast<uint8_t>(Serial1.read());

    // Auf das Startbyte der Positionsnachricht warten.
    if (state == 0) {
        if (b == 0x68) { 
            // Neue Nachricht beginnt: Buffer zurücksetzen.
            index = 0;
            state = 1;
        }
        continue;
    }

    // Weitere Bytes der Nachricht im Puffer sammeln.
    data[index++] = b;
    if (index < 7) continue; 

    // Eine vollständige Nachricht endet mit drei 0xFF-Bytes.
    if (data[4] == 0xFF && data[5] == 0xFF && data[6] == 0xFF) {
        // Zwei 8-Bit-Werte zu einem 16-Bit-X- und Y-Wert zusammensetzen.
        int16_t x = (static_cast<int16_t>(data[1]) << 8) | data[0];
        int16_t y = (static_cast<int16_t>(data[3]) << 8) | data[2];


        if (abs(x-lastx) > 2 || abs(x-lasty) > 2){

            Serial.print("x = ");
            Serial.println(x);
            Serial.print("y = ");
            Serial.println(y);

            lastx =x;
            lasty =y;
        }

    }
}
}
