<!--
author:   Nikolas Laaser und Hans Grundig

email:    Hans-Jakob.Grundig@student.tu-freiberg.de

version:  0.0.1

language: de

narrator: Deutsch Female

comment:  Aufgabenblatt für das RemotLab Wabball

link:     https://github.com/HansGrundig/RemoteLab_Wabball

script:   
-->




# RemoteLab_Wobball
> Herzlich Willkommen zu dem RemoteLab Wabball. 
Einleitung

Stell dir einen smarten Billardtisch vor, der die Bewegung der Kugel präzise verfolgt und sogar die zukünftige Position vorhersagt. Genau das bildet dieses RemoteLab nach: Schritt für Schritt lernst du, wie ein solches System Punkte visualisiert, die Kugelposition ausliest, die Spur aufzeichnet, Geschwindigkeit und Richtung bestimmt und schließlich eine Vorhersage trifft.

In diesem Versuch steuerst du eine Kugel auf einer Arduino-gesteuerten beweglichen Plattform. Die Kugel liegt auf einem Display und ihre Position können über die bereitgestellte API ausgelesen werden.

Aufgabenüberblick:
1. Punkte auf dem Display plotten
2. Auslesen der Position der Kugel
3. Bewegungspfad der Kugel darstellen
4. Geschwindigkeit und Bewegungsrichtung der Kugel bestimmen
5. Die Bewegungstrajektorie der Kugel vorhersagen 

> [!Hinweis]
> Die einzelnen Aufgaben sind mit steigender Schwierigkeit aufgebaut und bauen aufeinander auf.

## Technische Umsetzung:
- Display: Nextion 5.0" TFT Touch Display:
  - Auflösung: 800x480 Pixel
  - Schnittstelle: UART (Serial)
  - weitere Informationen: [Nextion Display Datasheet](https://nextion.tech/datasheets/NX8048P050-011R/)
- Arduino Uno Wifi
[Arduino Uno Pinout](.img\Pinout-UNOwifirev2_latest-1.png)
---

## Aufgabe 1: Punkte auf dem Displays plotten

1.1: Zeichne einen blauen Punkt an der Position **(400, 240)** auf dem Display. Nutze dafür das die Nextionbefehle (siehe Nextion-Dokumentation).
    [[?]]Über die Funktion void sendNextionCommand(const char *command) kannst du Nextion-Befehle an das Display senden.

Im smarten Billardtisch entspricht das dem ersten Testbild: Der Tisch markiert eine Referenzposition auf dem Spielfeld.


**Ziel:** Verstehen, wie Punkte auf dem Display dargestellt werden.


**Hinweise:**
- Übr die Funktion void sendNextionCommand(const char *command) kannst du Nextion-Befehle an das Display senden.

```cpp
void sendNextionCommand(const char *command) {
    Serial1.print(command);
    Serial1.write(0xFF);
    Serial1.write(0xFF);
    Serial1.write(0xFF);
}
```
(*siehe Nextion-Dokumentation*)

**Musterlösung:**

```cpp 
char cmd[64];
sprintf(cmd, "cirs %d,%d,2,31", 400 , 240);
sendNextionCommand(cmd);

```

## Aufgabe 2: Kugelkoordinaten ausgeben

Lass dir die aktuellen Koordinaten der Kugel ausgeben.

**Hinweis:** Mit der Methode `jiggle()` kannst du die Kugel leicht anstoßen, damit sich ihre Position verändert.

**Ausgabe-Beispiel:**

```text
x = 123
y = 87
```

**Ziel:** Die Positionsdaten der Kugel auslesen und anzeigen.

Hier liest der smarte Billardtisch erstmals aktiv die Lage der Kugel aus, so als würde er ihre aktuelle Position auf dem Spielfeld kontrollieren.


**Musterlösung:**

```cpp 
char cmd[64];

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

            Serial.print("x = ");
            Serial.println(x);
            Serial.print("y = ");
            Serial.println(y);
        }
}



```



## Aufgabe 3: Bewegung der Kugel visualisieren

Plotte kontinuierlich einen Punkt an der aktuellen Position der Kugel.

### Anforderungen

* Immer dort einen Punkt zeichnen, wo die Kugel das Display berührt.
* Es dürfen maximal **10 Punkte gleichzeitig** sichtbar sein.
* Sobald ein elfter Punkt hinzugefügt wird, soll der **älteste Punkt gelöscht** werden.

### Beispiel

```text
P1 P2 P3 P4 P5 P6 P7 P8 P9 P10
```

Nach dem nächsten Messpunkt:

```text
P2 P3 P4 P5 P6 P7 P8 P9 P10 P11
```

**Ziel:** Die Bewegung der Kugel als Spur darstellen.

Damit zeichnet der smarte Billardtisch die Flugbahn der Kugel wie eine digitale Stoßspur nach.

---

## Aufgabe 4: Geschwindigkeit und Richtung berechnen

Gib über die serielle Schnittstelle (`Serial`) kontinuierlich aus:

* Geschwindigkeit in **Pixel pro Sekunde (px/s)**
* Bewegungsrichtung in **Grad (°)**


### Ausgabe-Beispiel

```text
Geschwindigkeit: 54.2 px/s
Richtung: 137.5°
```

### Hinweise

* Berechne die Geschwindigkeit aus zwei aufeinanderfolgenden Positionsmessungen.
* Die Richtung kann über den Winkel zwischen den beiden Positionen bestimmt werden.

**Ziel:** Bewegungsdaten der Kugel auswerten.

An dieser Stelle lernt der smarte Billardtisch, wie schnell und in welche Richtung sich die Kugel über das Spielfeld bewegt.

```cpp

```

---

## Bonusaufgabe 5: Position vorhersagen

Schreibe eine Funktion

```cpp
predict(x, y, vel, dir)
```

welche aus

* aktueller X-Koordinate
* aktueller Y-Koordinate
* Geschwindigkeit (`vel`)
* Richtung (`dir`)

die erwartete Position der Kugel **eine Sekunde später** berechnet.

### Anforderungen

1. Berechne die vorhergesagte Position.
2. Gib die vorhergesagten Koordinaten aus.
3. Zeichne zur Visualisierung einen Punkt an der vorhergesagten Position.

### Beispiel

```text
Aktuelle Position: (100, 50)
Geschwindigkeit: 20 px/s
Richtung: 0°

Vorhersage nach 1 s:
(120, 50)
```

**Ziel:** Einführung in Bewegungsmodelle und Vorhersage von Positionen.

Zum Schluss wird der smarte Billardtisch vorausschauend und berechnet, wo die Kugel in einem Moment später auf dem Tisch liegen wird.

## Fazit

Am Ende hast du aus einem einfachen Display- und Sensorkonstrukt einen kleinen smarten Billardtisch gemacht, der Punkte setzt, Positionen liest, Bewegungen sichtbar macht und sogar die nächste Kugelposition abschätzt. Die Aufgaben führen damit von der reinen Ausgabe bis zur einfachen Bewegungsprognose in einer zusammenhängenden, praxisnahen Kette.

---

## Abgabe

Für jede Aufgabe:

1. Quellcode dokumentieren.
2. Kurze Beschreibung der Lösung.
3. Screenshot oder Foto der Ausgabe bzw. Visualisierung.