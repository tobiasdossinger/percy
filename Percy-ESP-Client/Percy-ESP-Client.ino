// Lade die WiFi Library
#include <WiFi.h>

// Nutzername und Passwort vom W-LAN Router / Access Point
// Ja, Plaintext... Boo!
const char * ssid = "LocalsOnly";
const char * password = "Tobias!!";

// Starte den Server auf Port 80
WiFiServer wifiServer(80);

// Hier werden die PINs der Motoren in einem Array gespeichert
// So kann man jeden Motor über den Array Index ansprechen
// Beispiel:
// int motorPINs[] = {12, 14, 27, 26};
// Motor 0 wäre dann PIN 12
int motorPINs[] = {12, 14, 27, 26};

// Hier werden die Muster definiert, etwas umständlich, aber relativ flexibel neue hinzuzufügen
// Ein Muster ist sehr ähnlich, wie eine CSS Animation. Es werden bestimmte "Keyframes", also feste Punkte innerhalb des Musters definiert
// Der erste Wert steht jeweils für die Zeit und der zweite für die Stärke des Motors.
// Beide Werte werden mit einer Kommazahl zwischen 0 (Start / Aus) und 1 (Ende / Volle Power) dargestellt
// So sieht ein Muster beispielsweise aus:
// {
//   {0, 0}       | Am Anfang Ist der Motor aus
//   {0.25, 0.8}  | Bei 25% der Zeit ist der Motor zu 80% an
//   {0.5, 0}     | Bei der Hälfte der Zeit ist der Motor wieder aus
//   {0.75, 1}    | Bei 3/4 der Zeit ist der Motor an, aber dieses Mal zu 100%
//   {1, 0}       | Am Ende, zu 100% der Zeit, ist der Motor dann wieder aus
// }
// Dieses Beispielmuster würde also zwei Impulse mit regelmäßigem Abstand geben, wobei der zweite Impuls stärker, als der erste ist
//
// Falls du Muster hinzufügen willst musst du die drei Zahlen in Klammern bei float patternList[X][X][X] beachten und evtl ändern. Diese stehen für die Anzahl der Elemente im Array
// Die Zahlen bedeuten folgendes: float patternList[<Anzahl der Muster>][<Anzahl der Zwischenschritte des längsten Musters>][<Wird immer 2 sein>]
// Solltest du ein Muster mit wenigen Zwischenschritten hinzufügen musst du wahrscheinlich also nur die erste der drei Zahlen um 1 erhöhen
// Beispielsweise wird dann  float patternList[3][5][2]  zu  float patternList[4][5][2]
// Wenn du jetzt ein Muster hinzufügen willst, das mehr als 5 Zwischenschritte hast musst du entsprechend auch die zweite Zahl ändern
// So wird dann  float patternList[3][5][2]  zu  float patternList[4][15][2]  wenn du ein Muster mit 15 Schritten erstellst
float patternList[3][5][2] = {
  {
    {0, 0},
    {0.5, 0.25},
    {0.75, 1},
    {1, 0}
  },
  {
    {0, 0},
    {0.25, 0.5},
    {0.5, 0},
    {0.75, 1},
    {1, 0}
  },
  {
    {0, 0},
    {0.4, 1},
    {0.6, 0},
    {0.8, 1},
    {1, 0}
  }
};

// Hier wird ein Grundgerüst für die Informationen, die dann mit jedem aktiven Vibrationsmuster verknüpft sind erstellt
struct activePattern {
  int tStart;       // Wann genau hat es angefangen
                    // Beispiel: <4124>ms nachdem der ESP gestartet wurde

  int tDuration;    // Wie lange geht es insgesamt
                    // Beispiel: <2500>ms, also eine halbe Sekunde

  int tEnd;         // Wann soll es aufhören (Wird durch tStart und tDuration errechnet)
                    // Beispiel: <6624>ms nachdem der ESP gestartet wurde

