#include <Arduino.h>

constexpr uint32_t kMonitorBaud = 9600;
constexpr uint32_t kNextionBaud = 115200; 

// ====================================================================
// --- 1. GLOBAL PHYSICAL CONSTANTS ---
// ====================================================================
const int16_t MAX_WIDTH = 800; 
const int16_t MAX_HEIGHT = 480;

const int16_t WALL_LEFT = 10;
const int16_t WALL_RIGHT = 793;
const int16_t WALL_TOP = 26;
const int16_t WALL_BOTTOM = 438;

// ====================================================================
// --- 2. NEXTION HELPER FUNCTIONS ---
// ====================================================================
void sendNextionCommand(const char *command) {
    Serial1.print(command);
    Serial1.write(0xFF);
    Serial1.write(0xFF);
    Serial1.write(0xFF);
}

void clearNextionGraphics() {
    sendNextionCommand("ref 0"); 
    Serial.println("Screen graphics wiped.");
}

// ====================================================================
// --- 3. ARDUINO SETUP ---
// ====================================================================
void setup() {
    Serial.begin(kMonitorBaud);
    Serial1.begin(9600);
    delay(500);
    
    sendNextionCommand("baud=115200"); 
    delay(100);
    
    Serial1.begin(kNextionBaud);
    delay(1000);

    Serial.println("Simple Dynamic Laser Ready!");
    clearNextionGraphics();
}

// ====================================================================
// --- 4. MAIN TRACKING LOOP ---
// ====================================================================
// ====================================================================
// --- 4. MAIN TRACKING LOOP ---
// ====================================================================
void loop() {
    static uint8_t state = 0;
    static uint8_t data[7]; 
    static uint8_t index = 0;
    
    // Past Trail (Blue)
    static int16_t pathX[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    static int16_t pathY[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    static uint32_t pathTime[10] = {0};

    // Future Trail (Red)
    static int16_t lastPredX[10];
    static int16_t lastPredY[10];
    static uint8_t predCount = 0;

    static uint32_t lastTouchTime = 0;
    static bool screenHasGraphics = false;
    static uint32_t lastPointTime = 0; 
    
    static uint32_t lastPredictionDrawTime = 0;

    // 5-SECOND SCREEN WIPE
    if (screenHasGraphics && (millis() - lastTouchTime > 5000)) {
        clearNextionGraphics();
        predCount = 0;         
        screenHasGraphics = false; 
        
        for(int i = 0; i < 10; i++) {
            pathX[i] = -1; 
            pathY[i] = -1;
            pathTime[i] = 0;
        }
    }

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

            uint32_t currentTime = millis();
            uint32_t dt = currentTime - lastPointTime; 
            lastPointTime = currentTime; 
            if (dt == 0) dt = 1;
            
            if (pathX[9] == -1 || abs(x - pathX[9]) > 1 || abs(y - pathY[9]) > 1) { 
                
                char cmd[64];

                // --- A. DRAW PAST TRACKING ---
                if (pathX[0] != -1) {
                    sprintf(cmd, "cirs %d,%d,6,65535", pathX[0], pathY[0]);
                    sendNextionCommand(cmd);
                }

                for (int i = 0; i < 9; i++) {
                    pathX[i] = pathX[i+1];
                    pathY[i] = pathY[i+1];
                    pathTime[i] = pathTime[i+1]; 
                }
                
                pathX[9] = x;
                pathY[9] = y;
                pathTime[9] = millis();

                sprintf(cmd, "cirs %d,%d,3,31", x, y);
                sendNextionCommand(cmd);

                // --- B. DRAW FUTURE LASER (10Hz) ---
                if (pathX[5] != -1 && (millis() - lastPredictionDrawTime > 100)) {
                    
                    // 1. Erase old red dots
                    for (int i = 0; i < predCount; i++) {
                        sprintf(cmd, "cirs %d,%d,5,65535", lastPredX[i], lastPredY[i]);
                        sendNextionCommand(cmd);
                    }
                    predCount = 0; 
                    
                    // 2. Measure velocity
                    int32_t dx = pathX[9] - pathX[5];
                    int32_t dy = pathY[9] - pathY[5];
                    uint32_t totalDt = pathTime[9] - pathTime[5];
                    if (totalDt == 0) totalDt = 1; 

                    float distance = sqrt((dx * dx) + (dy * dy));
                    float velocity_ms = distance / totalDt; 

                    if (velocity_ms > 0.1) {
                        float dX = dx / distance; 
                        float dY = dy / distance; 
                        
                        float gX = pathX[9]; 
                        float gY = pathY[9]; 

                        // -----------------------------------------------------------------
                        // --- THE NON-LINEAR (QUADRATIC) VELOCITY MAPPING ---
                        // Minimum distance is 5.0. It grows exponentially as velocity goes up!
                        // (Adjust the '20.0' multiplier if you want it to stretch out faster)
                        float dynamicStep = 5.0 + (velocity_ms * velocity_ms * 15.0);
                        
                        // Constrain it safely between 5px and 150px so it never glitches
                        float currentStepSize = constrain(dynamicStep, 5.0, 150.0);
                        // -----------------------------------------------------------------

                        int maxDots = 15;

                        // THE CHAIN-LINK LOOP
                        for (int i = 0; i < maxDots; i++) {
                            
                            gX += (dX * currentStepSize);
                            gY += (dY * currentStepSize);

                            bool bounced = false;

                            // Fold space if it hits a wall
                            while (gX < WALL_LEFT || gX > WALL_RIGHT || gY < WALL_TOP || gY > WALL_BOTTOM) {
                                if (gX > WALL_RIGHT) { gX = WALL_RIGHT - (gX - WALL_RIGHT); dX = -dX; bounced = true; }
                                else if (gX < WALL_LEFT) { gX = WALL_LEFT + (WALL_LEFT - gX); dX = -dX; bounced = true; }
                                
                                if (gY > WALL_BOTTOM) { gY = WALL_BOTTOM - (gY - WALL_BOTTOM); dY = -dY; bounced = true; }
                                else if (gY < WALL_TOP) { gY = WALL_TOP + (WALL_TOP - gY); dY = -dY; bounced = true; }
                            }

                            // --- PENALTY FOR HITTING A BORDER ---
                            if (bounced) {
                                currentStepSize *= 0.75; // Slash the distance between dots by 40% on impact!
                                
                                // Never let the dots stack completely on top of each other
                                if (currentStepSize < 5.0) {
                                    currentStepSize = 5.0; 
                                }
                            }

                            // Constrain as a final safety net
                            int16_t drawX = constrain((int16_t)gX, 0, MAX_WIDTH - 1);
                            int16_t drawY = constrain((int16_t)gY, 0, MAX_HEIGHT - 1);

                            // Draw the red dot
                            sprintf(cmd, "cirs %d,%d,2,63488", drawX, drawY);
                            sendNextionCommand(cmd);

                            lastPredX[predCount] = drawX;
                            lastPredY[predCount] = drawY;
                            predCount++;
                        }
                    }

                    lastPredictionDrawTime = millis();
                }
            }
        }
        state = 0;
        index = 0;
    }
}