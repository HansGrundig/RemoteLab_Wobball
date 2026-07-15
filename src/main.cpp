#include <Arduino.h>
#include <NextionControl.h>
#include <MPU6050.h>
#include <jiggle.h> 

constexpr uint32_t kMonitorBaud = 9600;
constexpr uint32_t kNextionBaud = 115200;

// ====================================================================
// --- 1. GLOBALE KONSTANTEN ---
// ====================================================================
const int16_t MAX_WIDTH = 800; 
const int16_t MAX_HEIGHT = 480;
const float WALL_LEFT = 47.0;
const float WALL_RIGHT = 748.0;
const float WALL_TOP = 55.0;
const float WALL_BOTTOM = 425.0;

// --- KALIBRIERUNG ---
const float GRAVITY_SCALE = 6500.0; 
const float BOUNCE_FACTOR = 0.48;   

MPU6050 mpu;

// ====================================================================
// --- 2. DIE PRÄDIKTION (Physik-Engine) ---
// ====================================================================
void predict(int16_t pathX[10], int16_t pathY[10], uint32_t pathTime[10]) {
    if (pathX[0] == -1 || pathX[9] == -1) return;

    char cmd[64];

    // Geschwindigkeit & Reibung aus dem Puffer berechnen
    float dt_start = (pathTime[4] - pathTime[0]) / 1000.0;
    if (dt_start < 0.001) dt_start = 0.001;
    float dist_start = sqrt(pow(pathX[4] - pathX[0], 2) + pow(pathY[4] - pathY[0], 2));
    float v_start = dist_start / dt_start;
    float t_mid_start = (pathTime[4] + pathTime[0]) / 2000.0;

    float dt_end = (pathTime[9] - pathTime[5]) / 1000.0;
    if (dt_end < 0.001) dt_end = 0.001;
    float dist_end = sqrt(pow(pathX[9] - pathX[5], 2) + pow(pathY[9] - pathY[5], 2));
    float v_end = dist_end / dt_end;
    float t_mid_end = (pathTime[9] + pathTime[5]) / 2000.0;

    float t_diff = t_mid_end - t_mid_start;
    float acceleration = (v_end - v_start) / (t_diff > 0.001 ? t_diff : 0.001);
    float live_friction = -acceleration;

    if (live_friction < 20.0) live_friction = 100.0;
    
    // Start-Vektor
    float vx = (pathX[9] - pathX[5]) / dt_end;
    float vy = (pathY[9] - pathY[5]) / dt_end;

    // Neigung des Tisches über externe MPU6050
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);
    float ax_g = (float)ax / 16384.0;
    float ay_g = (float)ay / 16384.0;
    float gx = -ay_g * GRAVITY_SCALE; 
    float gy = -ax_g * GRAVITY_SCALE;

    // Euler-Physik simulieren
    float simX = pathX[9];
    float simY = pathY[9];
    float lastDrawX = simX;
    float lastDrawY = simY;
    float dt = 0.02;     
    int max_steps = 400; 

    for (int step = 0; step < max_steps; step++) {
        vx += gx * dt;
        vy += gy * dt;

        float current_speed = sqrt(vx*vx + vy*vy);
        float speed_drop = live_friction * dt;

        if (current_speed > 0) {
            if (current_speed < speed_drop) {
                vx = 0; vy = 0; current_speed = 0; 
            } else {
                float multiplier = (current_speed - speed_drop) / current_speed;
                vx *= multiplier;
                vy *= multiplier;
            }
        }

        float gravity_magnitude = sqrt(gx*gx + gy*gy);
        if (current_speed == 0 && gravity_magnitude <= live_friction) break; 

        simX += vx * dt;
        simY += vy * dt;

        if (simX <= WALL_LEFT)   { simX = WALL_LEFT;   vx = -vx * BOUNCE_FACTOR; }
        if (simX >= WALL_RIGHT)  { simX = WALL_RIGHT;  vx = -vx * BOUNCE_FACTOR; }
        if (simY <= WALL_TOP)    { simY = WALL_TOP;    vy = -vy * BOUNCE_FACTOR; }
        if (simY >= WALL_BOTTOM) { simY = WALL_BOTTOM; vy = -vy * BOUNCE_FACTOR; }

        if (step % 3 == 0) {
            sprintf(cmd, "line %d,%d,%d,%d,33840", (int)lastDrawX, (int)lastDrawY, (int)simX, (int)simY);
            sendNextionCommand(cmd);
            lastDrawX = simX;
            lastDrawY = simY;
        }
    }

    // Zielfadenkreuz am Ende zeichnen
    sprintf(cmd, "line %d,%d,%d,%d,64512", (int)lastDrawX, (int)lastDrawY, (int)simX, (int)simY);
    sendNextionCommand(cmd);
    sprintf(cmd, "cir %d,%d,15,63488", (int)simX, (int)simY);
    sendNextionCommand(cmd);
    sprintf(cmd, "line %d,%d,%d,%d,63488", (int)simX-5, (int)simY, (int)simX+5, (int)simY);
    sendNextionCommand(cmd);
    sprintf(cmd, "line %d,%d,%d,%d,63488", (int)simX, (int)simY-5, (int)simX, (int)simY+5);
    sendNextionCommand(cmd);
}

