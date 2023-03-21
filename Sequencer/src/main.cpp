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
String json = "[{\"id\":1,\"a\":\"-1.0241305149841473\",\"b\":\"-1.7591162300913243\",\"gate\":true,\"trigger\":true,\"type\":\"0\",\"nextNodes\":[\"2\",\"-1\"]},{\"id\":2,\"a\":\"2.092339261959495\",\"b\":\"10.550034742090492\",\"gate\":false,\"trigger\":true,\"type\":\"0\",\"nextNodes\":[\"3\",\"-1\"]},{\"id\":3,\"a\":\"-10.430746999304823\",\"b\":\"9.309318380791229\",\"gate\":true,\"trigger\":true,\"type\":\"0\",\"nextNodes\":[\"4\",\"-1\"]},{\"id\":4,\"a\":\"11.882958557279402\",\"b\":\"10.298333446659466\",\"gate\":false,\"trigger\":true,\"type\":\"0\",\"nextNodes\":[\"5\",\"-1\"]},{\"id\":5,\"a\":\"1.406831537341903\",\"b\":\"-5.044896380944483\",\"gate\":false,\"trigger\":false,\"type\":\"0\",\"nextNodes\":[\"6\",\"-1\"]},{\"id\":6,\"a\":\"10.303789138166145\",\"b\":\"-5.219179833451005\",\"gate\":false,\"trigger\":false,\"type\":\"0\",\"nextNodes\":[\"1\",\"-1\"]}]";

JsonArray nodes;

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
  digitalWrite(outputChipSelectPin, LOW); // activate DAC Communication

  hspi->transfer16(data);

  digitalWrite(outputChipSelectPin, HIGH); // deactivate DAC Communication
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
  seqNextNode = seqCurrentNode.getNextNode(); // determine the next Node
}

void reset()
{
  seqCurrentNode = startNode; // overwrite the old current with the new current Node
  outputData();
  seqNextNode = startNode.getNextNode(); // determine the next Node
}

void determineStart()
{
  // find lowest id
  int lowestId = std::numeric_limits<int>::max();
  for (JsonVariant v : nodes)
  {
    if (v["id"].as<int>() < lowestId)
      ;
  }
  JsonObject jsonStartNode = nodes[lowestId];
  startNode.setValues(jsonStartNode["a"], jsonStartNode["b"], jsonStartNode["trigger"], jsonStartNode["gate"]);
}

Node nodeFromJson(JsonObject jsonObject)
{
  Node newNode;
  int type = jsonObject["type"];
  switch (type)
  {
  case 0:
    newNode = new Node(jsonObject["a"], jsonObject["b"], jsonObject["gate"], jsonObject["trigger"], NULL);
    break;

  default:
    break;
  }
}

void load()
{
  DynamicJsonDocument document(4096);
  DeserializationError error = deserializeJson(document, json);
  if (error)
  {
    Serial.print(F("ERROR: deserializeJson() failed with code "));
    Serial.println(error.f_str());
    return;
  }

  nodes = document.as<JsonArray>();

  determineStart();
}

/**
 * @brief Arduino Setup function
 * "put your setup code here, to run once:"
 */
void setup()
{
  pinMode(resetPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(resetPin), reset, FALLING);

  pinMode(stepPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(stepPin), step, FALLING);

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
        Serial.println("LOG: POST recieved");
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
