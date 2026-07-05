#include <Arduino.h>

constexpr uint32_t kMonitorBaud = 9600;
constexpr uint32_t kNextionBaud = 38400;

// Screen Resolution Constraints
constexpr int16_t kMaxWidth = 320; 
constexpr int16_t kMaxHeight = 240;

void sendNextionCommand(const char *command) {
    Serial1.print(command);
    Serial1.write(0xFF);
    Serial1.write(0xFF);
    Serial1.write(0xFF);
}

// Helper to instantly wipe all lines and restore background/pictures
void clearNextionGraphics() {
    // Note: Change '0' if your page has a different ID or name (e.g., "ref page1")
    sendNextionCommand("ref 0"); 
    Serial.println("Screen graphics wiped.");
}

void setup() {
    Serial.begin(kMonitorBaud);

    Serial1.begin(9600);
    delay(500);
    
    sendNextionCommand("baud=38400"); 
    delay(100);
    
    Serial1.begin(kNextionBaud);

    delay(1000);
    sendNextionCommand("bkcmd=0");
    sendNextionCommand("sendxy=1");

    Serial.println("Nextion Marble Tracker Ready!");
}

void loop() {
    static uint8_t state = 0;
    static uint8_t data[8];
    static uint8_t index = 0;
    static uint8_t packetType = 0;
    
    static int16_t histX[3] = {0, 0, 0};
    static int16_t histY[3] = {0, 0, 0};
    static uint8_t histCount = 0; 

    static int16_t lastStartX = -1;
    static int16_t lastStartY = -1;
    static int16_t lastPredX = -1;
    static int16_t lastPredY = -1;

    // --- NEW: Time Tracking Variables ---
    static uint32_t lastTouchTime = 0;
    static bool screenHasGraphics = false;

    // 1. TIMEOUT CHECK: Wipe screen after 10 seconds of inactivity
    if (screenHasGraphics && (millis() - lastTouchTime > 10000)) {
        clearNextionGraphics();
        histCount = 0;          // Reset history
        lastPredX = -1;         // Reset prediction
        screenHasGraphics = false; 
    }

    // 2. LIFT CHECK: If no touches for 250ms, prepare for a "New Path"
    if (histCount > 0 && (millis() - lastTouchTime > 250)) {
        histCount = 0; 
        lastPredX = -1;
    }

    while (Serial1.available()) {
        uint8_t b = static_cast<uint8_t>(Serial1.read());

        if (state == 0) {
            if (b == 0x67) { 
                packetType = b;
                index = 0;
                state = 1;
            }
            continue;
        }

        data[index++] = b;
        if (index < 8) continue;

        if (data[5] == 0xFF && data[6] == 0xFF && data[7] == 0xFF) {
            int16_t x = (static_cast<int16_t>(data[0]) << 8) | data[1];
            int16_t y = (static_cast<int16_t>(data[2]) << 8) | data[3];
            uint8_t touchEvent = data[4]; 

            if (packetType == 0x67) {
                if (touchEvent == 1) { 
                    
                    // IF starting a brand new path, wipe the old one first
                    //if (histCount == 0 && screenHasGraphics) {
                    //    clearNextionGraphics();
                    //}

                    // Log the time of this touch
                    lastTouchTime = millis();
                    screenHasGraphics = true;
                    
                    if (histCount == 0 || (abs(x - histX[2]) > 2 || abs(y - histY[2]) > 2)) {
                        
                        histX[0] = histX[1]; histY[0] = histY[1];
                        histX[1] = histX[2]; histY[1] = histY[2];
                        histX[2] = x;        histY[2] = y;

                        if (histCount < 3) histCount++;

                        if (histCount >= 2) {
                            char cmd[64];
                            

                            if (lastPredX != -1) {
                                sprintf(cmd, "line %d,%d,%d,%d,65535", lastStartX, lastStartY, lastPredX, lastPredY);
                                sendNextionCommand(cmd);
                            }

                            // Draw MARBLE TRAIL (Green)
                            sprintf(cmd, "line %d,%d,%d,%d,2016", histX[1], histY[1], histX[2], histY[2]);
                            sendNextionCommand(cmd);

                            // Calculate NEW PREDICTION
                            if (histCount == 3) {
                                int16_t vx = histX[2] - histX[1];
                                int16_t vy = histY[2] - histY[1];
                                int16_t ax = (histX[2] - histX[1]) - (histX[1] - histX[0]);
                                int16_t ay = (histY[2] - histY[1]) - (histY[1] - histY[0]);

                                int16_t predX = histX[2] + (vx * 5) + (ax * 5);
                                int16_t predY = histY[2] + (vy * 5) + (ay * 5);

                                predX = constrain(predX, 0, kMaxWidth - 1);
                                predY = constrain(predY, 0, kMaxHeight - 1);

                                // Draw PREDICTION (Red)
                                sprintf(cmd, "line %d,%d,%d,%d,63488", histX[2], histY[2], predX, predY);
                                sendNextionCommand(cmd);

                                lastStartX = histX[2];
                                lastStartY = histY[2];
                                lastPredX = predX;
                                lastPredY = predY;
                            }
                        }
                    }
                }
            }
        }
        state = 0;
        index = 0;
    }
}