// ====================================================================
// --- 3. ARDUINO SETUP ---
// ====================================================================
void setup() {
    Serial.begin(kMonitorBaud);
    
    Wire.begin();
    mpu.initialize();
    
    // Servos initialisieren
    initJiggle(9, 6, 5);
    
    Serial1.begin(9600);
    delay(500);
    sendNextionCommand("baud=115200");
    delay(100);
    Serial1.begin(kNextionBaud);
    delay(1000);
    
    clearNextionGraphics();
    Serial.println("System Ready!");
    Serial.println("-> Druecke 'j' zum Wackeln (Jiggle)");
    Serial.println("-> Druecke 'p' fuer Aufnahme & Vorhersage");
}

// ====================================================================
// --- 4. MAIN LOOP ---
// ====================================================================
void loop() {
    static uint8_t state = 0;
    static uint8_t data[7]; 
    static uint8_t index = 0;
    
    static int16_t pathX[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    static int16_t pathY[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    static uint32_t pathTime[10] = {0}; 
    
    static uint32_t lastTouchTime = 0;
    static bool screenHasGraphics = false;
    static uint32_t lastPredictTime = 0;

    // --- Aufnahme-Status ---
    static bool isRecording = false;

    // 1. Serielle Eingabe vom Terminal (PC/Edrys) verarbeiten
    if (Serial.available()) {
        char input = Serial.read();
        
        if (input == 'j' || input == 'J') {
            clearNextionGraphics();
            clearNextionGraphics();
            
            // --- NEU: Prädiktion und Aufnahme explizit stoppen ---
            isRecording = false;       
            screenHasGraphics = false; 
            
            // --- NEU: Puffer leeren, damit keine alten Punkte gezeichnet werden ---
            for(int i = 0; i < 10; i++) {
                pathX[i] = -1;  
                pathY[i] = -1;
            }

            Serial.println("-> Führe Jiggle aus...");
            jiggle(); 
            Serial.println("-> Jiggle beendet. Prädiktion gestoppt.");
            
            // Empfangspuffer flushen, damit Mülldaten vom Wackeln gelöscht werden
            while (Serial1.available()) Serial1.read();
        }
        else if (input == 'r' || input == 'R'){
            clearNextionGraphics();
        }
        else if (input == 'p' || input == 'P') {
            Serial.println("-> Starte Aufzeichnung & Prädiktion!");
            
            // Puffer sicherheitshalber noch mal flushen
            while (Serial1.available()) Serial1.read();
            
            clearNextionGraphics();
            screenHasGraphics = false;
            
            // Snake-Puffer für einen sauberen Start leeren
            for(int i = 0; i < 10; i++) {
                pathX[i] = -1; 
                pathY[i] = -1;
            }
            
            // Aufnahme aktivieren
            isRecording = true;
            lastTouchTime = millis();
        }
    }

    // 2. Timeout: Wenn die Kugel still liegt (5 Sek. nichts passiert), Aufnahme beenden
    if (isRecording && screenHasGraphics && (millis() - lastTouchTime > 5000)) {
        clearNextionGraphics();      
        screenHasGraphics = false;             
        isRecording = false; // Aufnahme & Prädiktion stoppen
        
        Serial.println("-> Kugel liegt still. Aufnahme beendet.");
        
        for(int i = 0; i < 10; i++) {
            pathX[i] = -1; 
            pathY[i] = -1;
        }
    }

    // 3. Nextion Touch-Daten parsen (läuft im Hintergrund weiter)
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
            
            // WICHTIG: Punkte werden nur gezeichnet und für die 
            // Prädiktion genutzt, wenn 'p' gedrückt wurde (isRecording == true)
            if (isRecording) {
                int16_t x = (static_cast<int16_t>(data[1]) << 8) | data[0];
                int16_t y = (static_cast<int16_t>(data[3]) << 8) | data[2];
                lastTouchTime = millis();
                screenHasGraphics = true;
                
                if (pathX[9] == -1 || abs(x - pathX[9]) > 2 || abs(y - pathY[9]) > 2) {
                    
                    char cmd[64];
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
                    
                    if (pathX[0] != -1 && (millis() - lastPredictTime > 400)) {
                        predict(pathX, pathY, pathTime);
                        lastPredictTime = millis();
                    }
                }
            }
        }
        state = 0;
        index = 0;
    }
}