  int motorID;      // Auf welchem Motor das Muster abgespielt werden soll (Der Index aus dem motorPINs Array)
                    // Beispiel: Motor <0>

  int patternID;    // Welches Pattern soll abgespielt werden (Der Index aus dem patternList Array)
                    // Beispiel: Pattern <3>
};

// Die Variable activePatterns ist ein Array aus einzelnen activePattern
// Die Länge des Arrays = Die Anzahl der Motoren, so kann jeder Motor immer nur ein Muster gleichzeitig abspielen
activePattern activePatterns[sizeof(motorPINs) / sizeof(motorPINs[0])];

// Diese Funktion wird später im Loop immer dann aufgerufen, wenn der ESP einen Befehl bekommt ein neues Muster zu starten
// Sie macht nichts anderes, als dem activePatterns Array von der Zeile oben drüber ein Muster (activePattern) mit den entsprechenden Werten hinzuzufügen
// Nehmen wir als Beispiel folgendes: Der ESP bekommt den Befehl über Motor 0 das Muster 3 abzuspielen und zwar 2500ms
// Später im Loop wird dann irgendwo addPattern(0, 3, 2500) aufgerufen
void addPattern(int motorID, int patternID, int duration) {
  // tStart, also wann das Muster gestartet wird wird auf die aktuelle Zeit des ESP (In Millisekunden gesetzt)
  // Nehmen wir das Beispiel von weiter oben mit 4124ms, tStart ist also 4124
  int tStart = millis();

  // Das Muster wird ausgefüllt
  activePattern testPattern = {
    tStart,               // 4124
    duration,             // 2500
    tStart + duration,    // 6624
    motorID,              // 0
    patternID             // 3
  };

  // Das Muster wird dann im Array activePatterns gespeichert. Die Stelle ist hier die ID des Motors
  // Bei motorID = 0 wäre das also ganz vorne im Array. So kann später im loop zugeordnet werden welcher Motor welches Muster abspielen soll
  activePatterns[motorID] = testPattern;
}

// Hier werden einige Variablen global definiert, die von mehreren Funktionen gebraucht werden. Mehr dazu später
int messagePart = 0;
String motorID = "";
String patternID = "";
String motorPower = "";
int tCurrent;


// Ab hier wird es etwas schwerer zu folgen, da die nachfolgenden Funktionen alle im Loop unter bestimmten Bedingungen aufgerufen werden.
// Es werden, also zunächst die einzelnen Funktionen erklärt und dann kommen setup() und loop() in denen die Funktionen benutzt werden.


