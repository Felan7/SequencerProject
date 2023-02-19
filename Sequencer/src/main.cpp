#include <Arduino.h>

#include <Node.h>

#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#include "SPIFFS.h" //internal storage
#include "SPI.h"    //SPI communication

// Nodes
Node seqCurrentNode;
Node seqNextNode;
Node startNode;

// Inputs
const int resetPin = 22;  // Reset Input Pin (Interrupt GPIO)
const int stepPin = 23;   // Step Input Pin (Interrupt GPIO)
const int xInputPin = 32; // X Input (ADC Pin)
const int yInputPin = 33; // y Input (ADC Pin)

// Outputs
const int outputPinA = 14;       // TODO: find real pin number
const int outputPinB = 15;       // TODO: find real pin number
const int outputPinTrigger = 16; // TODO: find real pin number
const int outputPinGate = 17;    // TODO: find real pin number
const int chipSelectPin = 18;

// Variable to store the HTTP request
String header;

// SPI
SPIClass *hspi = NULL;

// SSID & Password
const char *ssid = "ESP32";      // Enter your SSID here
const char *password = "secret"; // Enter your Password here
AsyncWebServer server(80);       // Object of WebServer(HTTP port, 80 is default)

// IP Address details
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

int translateOutputValues(double value)
{
  return (int)value;
}

/**
 * @brief Writes the data to the output pins.
 *
 */
void outputData()
{
  valueStruct outputValues = seqCurrentNode.getValues();
  // Value A
  analogWrite(outputPinA, translateOutputValues(outputValues.valueA));
  // Value B
  analogWrite(outputPinA, translateOutputValues(outputValues.valueA));
  // Trigger
  if (outputValues.trigger)
  {
    digitalWrite(outputPinTrigger, HIGH);
  }
  else
  {
    digitalWrite(outputPinTrigger, LOW);
  }

  // Gate
  if (outputValues.gate)
  {
    digitalWrite(outputPinGate, HIGH);
  }
  else
  {
    digitalWrite(outputPinGate, LOW);
  }
}

word buildWord(word value, bool writeToB = false)
{
  if (value > 1023)
  {
    value = 1023;
  }

  // B BUF NOT_GAIN NOT_SHDN
  word mcp_configuration = 0;
  if (writeToB)
  {
    // write to b
    mcp_configuration = B11010000 << 8;
  }
  else
  {
    // write to a
    mcp_configuration = B01110000 << 8;
  }
  word returnValue = value << 2;

  returnValue += mcp_configuration;

  return returnValue;
}

void sendWord(word data)
{
  digitalWrite(chipSelectPin, LOW); // activate DAC Communication

  hspi->transfer16(data);

  digitalWrite(chipSelectPin, HIGH); // deactivate DAC Communication
}

/**
 * @brief Interupt function for the step Input
 * Switches to the nextNode
 */
void step()
{

  seqCurrentNode = seqNextNode; // overwrite the old current with the new current Node
  outputData();
  seqNextNode = seqCurrentNode.getNextNode(); // determine the next Node
}

/**
 * @brief Interupt function for the reset Input
 * Resets the sequence to the starting node.
 */
void reset()
{
  seqNextNode = startNode.getNextNode();
}

// HTML & CSS contents which display on web server
String HTML = "<!DOCTYPE html>\
<html>\
<body>\
<h1>My First Web Server with ESP32 - AP Mode &#128522;</h1>\
</body>\
</html>";

// Replaces placeholder with LED state value
String processor(const String &var)
{
  return String();
}

const int testPin1 = 34;
const int testPin2 = 35;
/**
 * @brief Arduino Setup function
 * "put your setup code here, to run once:"
 */
void setup()
{
  pinMode(resetPin, INPUT);
  attachInterrupt(resetPin, reset, RISING);

  pinMode(stepPin, INPUT);
  attachInterrupt(stepPin, step, RISING);

  pinMode(testPin1, INPUT);
  pinMode(testPin2, INPUT);

  pinMode(outputPinA, OUTPUT);
  pinMode(outputPinB, OUTPUT);
  pinMode(outputPinTrigger, OUTPUT);
  pinMode(outputPinGate, OUTPUT);
  pinMode(chipSelectPin, OUTPUT);

  Serial.begin(115200);
  Serial.println("Hello");

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("ERROR: An Error has occurred while mounting SPIFFS.");
    return;
  }

  // SPI
  hspi = new SPIClass(HSPI);
  hspi->begin(); // Begin SPI Communication
  hspi->setBitOrder(MSBFIRST);

  // Create SoftAP
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);

  Serial.print("LOG: SSID=");
  Serial.println(ssid);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html"); });

  server.on(
      "/post",
      HTTP_POST,
      [](AsyncWebServerRequest *request) {},
      NULL,
      [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
      {
        for (size_t i = 0; i < len; i++)
        {
          Serial.write(data[i]);
        }

        Serial.println();

        request->send(200);
      });
}
});
server.begin();
Serial.println("LOG: HTTP server started");
}

/**
 * @brief Arduino loop function
 * "put your main code here, to run repeatedly:"
 */
void loop()
{
  int sensorValue = analogRead(testPin1) / 4;
  Serial.print("in=");
  Serial.print(sensorValue);
  Serial.print(" out=");
  sendWord(buildWord(sensorValue));
  Serial.println(analogRead(testPin2));
  delay(1000);
}
