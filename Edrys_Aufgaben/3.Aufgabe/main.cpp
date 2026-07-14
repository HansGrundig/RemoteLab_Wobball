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
    initJiggle(9, 6, 5);
    clearNextionGraphics();
}

// ====================================================================
// --- 5. MAIN LOOP ---
// ====================================================================

void loop() {
    static uint8_t state = 0;
    static uint8_t data[7]; 
    static uint8_t index = 0;
    
    // SNAKE-PUFFER (enthält die letzten 10 Punkte)
    // Initialisiert mit -1, damit leere Einträge erkennbar sind
    static int16_t pathX[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    static int16_t pathY[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    static uint32_t pathTime[10] = {0}; 

    static int16_t lastpathX = -1;
    static int16_t lastpathY = -1;
    static uint32_t lastTouchTime = 0;
    static bool screenHasGraphics = false;

    const int16_t MAX_WIDTH = 800; 
    const int16_t MAX_HEIGHT = 480;

    static uint32_t lastPointTime = 0; // Verfolgt die Zeit des letzten gültigen Punktes
    static uint32_t velocity = 0;



    // 1. 5-Sekunden-Reset: Wenn 5s keine Berührung, Bildschirm löschen und zurücksetzen
    if (screenHasGraphics && (millis() - lastTouchTime > 5000)) {
        clearNextionGraphics();      
        screenHasGraphics = false; 
        
        // Empty the snake buffer
        for(int i = 0; i < 10; i++) {
            pathX[i] = -1; 
            pathY[i] = -1;
        }
    }

    if (Serial.available()) {
        char input = Serial.read();
        
        if (input == 'j' || input == 'J') {
            clearNextionGraphics();
            screenHasGraphics = false; 
            
            for(int i = 0; i < 10; i++) {
                pathX[i] = -1; 
                pathY[i] = -1;
            }

            Serial.println("-> Fuehre Jiggle aus...");
            jiggle(); 
            Serial.println("-> Jiggle beendet.");
            
            while (Serial1.available()) Serial1.read();
        }
    }
    // 2. Auf benutzerdefinierte Nextion-Daten hören
    while (Serial1.available()) {
        uint8_t b = static_cast<uint8_t>(Serial1.read());

        if (state == 0) {
            if (b == 0x68) { 
                index = 0;
                state = 1;
            }
            continue;
        }

        data[index++] = b;
        if (index < 7) continue; 

        if (data[4] == 0xFF && data[5] == 0xFF && data[6] == 0xFF) {
            
            int16_t x = (static_cast<int16_t>(data[1]) << 8) | data[0];
            int16_t y = (static_cast<int16_t>(data[3]) << 8) | data[2];

            lastTouchTime = millis();
            screenHasGraphics = true;
            
            // Nur aktualisieren, wenn sich die Berührungsposition geändert hat
            if (pathX[9] == -1 || abs(x - pathX[9]) > 2 || abs(y - pathY[9]) > 2) { 
                
                char cmd[64];

                // --- SCHRITT A: ALTE GRAFIK ENTFERNEN ---
                // Ältesten grünen Punkt löschen (falls Puffer voll)
                if (pathX[0] != -1) {
                    sprintf(cmd, "cirs %d,%d,6,65535", pathX[0], pathY[0]);
                    sendNextionCommand(cmd);
                }

                // --- SCHRITT B: HISTORIE VERSCHIEBEN ---
                for (int i = 0; i < 9; i++) {
                    pathX[i] = pathX[i+1];
                    pathY[i] = pathY[i+1];
                    pathTime[i] = pathTime[i+1]; 
                }
                
                // --- SCHRITT C: NEUEN PUNKT HINZUFÜGEN & ZEICHNEN ---
                pathX[9] = x;
                pathY[9] = y;
                pathTime[9] = millis();

                sprintf(cmd, "cirs %d,%d,3,31", x, y);
                sendNextionCommand(cmd);
                    
                // Letzte Punkt-Koordinaten aktualisieren
                lastpathX = pathX[9]; 
                lastpathY = pathY[9]; 
            }

            
        }
        state = 0;
        index = 0;
    }
}