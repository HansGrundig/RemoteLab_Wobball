# Herzlich Willkommen zum RemoteLab Wobball 🎱

*Ein Beitrag für die RemoteLab Challenge 2026*

**Quick Links:**
- 🖥️ [Zum Live RemoteLab (Edrys)](https://edrys-labs.github.io/?/classroom/HrUaacqz6fkaefx5/UjuQnhZ1Ngb4M45E)
- 📝 [Interaktives Aufgabenblatt](https://liascript.github.io/course/?https://raw.githubusercontent.com/HansGrundig/RemoteLab_Wobball/master/Edrys_Aufgaben/Aufgabenstellung.md#1)

---

## 📖 Einführung
Stell dir einen smarten Billardtisch vor, der die Bewegung der Kugel präzise verfolgt und sogar die zukünftige Position vorhersagt! Diese geniale Idee realisiert du in diesem RemoteLab.

Im Rahmen der Veranstaltung *"Softwareentwicklung für eingebettete Systeme"* ist dieses RemoteLab entstanden. In fünf aufeinander aufbauenden Aufgaben werden die Grundlagen zur Kommunikation mit einem Nextion TFT Display, die Servosteuerung (inkl. inverser Kinematik) und das Auslesen von Sensorik (IMU) vermittelt. Während die Aufgaben 1 bis 4 schrittweise die technischen Grundlagen aufbauen, wird in Aufgabe 5 alle Funktionen kombiniert: Hier werden Bewegungsmodelle genutzt, um die Trajektorie und die zukünftige Position einer rollenden Kugel auf der Plattform vorherzusagen.
Die Aufgaben sind so konzipiert, dass sie sowohl für Studierende der Informatik als auch für Schüler ab der 10. Klasse interessant sind. Die Aufgaben können vollständig remote bearbeitet werden und sind nach Schwierzgrad gestaffelt, sodass sie sowohl für Anfänger als auch für Fortgeschrittene geeignet sind.

## 🛠️ Technische Umsetzung & Hardware
Das Projekt ermöglicht echte Hardware Interaktion über die Edrys-Plattform. Folgende  Hardwarekomponenten werden von den Studierenden ferngesteuert:
- **Mikrocontroller:** Arduino Uno WiFi Rev2 (ATmega328P)
- **Display:** Nextion 5.0" TFT Touch Display (800x480 Pixel, UART-Anbindung, Resistiver Touchscreen für Positionserkennung)
- **Sensorik:** MPU6050 (IMU) für präzise Lagedaten (I2C); 
- **Aktorik:** 3x Servomotoren zur Steuerung der Wobball-Plattform

## 🎯 Lernziele
- Verständnis für die Kommunikation mit einem Nextion TFT Display
- Grundlagen der Servosteuerung und inverse Kinematik
- Auslesen von Sensorik (IMU) und Verarbeitung der Daten

---

## 🌟 Usability & Accessibility Features
Da das Labor vollständig remote bedient wird, wurde höchster Wert auf Zugänglichkeit (Accessibility) und eine intuitive Benutzerführung (Usability) gelegt.

* **Interaktives Lernmaterial via LiaScript:**
  * 🌍 **Mehrsprachigkeit & Übersetzung:** Das Aufgabenblatt ist nativ übersetzbar, wodurch internationale Studierende problemlos teilnehmen können.
  * 🗣️ **Text-to-Speech (Vorlesefunktion):** Das Lernmaterial unterstützt Screenreader und bietet einen integrierten "Narrator" (Deutsch Female), was sehbehinderten Nutzern und auditiven Lerntypen stark entgegenkommt.
  * 🧠 **Interaktive Quizzes:** Eingebettete Wissensabfragen (z.B. zur Byte-Größe der UART-Nachrichten) geben den Studierenden direktes Feedback zu ihrem Lernfortschritt.
* **Visuelle & Räumliche Assistenz:**
  * ⚡ **Interaktive Schaltpläne:** Ein eigens entwickeltes, farbcodiertes SVG-Schaltdiagramm (`Schema.html`) zeigt Tooltips an, sobald man mit der Maus über Kabel oder Pins fährt. Das verhindert Verdrahtungs-Frust.
  * 🎲 **Integriertes 3D-Modell:** Über die Sketchfab-Integration im Aufgabenblatt können Studierende die Plattform aus jedem Winkel betrachten, um die räumliche Mechanik (inverse Kinematik) der Servos besser  zubegreifen, ohne physisch im Labor zu stehen.
* **Plug & Play Code-Struktur:** Die Bereitstellung modularer Code-Snippets und leicht verständlicher Vorlagen ermöglicht einen schnellen Start ohne komplizierte Einrichtung.

---

## 🚀 Geplante Funktionen (Roadmap)
Um das RemoteLab in Zukunft noch immersiver zu gestalten, sind folgende Erweiterungen in Arbeit:
- **Synchrones 3D-Modell (Digital Twin):** Das 3D-Modell im Browser soll sich in Echtzeit exakt so neigen, wie die physische Plattform im Labor (gesteuert über die IMU-Daten).
- **Erweiterte UI-Interaktion:** Button-Funktionalitäten auf der Edrys-UI, um die Kugel per Mausklick ferngesteuert anzustoßen ("Jiggle"-Funktion) oder das Display zu kalibrieren.
- **Erweiterte Auswertungs-Graphen:** Live-Plotting der Geschwindigkeits- und Richtungsdaten in der Weboberfläche.
- **Kontinuierliche Prädikten mit Servo Feedback:** Kugel dreht sich im Kreis, Plattform reagiert in Echtzeit, um die Kugel auf einer vorgegebenen Bahn zu halten.
