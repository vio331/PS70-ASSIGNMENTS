#include <WiFi.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// WiFi setup
const char *ssid = "MAKERSPACE";
const char *password = "12345678";

// Web server on port 80
WiFiServer server(80);

// DFPlayer pins for ESP32C3 XIAO
// RX of ESP32 connects to TX of DFPlayer
// TX of ESP32 connects to RX of DFPlayer
#define DFPLAYER_RX D1  // GPIO6 (D6) - Connect to TX pin on DFPlayer
#define DFPLAYER_TX D0  // GPIO7 (D7) - Connect to RX pin on DFPlayer

// Create a SoftwareSerial instance for DFPlayer
SoftwareSerial dfPlayerSerial(DFPLAYER_RX, DFPLAYER_TX);

// Create DFPlayer object
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  Serial.println("ESP32C3 DFPlayer WiFi Fart Machine");
  
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
  
  // Tell DFPlayer to use the SD card
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  
  // Configure WiFi in station mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.printf("Connecting to %s\n", ssid);
  
  // Wait for connection with timeout
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print(".");
    timeout++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection failed!");
  }
  
  // Start the web server
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  // Check for clients connecting to the server
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("New client connected");
    String currentLine = "";
    
    // While the client is connected
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        // Read the request line by line
        if (c == '\n') {
          // If the current line is blank, this is the end of the HTTP request
          if (currentLine.length() == 0) {
            // Send HTTP response header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Send the HTML page
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head>");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<style>");
            client.println("body { font-family: Arial; text-align: center; background-color: #f0f0f0; margin: 0; padding: 20px; }");
            client.println("h1 { color: #333; }");
            client.println(".button { display: inline-block; background-color: #4CAF50; border: none; color: white; padding: 16px 40px; ");
            client.println("text-decoration: none; font-size: 24px; margin: 20px; cursor: pointer; border-radius: 12px; }");
            client.println(".button:hover { background-color: #45a049; }");
            client.println("</style>");
            client.println("</head>");
            client.println("<body>");
            client.println("<h1>💨 Remote Fart Machine</h1>");
            client.println("<p><a href=\"/fart\" class=\"button\">FART! 💥</a></p>");
            client.println("<p>IP Address: " + WiFi.localIP().toString() + "</p>");
            client.println("</body>");
            client.println("</html>");
            
            // End the HTTP response
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          // Add the character to the currentLine
          currentLine += c;
        }
        
        // Check if the request is to trigger the fart sound
        if (currentLine.endsWith("GET /fart")) {
          Serial.println("💨 FART TRIGGERED!");
          
          // For file in root directory: myDFPlayer.playMp3Folder(1) won't work
          // Instead use direct file number for root directory files
          myDFPlayer.play(1);  // Play file 0001.mp3 from the root directory
          
          // Alternative method if the above doesn't work
          // Sometimes using the exact filename can be more reliable
          // myDFPlayer.playMp3("0001.mp3");  // Uncomment this if play(1) doesn't work
        }
      }
    }
    
    // Close the connection
    client.stop();
    Serial.println("Client disconnected");
  }
  
  // Check if DFPlayer has messages
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read());
  }
}

// Function to print DFPlayer messages
void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Play Finished: "));
      Serial.println(value);
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError: "));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          Serial.println(F("Unknown error"));
          break;
      }
      break;
    default:
      break;
  }
}