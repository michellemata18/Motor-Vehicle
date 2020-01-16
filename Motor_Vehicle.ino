#include <WiFi101.h>
#include <WiFiClient.h>
#include <WiFiMDNSResponder.h>
#include <WiFiServer.h>
#include <WiFiSSLClient.h>
#include <WiFiUdp.h>
#include <DualMC33926MotorShield.h>
#include <SPI.h>


DualMC33926MotorShield md;

char ssid[] = "YourNetworkNameHere";      // your network SSID (name)
char pass[] = "YourPasswordHere"; //network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);
String readString;
boolean Motor_Status = 0;


const int nD2 = 4;
const int M1DIR = 7;
const int M1PWM = 9;
const int nSF = 12;


void setup()
{
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }

  md.init();
   pinMode(M1DIR, OUTPUT);
   pinMode(M1PWM, OUTPUT);
   pinMode(nD2, OUTPUT);
   digitalWrite(nD2, HIGH);
   pinMode(nSF, INPUT);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);


    delay(10000);
  }
  server.begin();

  // print out the status:
  printWifiStatus();
}

void loop()
{
  WiFiClient client = server.available();  // try to get client

  if (client) {  
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {   // client data available to read
        char c = client.read(); // read 1 byte (character) from client

        // respond to client only after last line received
        if (readString.length() < 100) {
          readString += c;
        }
        if (c == '\n' && currentLineIsBlank) {
          Serial.println(readString);
         
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println("Refresh: 10");
          client.println();
          // send web page
          client.println("<!DOCTYPE html>");
          client.println("<html>");
          client.println("<head>");
          client.println("<title>Arduino Web Page</title>");
          client.println("</head>");
          client.println("<body>");
          client.println("<h1>Vehicle 1 Controls</h1>");
          client.print("<input type=submit value=Forward style=width:100px;height:45px onClick=location.href='/?Forward'>");
          client.print("&nbsp;<input type=submit value=Reverse style=width:100px;height:45px onClick=location.href='/?Reverse'>");
          client.print("&nbsp;<input type=submit value=Stop style=width:100px;height:45px onClick=location.href='/?off'>");
          //.println("<form method=\"get\">");
          client.println("</form>");
          client.println("</body>");
          client.println("</html>");
         
          break;
        }
        if (c == '\n') {
          // last character on line of received text
          // starting new line with next character read
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // a text character was received from client
          currentLineIsBlank = false;


        }
      } // end of (client.available())
    } // end of (client.connected())
    delay(1);      // give the web browser time to receive the data
    client.stop(); // close the connection
    // end of (client)

    if (readString.indexOf("?Forward") > 0)
    {
      digitalWrite(M1DIR, HIGH);
      analogWrite(M1PWM, 400);
      delay(1000);
      digitalWrite(M1DIR, LOW);
      analogWrite(M1PWM, 0);
    }
    if (readString.indexOf("?Reverse") > 0)
    {

      digitalWrite(nD2, HIGH);
      digitalWrite(M1DIR, HIGH);
      analogWrite(M1PWM, -400);
      
      delay(1000);
      digitalWrite(M1DIR, LOW);
      analogWrite(M1PWM, 0);
    }
    
    if (readString.indexOf("?off") > 0)
    {
      digitalWrite(M1DIR, LOW);
      analogWrite(M1PWM, 0);
    }
    readString = "";
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