// Für die Kommunikation mit dem ESP wurde ein eigenes kleines Protokoll geschrieben
// Ein Datenpaket an Percy kann so aussehen: 
// 0-3-2500x
// Die Bindestriche dienen hier als Kennzeichnung für das Ende eines Wertes und das x für das Ende des Befehls.
// Diese Funktion benutzt einige der oben definierten Variablen:
// messagePart -> In welchem Teil des Datenpaketes wir uns gerade befinden:
// Teil 0 - Teil 1 - Teil 2 x
// 0      - 3      - 2500   x
// Die einzelnen Teile stehen für die ID des Motors, die ID des Musters und die Dauer in ms, also die Werte, die die Funktion addPattern() braucht
// Die folgende Funktion geht jedes Zeichen einzeln durch
void socketLoop(char c) {
  // Anhand des Beispiels, das wir die ganze Zeit verwenden ist c also:
  // Runde 1: 0
  // Runde 2: -
  // Runde 3: 3
  // Runde 4: -
  // Runde 5: 2
  // Runde 6: 5
  // Runde 7: 0
  // Runde 8: 0
  // Runde 9: x


  // Das folgende ist ein langer if-else Befehl, es kann also immer nur ein Teil davon ausgeführt werden


  // Falls das aktuelle Zeichen ein - ist
  // Also falls es das Ende eines Wertes ist
  if (c == '-') {
    // Zähle die Variable messagePart hoch, das wird gleich wichtig
    messagePart++;
  
  // Falls das aktuelle Zeichen ein x ist
  // Also falls das Datenpaket zuende ist
  } else if (c == 'x') {
    // Starte das Muster und übergib ihm die Werte, die aus dem Datenpaket gelesen wurden
    // Da es Strings sind müssen sie vorher zu Integern umgewandelt werden
    addPattern(motorID.toInt(), patternID.toInt(), motorPower.toInt());
    // Setze die Variablen wieder zurück
    motorID = "";
    patternID = "";
    motorPower = "";
    messagePart = 0;

  // Falls wir in Teil 0 des Datenpaketes sind
  // Also die ID des Motors
  } else if (messagePart == 0) {
    // Füge der Variable motorID das aktuelle Zeichen hinzu
    motorID = motorID + c;

  // Falls wir in Teil 1 des Datenpaketes sind
  // Also die ID des Vibrationsmusters
  } else if (messagePart == 1) {
    // Füge der Variable patternID das aktuelle Zeichen hinzu
    patternID = patternID + c;

  // Falls wir in Teil 2 des Datenpaketes sind
  // Also die Dauer
  } else if (messagePart == 2) {
    // Füge der Variable motorPower das aktuelle Zeichen hinzu
    motorPower = motorPower + c;
  }

  // Warte kurz
  delay(10);
}

