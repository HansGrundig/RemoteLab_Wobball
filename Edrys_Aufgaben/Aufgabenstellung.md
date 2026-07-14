<!--
author:   Nikolas Laaser und Hans Grundig

email:    Hans-Jakob.Grundig@student.tu-freiberg.de

version:  0.0.1

language: de

narrator: Deutsch Female

comment:  Aufgabenblatt für das RemotLab Wabball

repository: https://github.com/HansGrundig/RemoteLab_Wabball

import: https://raw.githubusercontent.com/liaTemplates/AVR8js/main/README.md

icon: https://tu-freiberg.de/sites/default/files/styles/crop_landscape_1300/public/2023-08/Bild2.png
-->

# RemoteLab Wobball

> Herzlich Willkommen zu dem RemoteLab Wobball! 

Stell dir einen smarten Billardtisch vor, der die Bewegung der Kugel präzise verfolgt und sogar die zukünftige Position vorhersagt.  
Genau das bildet dieses RemoteLab nach: Schritt für Schritt lernst du, wie ein solches System Punkte visualisiert, die Kugelposition ausliest, die Spur aufzeichnet, Geschwindigkeit und Richtung bestimmt und schließlich eine Vorhersage trifft.

In diesem Versuch steuerst du eine Kugel auf einer Arduino-gesteuerten beweglichen Plattform. Die Kugel liegt auf einem Display und ihre Position können über die bereitgestellte API ausgelesen werden.

![Smartes Billard ](../.img/SmartBilliard_V2.png  "[^1]")<!--style="display: block; margin-left: auto; margin-right: auto;"-->

[^1]: Mit Google Gemini generiert.

