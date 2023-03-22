#include <Arduino.h>

#include <Node.h>

#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#include "SPIFFS.h" //internal storage
#include "SPI.h"    //SPI communication

#include <ArduinoJson.h> //JSON
#include <limits>

// Nodes
Node seqCurrentNode;
Node seqNextNode;
Node startNode;
int SeqCurrentNodeId;
int seqNextNodeId;
int startNodeId;

// Inputs
const int resetPin = 16;          // Reset Input Pin (Interrupt GPIO)
const int stepPin = 17;           // Step Input Pin (Interrupt GPIO)
const int inputChipSelectPin = 5; // input SPI chip select pin

// Outputs
const int outputPinTrigger = 2;     // TODO: find real pin number
const int outputPinGate = 15;       // TODO: find real pin number
const int outputChipSelectPin = 22; // output SPI chip select pin

// Variable to store the HTTP request
String header;

// SPI
SPIClass *hspi = NULL;
SPIClass *vspi = NULL;

// SSID & Password
const char *ssid = "ESP32";      // Enter your SSID here
const char *password = "secret"; // Enter your Password here
AsyncWebServer server(80);       // Object of WebServer(HTTP port, 80 is default)

// IP Address details
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

bool reset_state = false;
bool step_state = false;
bool load_state = false;

// global input variables
double x = 0;
double y = 0;

// JSON string
String json = "[{\"id\":1,\"a\":\"-4.723400702605518\",\"b\":\"-2.192637650019835\",\"gate\":true,\"trigger\":true,\"type\":\"1\",\"nextNodes\":[\"2\",\"3\"]},{\"id\":2,\"a\":\"3.771978910850443\",\"b\":\"7.509817019959371\",\"gate\":true,\"trigger\":true,\"type\":\"0\",\"nextNodes\":[\"1\",\"-1\"]},{\"id\":3,\"a\":\"7.92840489573215\",\"b\":\"9.281641712665706\",\"gate\":true,\"trigger\":false,\"type\":\"0\",\"nextNodes\":[\"1\",\"-1\"]}]";

const int NODES_LENGTH = 99;
Node nodes[NODES_LENGTH];

int translateOutputValues(double value)
{
  return (int)value;
}

word scaleValue(double doubleValue)
{
  if (doubleValue < -5)
  {
    doubleValue = -5;
  }
  else if (doubleValue > 5)
  {
    doubleValue = 5;
  }

  doubleValue += 5;

  return doubleValue * 102.3; // (x - from_min) * (to_max - to_min) / (from_max - from_min) + to_min
}

word buildWord(double doubleValue, bool writeToB = false)
{

  word value = scaleValue(doubleValue);

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
    mcp_configuration = B01010000 << 8;
  }
  word returnValue = value << 2;

  returnValue += mcp_configuration;

  return returnValue;
}

void sendWord(word data)
{
  digitalWrite(outputChipSelectPin, LOW); // activate DAC Communication

  hspi->transfer16(data);

  digitalWrite(outputChipSelectPin, HIGH); // deactivate DAC Communication
}

/**
 * @brief Writes the data to the output pins.
 *
 */
void outputData()
{
  valueStruct outputValues = seqCurrentNode.getValues();
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

  sendWord(buildWord(outputValues.valueA));
  sendWord(buildWord(outputValues.valueB, true));
}

/**
 * @brief Interrupt function for the step Input
 * Switches to the nextNode
 */
void IRAM_ATTR interrupt_step()
{
  // seqCurrentNode = seqNextNode; // overwrite the old current with the new current Node
  // outputData();
  // seqNextNode = seqCurrentNode.getNextNode(); // determine the next Node
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200)
  {
    step_state = true;
  }
  last_interrupt_time = interrupt_time;
}

/**
 * @brief Interupt function for the reset Input
 * Resets the sequence to the starting node.
 */
void IRAM_ATTR interrupt_reset()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200)
  {
    reset_state = true;
  }
  last_interrupt_time = interrupt_time;
  // seqNextNode = startNode.getNextNode();
}

void step()
{
  seqCurrentNode = seqNextNode; // overwrite the old current with the new current Node
  outputData();
  seqNextNode = nodes[seqCurrentNode.getNextNode()]; // determine the next Node
}

void reset()
{
  seqCurrentNode = startNode; // overwrite the old current with the start Node
  outputData();
  seqNextNode = nodes[startNode.getNextNode()]; // determine the next Node
}

