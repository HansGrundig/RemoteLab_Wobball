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
kommt noch
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

kommt nocht

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

### Musterlösung
```cpp
kommt noch
```

---

## Bonusaufgabe 5: Position vorhersagen

Schreibe eine Funktion, die unter Verwendung der aktuellen Position, Geschwindigkeit und Richtung die **stillstands Position** der Kugel berechnet. 

```cpp
kommt  noch
```

>[!IMPORTANT] ❗ Anforderungorderungen:
>1. Berechne die vorhergesagte Position.
>2. Gib die vorhergesagten Koordinaten aus.
>3. Zeichne zur Visualisierung einen Punkt an der vorhergesagten Position.

**Beispiel**

```text
Aktuelle Position: (100, 50)
Geschwindigkeit: 20 px/s
Richtung: 0°

```

**Ziel:** Einführung in Bewegungsmodelle und Vorhersage von Positionen.

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


