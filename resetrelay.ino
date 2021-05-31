#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "mySSID"
#define STAPSK  "myPassword"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

#define PIN_LED 13
#define PIN_RELAY 12
#define PIN_BUTTON 0

int value = 1;

int i = 0;

WiFiServer server(80);

void on()
{
  digitalWrite(PIN_LED, LOW);
  digitalWrite(PIN_RELAY, HIGH);
  value = 1;
}

void off()
{
  digitalWrite(PIN_LED, HIGH);
  digitalWrite(PIN_RELAY, LOW);
  value = 0;
}

void toggle()
{
  if(value)
  {
    off();
  }
  else
  {
    on();
  }
}

void reset()
{
  if(value == 1)
  {
    off();
    delay(200);
  }
  on();
  delay(200);  
}

void IRAM_ATTR isr() {
  toggle();  
  delay(1000);
}

void setup() {
  Serial.begin(9600);

  Serial.printf("Relay controller\n");  
  
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);
  
  attachInterrupt(PIN_BUTTON, isr, FALLING);


    // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }

  Serial.println();
  Serial.println(F("WiFi connected"));

  // Start the server
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());

  on();  
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println(F("new client"));

  client.setTimeout(5000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(F("request: "));
  Serial.println(req);

  // Match the request
  
  if (     req.indexOf(F("/gpio/0")) != -1) { off();  } 
  else if (req.indexOf(F("/gpio/1")) != -1) { on(); } 
  else if (req.indexOf(F("/gpio/T")) != -1) { toggle(); } 
  else if (req.indexOf(F("/gpio/R")) != -1) { reset(); } 
  else{ Serial.println(F("invalid request")); }

  // read/ignore the rest of the request
  // do not client.flush(): it is for output only, see below
  while (client.available()) {
    // byte by byte is not very efficient
    client.read();
  }

  // Send the response to the client
  // it is OK for multiple small client.print/write,
  // because nagle algorithm will group them into one single packet
  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now "));
  client.print((value) ? F("high") : F("low"));
  client.print(F("<br><br>"));
  client.print(F("<br><br>Click <a href='http://")); client.print(WiFi.localIP()); client.print(F("/gpio/T'>here</a> to toggle relay"));
  client.print(F("<br><br>Click <a href='http://")); client.print(WiFi.localIP()); client.print(F("/gpio/R'>here</a> to reset"));
  client.print(F("<br><br>Click <a href='http://")); client.print(WiFi.localIP()); client.print(F("/gpio/1'>here</a> to toggle relay on"));
  client.print(F("<br><br>Click <a href='http://")); client.print(WiFi.localIP()); client.print(F("/gpio/0'>here</a> to toggle relay off"));
  client.print(F("</html>"));

  // The client will actually be *flushed* then disconnected
  // when the function returns and 'client' object is destroyed (out-of-scope)
  // flush = ensure written data are received by the other side
  Serial.println(F("Disconnecting from client"));
  
}