## Aufgabenüberblick
1. Punkte auf dem Display plotten ([Direktlink](#Aufgabe-1:-Punkte-auf-dem-Displays-plotten))
2. Auslesen der Position der Kugel ([Direktlink](#Aufgabe-2:-Kugelkoordinaten-ausgeben))
3. Bewegungspfad der Kugel darstellen ([Direktlink](#Aufgabe-3:-Bewegung-der-Kugel-visualisieren))
4. Geschwindigkeit und Bewegungsrichtung der Kugel bestimmen ([Direktlink](#Aufgabe-4:-Geschwindigkeit-und-Richtung-berechnen))
5. Die Bewegungstrajektorie der Kugel vorhersagen ([Direktlink](#Bonusaufgabe-5:-Position-vorhersagen))

> [!Note] 💡 Hinweis:
> Die einzelnen Aufgaben sind mit steigender Schwierigkeit aufgebaut und bauen aufeinander auf. Daher ist es sinnvoll, die Aufgaben in der vorgegebenen Reihenfolge zu bearbeiten
>oder die vorherige Musterlösungen zu übernehmen.

<lia-keep>
    <div class="sketchfab-embed-wrapper" style="display: flex; flex-direction: column; align-items: center; margin: 20px auto; width: 100%; max-width: 800px;"> 
        <iframe title="Wobball 3D Modell" style="width: 100%; height: 500px; border: none; border-radius: 8px;" allowfullscreen mozallowfullscreen="true" webkitallowfullscreen="true" allow="autoplay; fullscreen; xr-spatial-tracking" xr-spatial-tracking execution-while-out-of-viewport execution-while-not-rendered web-share src="https://sketchfab.com/models/eefa3e5be18f49cb83b119f35c004882/embed?autospin=1"> </iframe> 
        <p style="font-size: 13px; font-weight: normal; margin: 10px 0 5px 0; color: #4A4A4A; text-align: center;"> 
            <a href="https://sketchfab.com/3d-models/wobball-3d-modell-eefa3e5be18f49cb83b119f35c004882?utm_medium=embed&utm_campaign=share-popup&utm_content=eefa3e5be18f49cb83b119f35c004882" target="_blank" rel="nofollow" style="font-weight: bold; color: #1CAAD9;">Wobball 3D Modell</a> by Hans Grundig is licensed under <a href="https://creativecommons.org/licenses/by/4.0/" target="_blank" rel="nofollow" style="font-weight: bold; color: #1CAAD9;">CC Attribution</a>. 
        </p> 
    </div>
</lia-keep>



## Technische Umsetzung
- Display: Nextion 5.0" TFT Touch Display:
  - Auflösung: 800x480 Pixel
  - Schnittstelle: UART (Serial)
  - weitere Informationen: [Nextion Display Datasheet](https://nextion.tech/datasheets/NX8048P050-011R/)

- Arduino Uno Wifi:
  - Microcontroller: ATmega328P
  - weitere Informationen: [Arduino Uno Wifi Rev2](https://store.arduino.cc/products/arduino-uno-wifi-rev2)

- 3x Feetech FS9225M Digital Servomotor:
  - Drehwinkel: 270°
  - weitere Informationen: [Feetech FS9225M](https://www.premium-modellbau.de/media/pdf/66/1c/1c/FS9225M-specs.pdf)

- IMU: MPU6050 6-Achsen Gyroskop und Beschleunigungssensor:
  - weitere Informationen: [MPU6050](https://www.invensense.com/products/motion-tracking/6-axis/mpu-6050/)

Verbunden sind Display und Arduino über die Pins **RX1** und **TX1**.


<lia-keep>
    <iframe src="https://hansgrundig.github.io/RemoteLab_Wobball/src/Schema.html" width="100%" height="650px" style="border:none; overflow:hidden;"></iframe>
</lia-keep >


## Aufgabe 1: Punkte auf dem Displays plotten
--{{0}}--
In unserem smarten Billardtisch möchten wir zunächst die Funktionaliät des Displays überprüfen.
Daher ist deine Aufgabe einen blauen Punkt in  die Mitte des Displays zu plotten.
Nutze dafür die Nextionbefehle.

**Ziel:** Verstehen, wie Punkte auf dem Display dargestellt werden.

{{1}}

> [!Note] 💡 Hinweis:
> Über die Funktion `sendNextionCommand()` kannst du Nextion-Befehle an das Display senden.
>
>```cpp
>void sendNextionCommand(const char *command) {
>    Serial1.print(command);
>    Serial1.write(0xFF);
>    Serial1.write(0xFF);
>    Serial1.write(0xFF);
>}
>```
>(*siehe [Nextion-Instruction-Set](https://nextion.tech/instruction-set/) )


### **Musterlösung**

```cpp 
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

```
--------------------
--{{3}}--
**Quizfrage:**

Wie groß ist der gesendete Nachricht in der Musterlösung (inkl. Terminations bytes)?

    [[20]]
    [[?]] sprintf wandelt den Befehl in einen String um
    [[?]] Jedes Zeichen in einem String ist 1 byte
    [[?]] Beachte Leerzeichen und Kommas
**************

Die Lösung ist 20, weil:

| Textteil      | Zeichen | Anzahl der Bytes |
|:------------- | -------:| ----------------:|
| Der Befehl    |  cirs_  |                5 |
| X-Koordinate  |   400   |                3 |
| Y-Koordinate  |   240   |                3 |
| Punktgröße    |    2    |                1 |
| 3x Komma      |   ,     |                1 |
| Farbwert      |   31    |                2 |
| 3x Stopp Bytes|  0xFF   |                1 |

Der string `cmd` endet mit 1 byte großem \n, welcher aber nicht mit geschickt wird

**************

## Aufgabe 2: Kugelkoordinaten ausgeben
--{{0}}--
Da wir nun Punkte auf dem Display plotten können, wollen wir nun die Position der Kugel auslesen.  
Lass dir dafür aktuellen Koordinaten der Kugel auf dem Serial Monitorausgeben.

**Ausgabe-Beispiel:**

```text
x = 123
y = 87
x = ...
y = ...
```

**Ziel:** Die Positionsdaten der Kugel auslesen und anzeigen.


> [!Note] 💡 Hinweis:
> Mit der Methode `jiggle()` kannst du die Kugel leicht anstoßen, damit sich ihre Position verändert.

### Exkurs: Nextion Resistiver Touchscreen
Der Nextion NX8048P050-011R Touchscreen ist ein resistiver Touchscreen, der auf Druck reagiert.
Dabei wird durch Druck auf die Oberfläche ein elektrischer Kontakt von zwei Elektroden hergestellt, der die Position des Drucks ermittelt.

![Funktionsweise](../.img/resistive_touchscreens.png)

weitere Informationen: [Resistiver Touchscreen](https://www.leifiphysik.de/elektrizitaetslehre/komplexere-schaltkreise/ausblick/resistiver-touchscreen)


### **Musterlösung**

```cpp 

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


```

## Aufgabe 3: Bewegung der Kugel visualisieren

Damit zeichnet der smarte Billardtisch die Trajektorie der Kugel wie eine digitale Stoßspur nach.
Plotte kontinuierlich den zurückgelegten Pfad der Kugel.

>[!IMPORTANT] ❗ Anforderungorderungen:
>* Immer dort einen Punkt zeichnen, wo die Kugel das Display berührt.
>* Es dürfen maximal **10 Punkte gleichzeitig** sichtbar sein.
>* Sobald ein elfter Punkt hinzugefügt wird, soll der **älteste Punkt gelöscht** werden.

**Ziel:** Die Bewegung der Kugel als Spur darstellen.

### Musterlösung

```cpp
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
```

## Aufgabe 4: Geschwindigkeit und Richtung berechnen

Gib über die serielle Schnittstelle (`Serial`) kontinuierlich aus:

* Geschwindigkeit in **Pixel pro Sekunde (px/s)**
* Bewegungsrichtung in **Grad (°)**


**Ausgabe-Beispiel**

```text
Geschwindigkeit: 54.2 px/s
Richtung: 137.5°
```

> [!Note] 💡 Hinweis:   
>* Berechne die Geschwindigkeit aus zwei aufeinanderfolgenden Positionsmessungen.
>* Die Richtung kann über den Winkel zwischen den beiden Positionen bestimmt werden.

**Ziel:** Bewegungsdaten der Kugel auswerten.

### Musterlösung
```cpp
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

const float WALL_LEFT = 33.0;
const float WALL_RIGHT = 769.0;
const float WALL_TOP = 57.0;
const float WALL_BOTTOM = 456.0;

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

    // Servos initialisieren
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

    static uint32_t lastPointTime = 0; 
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
            
            // Puffer leeren
            for(int i = 0; i < 10; i++) {
                pathX[i] = -1; 
                pathY[i] = -1;
            }

            Serial.println("-> Fuehre Jiggle aus...");
            jiggle(); 
            Serial.println("-> Jiggle beendet.");
            
            // Empfangspuffer flushen
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

            // --- DELTA-ZEIT BERECHNUNG ---
            uint32_t currentTime = millis();
            uint32_t dt = currentTime - lastPointTime; // 'dt' ist die exakte Millisekunden zwischen Punkten
            lastPointTime = currentTime; // Zeit für den NÄCHSTEN Punkt speichern

            // Sicherheitscheck: Vermeidet Division durch Null, falls Pakete sofort eintreffen
            if (dt == 0) dt = 1;
            
            // Nur aktualisieren, wenn sich die Berührungsposition geändert hat
            if (pathX[9] == -1 || abs(x - pathX[9]) > 2 || abs(y - pathY[9]) > 2) { 
                
                char cmd[64];

                // --- SCHRITT A: ALTE GRAFIK ENTFERNEN ---
                // 1. Ältesten grünen Punkt löschen (falls Puffer voll)
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

                // --- SCHRITT D: BERECHNUNGEN ---
                // Warten, bis mindestens 5 Punkte im Puffer sind
                if (pathX[5] != -1) {
                    
                    // 1. Gesamtdifferenz über die letzten 5 Punkte berechnen
                    int16_t dx = pathX[9] - pathX[5];
                    int16_t dy = pathY[9] - pathY[5];

                    // 2. Exakte Zeitspanne über dieses 5-Punkte-Fenster
                    uint32_t totalDt = pathTime[9] - pathTime[5];
                    if (totalDt == 0) totalDt = 1; // Vermeidet Division durch Null

                    // 3. Geschwindigkeit berechnen
                    // Pythagoras zur Berechnung der zurückgelegten Pixel-Distanz
                    float distance = sqrt((dx * dx) + (dy * dy));

                    // Geschwindigkeit in Pixel pro Sekunde (ms -> s: *1000)
                    int16_t velocityPPS = (distance * 1000.0) / totalDt;

                    Serial.print("Velocity in PPS=");
                    Serial.println(velocityPPS);

                    // 4. Richtung in Grad berechnen
                    float angleRads = atan2(dx, -dy);
                    float direction = angleRads * 180.0 / PI;

                    // Normalisieren auf 0-360
                    if (direction < 0) {
                        direction += 360.0;
                    }
                    Serial.print("Direction in Degree: ");
                    Serial.println(direction,3);
                    
                    // Letzte Punkt-Koordinaten aktualisieren
                    lastpathX = pathX[9]; 
                    lastpathY = pathY[9]; 
                }
            }

            
        }
        state = 0;
        index = 0;
    }
}
```

---

## Bonusaufgabe 5: Position vorhersagen

Schreibe eine Funktion, die unter Verwendung der aktuellen Position, Geschwindigkeit und Richtung die **stillstands Position** der Kugel berechnet. 

>[!IMPORTANT] ❗ Anforderungorderungen:
>1. Berechne die vorhergesagte Position.
>2. Gib die vorhergesagten Koordinaten aus.
>3. Zeichne zur Visualisierung ein Kreuz an der vorhergesagten Position.


**Ziel:** Einführung in Bewegungsmodelle und Vorhersage von Positionen.

### Musterlösung

```cpp
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
const float WALL_LEFT = 33.0;
const float WALL_RIGHT = 769.0;
const float WALL_TOP = 57.0;
const float WALL_BOTTOM = 456.0;

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
```

## Fazit

Am Ende hast du aus einem einfachen Display- und Sensorkonstrukt einen kleinen smarten Billardtisch gemacht, der Punkte setzt, Positionen liest, Bewegungen sichtbar macht und sogar die nächste Kugelposition abschätzt. Die Aufgaben führen damit von der reinen Ausgabe bis zur einfachen Bewegungsprognose in einer zusammenhängenden, praxisnahen Kette.

---

## Feedback

Wie hat dir unsere RemoteLab gefallen?

    [(1)] ⭐
    [(2)] ⭐⭐
    [(3)] ⭐⭐⭐
    [(4)] ⭐⭐⭐⭐
    [(5)] ⭐⭐⭐⭐⭐

Was können wir deiner Meinung nach Verbessern?

    [[___]]

Vielen Danke für deine Teilnahme 💟