void determineStart()
{
  startNodeId = 1;
  startNode = nodes[startNodeId];
  // TODO: actually find lowest id
  // int lowestId = std::numeric_limits<int>::max();
  // for (size_t i = 0; i < NODES_LENGTH; i++)
  // {
  //   if (nodes[i] != NULL)
  //   {
  //     startNode = i;
  //     break;
  //   }
  // }
}

Node nodeFromJson(JsonObject jsonObject)
{
  Node newNode(
      jsonObject["id"].as<int>(),
      jsonObject["a"].as<double>(),
      jsonObject["b"].as<double>(),
      jsonObject["gate"].as<bool>(),
      jsonObject["trigger"].as<bool>(),
      jsonObject["nextNodes"][0].as<int>(),
      jsonObject["nextNodes"][1].as<int>(),
      static_cast<nodeType>(jsonObject["type"].as<int>()));

  return newNode;
}

void load()
{
  DynamicJsonDocument document(4096);
  DeserializationError error = deserializeJson(document, json);
  if (error)
  {
    Serial.print(F("ERROR: deserializeJson() failed with code "));
    Serial.println(error.f_str());
  }
  else
  {
    JsonArray jsonArray = document.as<JsonArray>();
    for (JsonVariant v : jsonArray)
    {
      nodes[v["id"].as<int>()] = nodeFromJson(v);
    }
    determineStart();
    reset();
  }
}

/**
 * @brief Arduino Setup function
 * "put your setup code here, to run once:"
 */
void setup()
{
  pinMode(resetPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(resetPin), interrupt_reset, FALLING);

  pinMode(stepPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(stepPin), interrupt_step, FALLING);

  pinMode(inputChipSelectPin, OUTPUT);
  digitalWrite(inputChipSelectPin, HIGH);

  pinMode(outputPinTrigger, OUTPUT);
  pinMode(outputPinGate, OUTPUT);
  pinMode(outputChipSelectPin, OUTPUT);

  Serial.begin(115200);
  Serial.println("LOG: Serial started");

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("ERROR: An Error has occurred while mounting SPIFFS.");
    return;
  }

  // SPI h
  hspi = new SPIClass(HSPI);
  hspi->begin(); // Begin SPI Communication
  hspi->setBitOrder(MSBFIRST);

  // SPI v
  vspi = new SPIClass(HSPI);
  vspi->setBitOrder(MSBFIRST);
  vspi->setDataMode(SPI_MODE3);
  vspi->begin(); // Begin SPI Communication

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
        Serial.println("LOG: POST received");
        request->send(200);
        for (size_t i = 0; i < len; i++)
        {
          Serial.write(data[i]);
        }

        Serial.println();
      });

  server.serveStatic("/", SPIFFS, "/"); // load static server files from Memory
  server.onNotFound([](AsyncWebServerRequest *request)
                    { request->send(404); });

  server.begin();
  Serial.println("LOG: HTTP server started");
  Serial.print("LOG: localIP=");
  Serial.println(WiFi.localIP());

  load_state = true; // force loading
}

void readInputs()
{
  unsigned int dataIn = 0;
  unsigned int tempX = 0;

  digitalWrite(inputChipSelectPin, LOW);
  uint8_t dataOut = 0b00000001;
  dataIn = vspi->transfer(dataOut);
  dataOut = 0b10100000;
  dataIn = vspi->transfer(dataOut);
  tempX = dataIn & 0x0F;
  dataIn = vspi->transfer(0x00);
  tempX = tempX << 8;
  tempX = tempX | dataIn;
  x = tempX;
  Serial.print("x=");
  Serial.print(tempX);

  dataOut = 0b00000001;
  dataIn = vspi->transfer(dataOut);
  dataOut = 0b11100000;
  dataIn = vspi->transfer(dataOut);
  unsigned int tempY = dataIn & 0x0F;
  dataIn = vspi->transfer(0x00);
  tempY = tempY << 8;
  tempY = tempY | dataIn;
  y = tempY;
  Serial.print(" y=");
  Serial.println(tempY);

  // input = input << 1;
  digitalWrite(inputChipSelectPin, HIGH);
}

/**
 * @brief Arduino loop function
 * "put your main code here, to run repeatedly:"
 */
void loop()
{

  // readInputs();

  if (reset_state)
  {
    reset();
    reset_state = false;
  }

  if (step_state)
  {
    step();
    step_state = false;
  }

  if (load_state)
  {
    load();
    load_state = false;
  }
}
