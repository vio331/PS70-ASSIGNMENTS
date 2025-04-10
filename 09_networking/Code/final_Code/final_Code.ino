#include <WiFi.h>
#include <DFRobotDFPlayerMini.h>


WiFiServer server(80);
const char *ssid = "MAKERSPACE";
const char *password = "12345678";
const int PIN = D7;


#define DFPLAYER_RX 7
#define DFPLAYER_TX 6
// NetworkServer server(80);



void setup() {
  Serial.begin(115200);
  Serial.println("ESP32C3 DFPlayer WiFi Fart Machine");
  pinMode(PIN, OUTPUT);  // set the LED pin mode
  delay(10);

// make the speaker work

  // Initialize DFPlayer serial connection
  dfPlayerSerial.begin(9600);
  
  // Initialize DFPlayer
   Serial.println("Initializing DFPlayer...");
  if (!myDFPlayer.begin(dfPlayerSerial, true, false)) {  // Use softwareSerial and don't reset DFPlayer
     Serial.println("DFPlayer not responding. Check wiring and SD card.");
   } else {
     Serial.println("DFPlayer initialized successfully!");
   }
  
  // Set DFPlayer volume (0 to 30)
  myDFPlayer.volume(25);
  
  // // Tell DFPlayer to use the SD card
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  NetworkClient client = server.accept();  // listen for incoming clients

  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        if (c == '\n') {            // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 8 on.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 8 off.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(PIN, HIGH);  // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(PIN, LOW);  // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