// Diese Funktion kümmert sich darum, dass die Muster ausgeführt werden, sie wird sehr oft ausgeführt (Mehrmals die Sekunde)
// Hier wird anhand der vorgegebenen Werte aus patternList berechnet wie stark welcher Motor gerade vibrieren soll
// Innerhalb der Funktion sind einige Dinge wichtig zu verstehen
// Es gibt für jedes Muster eine Start und Endzeit. Diese ist (Wie oben beschrieben) immer in Millisekunden seit dem Start des ESP angegeben
// .tStart ist hierbei immer wann das Muster angefangen hat
// .tDuration wie lange das Muster insgesamt dauert
// .tEnd dann entsprechend wann das Muster vorbei ist
// Diese Werte werden nur in addPattern() einmal beim Start des Vibrationsmusters gesetzt, bleiben also gleich
// tCurrent wiederum verändert sich jedes mal wenn motorLoop() aufgerufen wird und ist die aktuelle Zeit des ESP
// Aus der Kombination dieser festen Werte für das Muster und der aktuellen Zeit wird dann errechnet, wie stark welcher Motor vibrieren soll
void motorLoop() {
  // Alles in der motorLoop Funktion wird durch diesen For-Loop so oft ausgeführt, wie es Motoren gibt
  // Also praktisch für jeden Motor
  for (int i = 0; i < sizeof(motorPINs) / sizeof(motorPINs[0]); i++) {
    // Der Faulheit halber wird hier die Variable currentPattern definiert
    // Die Variable ist dann immer das aktive Muster für den aktuellen Motor
    activePattern currentPattern = activePatterns[i];

    // Erst einmal überprüfen wir, ob auf dem Motor überhaupt ein Muster abgespielt werden muss
    // tEnd ist ja wann das Muster fertig ist
    // Wenn diese Zeit also größer ist, als die aktuelle Zeit (tCurrent), dann ist das Muster nicht vorbei und der Motor muss etwas machen
    if (tCurrent < currentPattern.tEnd) {
      // Anhand der aktuellen Zeit und der Dauer des Vibrationsmusters wird errechnet, wie viel des Musters bereits fertig ist
      // Beispiel wenn das Muster bei 4124 angefangen hat und bei 6624 aufhört, also 2500ms dauert und tCurrent gerade 5351 ist:
      // float totalProgress = (5351 - 4124) / (2500)
      // totalProgress ist also 0.4908 bzw haben wir gerade 49.08% des Musters durch
      float totalProgress = float(tCurrent - currentPattern.tStart) / float(currentPattern.tDuration);

      // Als nächstes suchen wir uns raus, wo genau wir uns im Muster gerade befinden.
      // Nehmen wir nochmal das Beispiel:
      // {
      //   {0, 0}
      //   {0.25, 0.8}
      //   {0.5, 0}
      //   {0.75, 1}
      //   {1, 0}
      // }
      // Wenn wir bei 49.08% sind wären wir also gerade zwischen {0.25, 0.8} und {0.5, 0}
      // Im folgenden wird einfach ganz stupide jeder einzelne Abschnitt des Musters durchgelaufen und geschaut, ob wir zu weit gegangen sind
      // Beispiel:
      // {0, 0}       -> 0    -> Ist 0 größer, als 0.4908? Nein! Weiter!
      // {0.25, 0.8}  -> 0.25 -> Ist 0.25 größer, als 0.4908? Nein! Weiter!
      // {0.5, 0}     -> 0.5  -> Ist 0.5 größer, als 0.4908? Ja! Also speicher das in topIndex, damit wir das später verwenden können
      int topIndex = 0;
      for (int ii = 0; ii < sizeof(patternList[currentPattern.patternID]) / sizeof(patternList[currentPattern.patternID][0]); ii++) {
        if (patternList[currentPattern.patternID][ii][0] > totalProgress) {
          topIndex = ii;
          break;
        }
      }

      // Wir haben ja gesagt, dass bei 25% der Zeit der Motor die Stärke 80% haben soll, und bei 50% der Zeit die Stärke 0%
      // Wenn wir gerade bei 49.08% der Zeit sind müssen wir als letztes noch errechnen, wie stark der Motor jetzt genau vibrieren soll
      // Irgendwo zwischen 80% und 0% Power also
      // 49% ist ja sehr nah an 50%, also wahrscheinlich ist der Motor schon fast wieder aus
      // Durch topIndex können wir jetzt ja sagen, dass wir uns zwischen {0.25, 0.8} und {0.5, 0} befinden
      // Diese beiden werden als sectionStart und sectionEnd gespeichert
      float * sectionStart = patternList[currentPattern.patternID][topIndex - 1];
      float * sectionEnd = patternList[currentPattern.patternID][topIndex];
      // Als nächstes wird dann noch der Fortschritt zwischen den beiden Abschnitten errechnet
      //    sectionProgress = (0.4908        - 0.25           ) / (0.5           - 0.25           )
      float sectionProgress = (totalProgress - sectionStart[0]) / (sectionEnd[0] - sectionStart[0]);
      // sectionProgress ist also 0.9632 oder in anderen Worten: Wir haben 96.32% des Abschnittes von {0.25, 0.8} zu {0.5, 0} abgeschlossen
      // Mithilfe dieses "relativen" Fortschrittes zwischen den beiden Abschnitten wird dann die aktuelle Stärke errechnet
      //     currentIntensity = 0.8             + ((0             - 0.8            ) * 0.9632         )
       float currentIntensity = sectionStart[1] + ((sectionEnd[1] - sectionStart[1]) * sectionProgress);
       // Und so kriegen wir letzendlich den Wert, den der Motor gerade zu diesem Zeitpunkt vibrieren soll: 0.02944 bzw 2.944%

      // Dann lassen wir den Motor mal vibrieren...
      // i kommt aus dem Loop und ist die ID des aktuellen Motors
      // Die currentIntensity wird noch mit 255 multipliziert, um einen Wert zwischen 0 und 255 zu bekommen
      ledcWrite(i + 1, 255 * currentIntensity);


      // ###########
      // Ganz unten stehen viele Serial.print() Zeilen kommentiert, diese können hier zum debuggen eingefügt werden
      // ###########

    // Dieses Else kommt noch von der Abfrage oben, ob der aktuelle Motor vibrieren soll.
    // Falls er also nicht vibrieren soll (Kein Muster vorhanden oder gespeichertes Muster ist schon vorbei)
    } else {
      // Wird er einfach auf 0 gesetzt
      ledcWrite(i + 1, 0);
    }
  }

  // Und zuletzt wird die tCurrent Variable aktualisiert, für das nächste mal wenn motorLoop ausgeführt wird.
  tCurrent = millis();
}


