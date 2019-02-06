# Percy

Percy ist ein Leitsystem für blinde Menschen. Es besteht aus einer modischen Brille und am Körper aufklebbaren Vibrationspads. Die Brille nimmt die Umgebung wahr und erkennt Hindernisse. Die Pads führen den Nutzer mithilfe bestimmter Vibrationsimpulse sicher um diese herum.



## Bauteile

### Bei der Abgabe vorhandene Hardware
* microSD-Karte für den Raspberry Pi
* Kurzes USB-Kabel 
* Hemd mit Vibrationsmotoren und verbautem Microcontroller (ESP32)
* Powerbank (Batterie) ohne Batterien

### Zur Inbetriebnahme des Prototyps wird zusätzlich benötigt
* Raspberry Pi 3 Model B+ und Netzteil
* Tastatur für den Raspberry Pi - Am besten Drahtlos (Billige Logitech Tastaturen mit USB Dongle funktionieren gut)
* Router (fritz.box)
* 4xAA Batterien (Besser mehr, die gehen schnell leer)

#### Optional
* Maus
* Monitor
* Pi Camera


## Hardware Setup
Falls ein neuer Router verwendet wird ist das Setup etwas komplizierter, weil das W-LAN erst konfiguriert werden muss (Siehe Abschnitt W-LAN Setup)

### ESP32 (Hemd + Brille)
1. Der ESP ist in der schwarzen Box mit den vielen Kabeln versteckt. Insgesamt gibt es drei Paare an Kabeln, die durch Steckverbindungen mit den an Hemd und Brille befestigten Motoren verbunden werden können.

2. Die Powerbank an der schwarzen Box muss mit 4xAA Batterien gefüllt werden, um den ESP mit Power zu versorgen.

3. Sie hat einen kleinen an- und ausschalter neben dem USB Anschluss.

4. Die Powerbank muss mit dem USB Kabel an den Raspberry angeschlossen werden.

5. Sicherstellen, dass die Powerbank eingeschaltet ist, der ESP muss laufen, damit der Raspberry ihn erkennt.

### Raspberry Pi (Tastatur / Code Demos)
1. Tastatur (und eventuell Maus + Bildschirm) and den Raspberry anschließen.

2. Sicherstellen, dass die SD Karte eingesteckt ist

3. Raspberry Pi starten

4. Terminal öffnen. Zu finden oben in der Task-Leiste

5. Im Terminal eingeben `source start.sh`

6. Falls ein Fehler auftritt versuch es nochmal mit `python percyController.py 42`

7. So lange das Terminal Fenster geöffnet ist können bestimmte Vibrationsmuster mit W A S D aktiviert werden. Shift + W A S D steuert die Motoren direkt an.

## W-LAN Setup
Voraussetzungen:
* Arduino IDE mit ESP32 Einstellungen installiert
* W-LAN Name und Passwort sind bekannt
* Funktioniert nicht bei WPA-Enterprise W-LAN Netzwerken (eduroam und h-da zählen dazu)
* Für eine statische IP muss bekannt sein, welche IP Adressen vom Router vergeben werden. (Am einfachsten ist es, wenn man mit verbundenem PC seine eigene IP abfragt). Bei der FritzBox kann es etwas wie `192.168.178.100` sein. Die Zahlen hinter dem letzten Punkt sind nicht wichtig, es geht um alles davor `192.168.178`

### ESP32
1. ESP32 mit einem USB Kabel an den PC anschließen

2. Die Datei `Percy-ESP-Client/Percy-ESP-Client.ino` in der Arduino IDE öffnen

3. In den ersten Zeilen die Variablen `ssid` und `password` ändern. Darauf achten, dass die Anführungszeichen da bleiben.

4. In den Zeilen `local_IP` und `gateway` entsprechend die IP eingeben, die der ESP32 bekommen soll und die IP des Routers. Meistens unterscheiden sich hier nur die Zahlen hinter dem letzten Punkt. Eventuell noch die Zeile darunter `subnet`, wobei das selten notwendig sein sollte.

5. Sketch auf den ESP32 hochladen

### Raspberry Pi
1. Bei angeschaltetem Raspberry auf das W-Lan Symbol in der Taskleiste oben rechts klicken und Daten eingeben

2. Den Datei-Exporer in der Taskleiste oben links öffnen

3. Die Datei `percy/percyController.py` öffnen und die Zeile mit der IP Adresse bearbeiten. `192.168.178.`

## Mögliche Fehler

Falls im Terminal kein `(env)` vor der Eingabe steht muss im Ordner `percy` der Befehl `source env/bin/activate` ausgeführt werden

Falls der Fehler "No route to host" erscheint, oder die Motoren sich aufhängen kann es sein, dass die Batterien in der Powerbank nicht mehr genügend Energie haben.
