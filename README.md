<!--
author:   Nikolas Laaser und Hans Grundig

email:    Hans-Jakob.Grundig@student.tu-freiberg.de

version:  0.0.1

language: de

narrator: Deutsch Female

comment:  Aufgabenblatt für das RemotLab Wabball

repository:     https://github.com/HansGrundig/RemoteLab_Wabball

import: https://raw.githubusercontent.com/liaTemplates/AVR8js/main/README.md

-->

# RemoteLab Wobball
> Herzlich Willkommen zu dem RemoteLab Wabball! 

Stell dir einen smarten Billardtisch vor, der die Bewegung der Kugel präzise verfolgt und sogar die zukünftige Position vorhersagt.  
Genau das bildet dieses RemoteLab nach: Schritt für Schritt lernst du, wie ein solches System Punkte visualisiert, die Kugelposition ausliest, die Spur aufzeichnet, Geschwindigkeit und Richtung bestimmt und schließlich eine Vorhersage trifft.

In diesem Versuch steuerst du eine Kugel auf einer Arduino-gesteuerten beweglichen Plattform. Die Kugel liegt auf einem Display und ihre Position können über die bereitgestellte API ausgelesen werden.

![Smartes Billard](./.img/SmartBilliard_V2.png) 

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
    <div class="sketchfab-embed-wrapper"> <iframe title="Assembly 2" frameborder="0" allowfullscreen mozallowfullscreen="true" webkitallowfullscreen="true" allow="autoplay; fullscreen; xr-spatial-tracking" xr-spatial-tracking execution-while-out-of-viewport execution-while-not-rendered web-share src="https://sketchfab.com/models/eefa3e5be18f49cb83b119f35c004882/embed?autospin=1"> </iframe> <p style="font-size: 13px; font-weight: normal; margin: 5px; color: #4A4A4A;"> <a href="https://sketchfab.com/3d-models/assembly-2-eefa3e5be18f49cb83b119f35c004882?utm_medium=embed&utm_campaign=share-popup&utm_content=eefa3e5be18f49cb83b119f35c004882" target="_blank" rel="nofollow" style="font-weight: bold; color: #1CAAD9;"> Assembly 2 </a> by <a href="https://sketchfab.com/HansGru?utm_medium=embed&utm_campaign=share-popup&utm_content=eefa3e5be18f49cb83b119f35c004882" target="_blank" rel="nofollow" style="font-weight: bold; color: #1CAAD9;"> HansGru </a> on <a href="https://sketchfab.com?utm_medium=embed&utm_campaign=share-popup&utm_content=eefa3e5be18f49cb83b119f35c004882" target="_blank" rel="nofollow" style="font-weight: bold; color: #1CAAD9;">Sketchfab</a></p></div>
</lia-keep>



## Technische Umsetzung
- Display: Nextion 5.0" TFT Touch Display:
  - Auflösung: 800x480 Pixel
  - Schnittstelle: UART (Serial)
  - weitere Informationen: [Nextion Display Datasheet](https://nextion.tech/datasheets/NX8048P050-011R/)

- Arduino Uno Wifi:
  - Microcontroller: ATmega328P
  - weitere Informationen: [Arduino Uno Wifi Rev2](https://store.arduino.cc/products/arduino-uno-wifi-rev2)

Verbunden sind Display und Arduino über die Pins **RX1** und **TX1**.


<lia-keep>
    <iframe src="https://hansgrundig.github.io/RemoteLab_Wabball/src/Schema.html" width="100%" height="650px" style="border:none; overflow:hidden;"></iframe>
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

char cmd[64];
sprintf(cmd, "cirs %d,%d,2,31", 400 , 240);     // 400,240 = Mitte des Displays
sendNextionCommand(cmd);

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

![Funktionsweise](./.img/resistive_touchscreens.png)

weitere Informationen: [Resistiver Touchscreen](https://www.leifiphysik.de/elektrizitaetslehre/komplexere-schaltkreise/ausblick/resistiver-touchscreen)


### **Musterlösung**

```cpp 

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

        Serial.print("x = ");
        Serial.println(x);
        Serial.print("y = ");
        Serial.println(y);
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

An dieser Stelle lernt der smarte Billardtisch, wie schnell und in welche Richtung sich die Kugel über das Spielfeld bewegt. Gib die Ergebnisse dazu über die serielle Schnittstelle (`Serial`) kontinuierlich aus:

* Geschwindigkeit in **Pixel pro Sekunde (px/s)**
* Bewegungsrichtung in **Grad (°)**

### Musterlösung
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

>[!IMPORTANT] ❗ Anforderungorderungen:
>1. Berechne die vorhergesagte Position.
>2. Gib die vorhergesagten Koordinaten aus.
>3. Zeichne zur Visualisierung einen Punkt an der vorhergesagten Position.

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