// Hier kommen jetzt endlich die setup() und loop() Funktionen, angefangen mit setup()
// Die setup() Funktion wird sobald der ESP bereit ist einmal ausgeführt
void setup() {
  // Hier aktivieren wir die Serial Konsole. Ist wie console.log() in JavaScript
  Serial.begin(9600);

  // Warte kurz, damit der W-LAN Chip und so bereit sind
  delay(1000);

  // Verbinde dich mit dem W-LAN
  WiFi.begin(ssid, password);

  // So lange noch keine Verbindung besteht
  while (WiFi.status() != WL_CONNECTED) {
    // Warte eine Sekunde und probier es nochmal
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Wenn wir verbunden sind geben wir kurz die IP in der Konsole aus, damit man sich einfacher verbinden kann
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());

  // Jetzt machen wir das selbe, wie in der motorLoop Funktion, wir gehen alle Motoren durch
  for (int i = 0; i < sizeof(motorPINs) / sizeof(motorPINs[0]); i++) {
    // Erst wird jedem Motor ein Channel zugewiesen
    ledcAttachPin(motorPINs[i], i + 1);
    // Dann wird der Channel initialisiert (12 kHz PWM Frequenz und 8-bit)
    // Warum ist egal, it just works...
    ledcSetup(i + 1, 12000, 8);
  }

  // Dann wird tCurrent zu Beginn schon einmal gesetzt
  tCurrent = millis();

  // Und wir starten den WiFi Server
  wifiServer.begin();
}


// Und als letztes die loop() Funktion
// Diese wird ganz oft ausgeführt, wie in p5.js die draw() Funktion
void loop() {

  // Als erstes prüfen wir, ob wir mit einem anderen Gerät verbunden sind
  WiFiClient client = wifiServer.available();
  // Außerdem wird hier auch die motorLoop() Funktion ausgeführt.
  motorLoop();

  // Falls wir mit einem anderen Gerät verbunden sind
  if (client) {
    // So lange wir mit diesem Gerät verbunden sind
    while (client.connected()) {
      // Führe ständig motorLoop() aus
      motorLoop();

      // So lange der Client irgendwelche Daten gesendet hat, die wir noch nicht verarbeitet haben
      while (client.available() > 0) {
        // Führe socketLoop aus und verarbeite die Daten
        socketLoop(client.read());
      }
    }

    // Wenn der Client disconnected räum auf
    client.stop();
    Serial.println("Client disconnected");

  }
}








// Debug Hilfe
            // Serial.print("IDs: ");
            // Serial.print(i);
            // Serial.print(" | ");
            // Serial.print(topIndex);
            // Serial.print(" --- ");
            
            // Serial.print("Progress: ");
            // Serial.print(totalProgress);
            // Serial.print(" / ");
            // Serial.print(sectionProgress);
            // Serial.print(" (");
            // Serial.print(sectionStart[0]);
            // Serial.print(" | ");
            // Serial.print(sectionEnd[0]);
            // Serial.print(") --- ");
            
            // Serial.print("Intensity: ");
            // Serial.print(currentIntensity);
            // Serial.print(" - ");
            // Serial.print(255 * currentIntensity);
            // Serial.print(" (");
            // Serial.print(sectionStart[1]);
            // Serial.print(" | ");
            // Serial.print(sectionEnd[1]);
            // Serial.print(") --- ");
            
            // Serial.println(tCurrent);
