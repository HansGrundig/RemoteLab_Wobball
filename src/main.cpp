#include <Arduino.h>
#include <Arduino_LSM6DS3.h> // Bibliothek für die Onboard-IMU

constexpr uint32_t kMonitorBaud = 9600;
constexpr uint32_t kNextionBaud = 115200; 

// ====================================================================
// --- 1. GLOBALE KONSTANTEN ---
// ====================================================================
const int16_t MAX_WIDTH = 800; 
const int16_t MAX_HEIGHT = 480;

const float WALL_LEFT = 33.0;
const float WALL_RIGHT = 769.0;
const float WALL_TOP = 57.0;
const float WALL_BOTTOM = 456.0;

// --- KALIBRIERUNG (Nur noch diese beiden Werte sind fest) ---
const float GRAVITY_SCALE = 6500.0; // Wie stark sich 1G Neigung auf die Pixel-Beschleunigung auswirkt
const float BOUNCE_FACTOR = 0.48;   // Energie, die nach dem Abprallen an der Wand übrig bleibt (75%)

// ====================================================================
// --- 2. HILFSFUNKTIONEN & STRUKTUREN ---
// ====================================================================
void sendNextionCommand(const char *command) {
    Serial1.print(command);
    Serial1.write(0xFF);
    Serial1.write(0xFF);
    Serial1.write(0xFF);
}

void clearNextionGraphics() {
    sendNextionCommand("ref 0"); 
}

struct PointData {
    int16_t x;
    int16_t y;
    uint32_t t;
};

// ====================================================================
// --- 3. DIE HYBRIDE PHYSIK-ENGINE (LIVE REIBUNG + IMU KURVEN) ---
// ====================================================================
void predictFullTrajectory(PointData points[20]) {
    char cmd[64];

    // ---------------------------------------------------------
    // SCHRITT A: Live-Reibung aus den 20 Punkten berechnen
    // ---------------------------------------------------------
    float v[4];
    float t_mid[4];

    // Wir teilen die 20 Punkte in 4 Blöcke und messen die Geschwindigkeit
    for (int i = 0; i < 4; i++) {
        int idx1 = i * 5;
        int idx2 = (i == 3) ? 19 : idx1 + 5;
        
        float stepX = points[idx2].x - points[idx1].x;
        float stepY = points[idx2].y - points[idx1].y;
        float dt_step = (points[idx2].t - points[idx1].t) / 1000.0;
        if (dt_step < 0.001) dt_step = 0.001; 
        
        float stepDist = sqrt(stepX*stepX + stepY*stepY);
        v[i] = stepDist / dt_step;
        t_mid[i] = (points[idx1].t + points[idx2].t) / 2000.0; 
    }

    // Geschwindigkeiten mitteln (Erste Hälfte vs. Zweite Hälfte)
    float v_start = (v[0] + v[1]) / 2.0;
    float v_end = (v[2] + v[3]) / 2.0;
    float t_diff = ((t_mid[2] + t_mid[3]) / 2.0) - ((t_mid[0] + t_mid[1]) / 2.0);

    // Negative Beschleunigung (Reibung) berechnen
    float acceleration = (v_end - v_start) / (t_diff > 0.001 ? t_diff : 0.001); 
    float live_friction = -acceleration; 
    
    // Fallback: Wenn die Reibung durch Rauschen unlogisch ist, nehmen wir einen Standardwert
    if (live_friction < 20.0) live_friction = 100.0; 

    Serial.print("Live gemessene Reibung: "); Serial.println(live_friction);


    // ---------------------------------------------------------
    // SCHRITT B: Start-Vektor für die Simulation festlegen
    // ---------------------------------------------------------
    // Wir nutzen das letzte Viertel (Punkte 15-19) für den exakten "Abwurfwinkel"
    float dt_end = (points[19].t - points[15].t) / 1000.0;
    if (dt_end < 0.001) dt_end = 0.001;
    
    float vx = (points[19].x - points[15].x) / dt_end;
    float vy = (points[19].y - points[15].y) / dt_end;


    // ---------------------------------------------------------
    // SCHRITT C: IMU Schwerkraft auslesen
    // ---------------------------------------------------------
    float ax_g = 0, ay_g = 0, az_g = 0;
    if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(ax_g, ay_g, az_g);
    }
    
    // ACHTUNG: Hier ggf. die Vorzeichen anpassen, je nachdem wie der Arduino aufgeklebt ist!
    float gx = -ay_g * GRAVITY_SCALE; 
    float gy = -ax_g * GRAVITY_SCALE; 
    
    Serial.print("Gravitation X: "); Serial.print(gx);
    Serial.print(" | Y: "); Serial.println(gy);


    // ---------------------------------------------------------
    // SCHRITT D: Euler-Physik-Simulation (Kurven & Bounces)
    // ---------------------------------------------------------
    float simX = points[19].x;
    float simY = points[19].y;
    float lastDrawX = simX;
    float lastDrawY = simY;

    float dt = 0.02;         // 20ms pro Simulationsschritt
    int max_steps = 400;     // Maximal 8 Sekunden in die Zukunft

    for (int step = 0; step < max_steps; step++) {
        
        // 1. Schwerkraft anwenden
        vx += gx * dt;
        vy += gy * dt;
        
        // 2. Live-Reibung abziehen (wirkt exakt entgegen der Flugrichtung)
        float current_speed = sqrt(vx*vx + vy*vy);
        float speed_drop = live_friction * dt;
        
        if (current_speed > 0) {
            if (current_speed < speed_drop) {
                vx = 0; vy = 0; current_speed = 0; // Kugel hält an
            } else {
                float multiplier = (current_speed - speed_drop) / current_speed;
                vx *= multiplier;
                vy *= multiplier;
            }
        }
        
        // 3. Überprüfen: Steht die Kugel still und die Schwerkraft ist zu schwach zum Losrollen?
        float gravity_magnitude = sqrt(gx*gx + gy*gy);
        if (current_speed == 0 && gravity_magnitude <= live_friction) {
            break; // Simulation beendet!
        }
        
        // 4. Position updaten
        simX += vx * dt;
        simY += vy * dt;
        
        // 5. Banden-Kollision (Bounce)
        if (simX <= WALL_LEFT)   { simX = WALL_LEFT;   vx = -vx * BOUNCE_FACTOR; }
        if (simX >= WALL_RIGHT)  { simX = WALL_RIGHT;  vx = -vx * BOUNCE_FACTOR; }
        if (simY <= WALL_TOP)    { simY = WALL_TOP;    vy = -vy * BOUNCE_FACTOR; }
        if (simY >= WALL_BOTTOM) { simY = WALL_BOTTOM; vy = -vy * BOUNCE_FACTOR; }
        
        // 6. Zeichnen (Jeden 3. Schritt, damit der Buffer nicht überläuft)
        if (step % 3 == 0) {
            sprintf(cmd, "line %d,%d,%d,%d,33840", (int)lastDrawX, (int)lastDrawY, (int)simX, (int)simY);
            sendNextionCommand(cmd);
            lastDrawX = simX;
            lastDrawY = simY;
        }
    }

    // Lücke schließen und Zielfadenkreuz zeichnen
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
// --- 4. ARDUINO SETUP ---
// ====================================================================
void setup() {
    Serial.begin(kMonitorBaud);
    
    if (!IMU.begin()) {
        Serial.println("Fehler: IMU Sensor nicht gefunden!");
        while (1);
    }

    Serial1.begin(9600);
    delay(500);
    sendNextionCommand("baud=115200"); 
    delay(100);
    Serial1.begin(kNextionBaud);
    delay(1000);

    Serial.println("System Ready! Drücke 'p' im Serial Monitor, um die Aufnahme zu starten.");
    clearNextionGraphics();
}

// ====================================================================
// --- 5. MAIN LOOP ---
// ====================================================================
void loop() {
    static uint8_t state = 0;
    static uint8_t data[7]; 
    static uint8_t index = 0;

    static PointData history[20];
    static uint8_t histCount = 0;
    static bool isRecording = false;       

    static uint32_t lastMovementTime = 0;
    static int16_t currentRealX = -1;
    static int16_t currentRealY = -1;
    static bool screenHasGraphics = false; 

    // --- MANUELLE PRÄDIKTION ÜBER DEN SERIAL MONITOR ('p') ---
    if (Serial.available() > 0) {
        char incomingChar = Serial.read();
        if (incomingChar == 'p' || incomingChar == 'P') {
            if (screenHasGraphics) {
                clearNextionGraphics();
                screenHasGraphics = false;
            }
            isRecording = true;
            histCount = 0;
            lastMovementTime = millis();
            Serial.println(">>> AUFNAHME SCHARF! Bewege die Kugel... <<<");
        }
    }

    if (screenHasGraphics && (millis() - lastMovementTime > 5000)) {
        clearNextionGraphics();
        screenHasGraphics = false; 
        isRecording = false; 
        histCount = 0;
        Serial.println("Screen geputzt (5 Sekunden Inaktivität).");
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

            if (currentRealX == -1 || abs(x - currentRealX) > 2 || abs(y - currentRealY) > 2) {
                lastMovementTime = millis(); 
                currentRealX = x;
                currentRealY = y;
                
                if (isRecording && histCount < 20) {
                    
                    // Banden-Schutz: Wenn die Kugel an der Wand klebt, stoppen wir die Aufnahme
                    if (x <= WALL_LEFT + 8 || x >= WALL_RIGHT - 8 || 
                        y <= WALL_TOP + 8 || y >= WALL_BOTTOM - 8) {
                        
                        if (histCount > 0) {
                            Serial.println("Wand-Kollision detektiert! Verwerfe Daten...");
                            histCount = 0; 
                        }
                    } else {
                        history[histCount].x = x;
                        history[histCount].y = y;
                        history[histCount].t = millis();
                        
                        if (histCount > 0) {
                            char cmd[64];
                            sprintf(cmd, "line %d,%d,%d,%d,65535", 
                                    history[histCount-1].x, history[histCount-1].y, x, y);
                            sendNextionCommand(cmd);
                            screenHasGraphics = true;
                        }
                        
                        histCount++;

                        if (histCount == 20) {
                            isRecording = false; 
                            Serial.println("20 saubere Punkte erfasst. Starte IMU Physik-Simulation...");
                            predictFullTrajectory(history);
                            screenHasGraphics = true;
                        }
                    }
                }
            }
        }
        state = 0;
        index = 0;
    }
}