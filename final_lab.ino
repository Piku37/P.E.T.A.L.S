#include "config.h"
#include <DHT.h>  // Include DHT sensor library
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/************************** Configuration ***********************************/
#define DHTPIN 2   // Define the pin connected to the DHT sensor
#define DHTTYPE DHT11  // Define the type of DHT sensor

DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor

/************************ Adafruit display *******************************/
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define the Adafruit IO feed
AdafruitIO_Feed *zap1 = io.feed("zap1");
AdafruitIO_Feed *zap2 = io.feed("zap2");

const int soilMoisturePin = 34; // Define the pin connected to the soil moisture sensor

// Define thresholds for sensor readings
const float minTemperature = 18.0; // Minimum temperature for plant happiness
const float maxTemperature = 30.0; // Maximum temperature for plant happiness
const float minHumidity = 40.0;    // Minimum humidity for plant happiness
const float maxHumidity = 80.0;    // Maximum humidity for plant happiness
const int minSoilMoisture = 300;   // Minimum soil moisture for plant happiness
const int maxsoilMoisture = 800;
// Function declarations
void handleZap1Message(AdafruitIO_Data *data);
void handleZap2Message(AdafruitIO_Data *data);
void happyDisplay();
void unhappyDisplay(float temperature, float humidity, int soilMoisture);

//bitmapdata
const unsigned char Calendar [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0x83, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0x01, 0xff, 0xff, 0xc0, 0x7f, 0xff, 
	0xff, 0xff, 0x00, 0xff, 0xff, 0xc0, 0x3f, 0xff, 0xff, 0xff, 0x10, 0xff, 0xff, 0xc0, 0x3f, 0xff, 
	0xff, 0xfe, 0x10, 0xff, 0xff, 0xc4, 0x3f, 0xff, 0xff, 0x00, 0x10, 0x00, 0x00, 0x04, 0x00, 0x3f, 
	0xfe, 0x00, 0x10, 0x00, 0x00, 0x04, 0x00, 0x3f, 0xfe, 0x00, 0x10, 0x00, 0x00, 0x04, 0x00, 0x3f, 
	0xfe, 0x1f, 0x10, 0xff, 0xff, 0xc4, 0x3e, 0x3f, 0xfe, 0x1f, 0x00, 0xff, 0xff, 0xc0, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x00, 0xff, 0xff, 0xc0, 0x3e, 0x3f, 0xfe, 0x1f, 0x01, 0xff, 0xff, 0xc0, 0x7e, 0x3f, 
	0xfe, 0x1f, 0x83, 0xff, 0xff, 0xf0, 0xfe, 0x3f, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 
	0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 
	0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 
	0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 
	0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 
	0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x3f, 0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 0xfe, 0x1f, 0x00, 0x10, 0x00, 0x00, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x0a, 0x16, 0x14, 0x34, 0x3e, 0x3f, 0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x3f, 
	0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 
	0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xfe, 0x10, 0x00, 0x00, 0x00, 0x00, 0x02, 0x3f, 
	0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 
	0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

const unsigned char Mail [] PROGMEM = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 
	0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 
	0xfc, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x3f, 0xfc, 0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x3f, 
	0xfc, 0x20, 0x1f, 0xff, 0xff, 0xf8, 0x04, 0x3f, 0xfc, 0x30, 0x0f, 0xff, 0xff, 0xf0, 0x0c, 0x3f, 
	0xfc, 0x3c, 0x03, 0xff, 0xff, 0xc0, 0x3c, 0x3f, 0xfc, 0x3e, 0x01, 0xff, 0xff, 0x80, 0x7c, 0x3f, 
	0xfc, 0x3f, 0x80, 0x7f, 0xfe, 0x01, 0xfc, 0x3f, 0xfc, 0x3f, 0xc0, 0x3f, 0xfc, 0x03, 0xfc, 0x3f, 
	0xfc, 0x3f, 0xf0, 0x0f, 0xf0, 0x0f, 0xfc, 0x3f, 0xfc, 0x3f, 0xf8, 0x07, 0xe0, 0x1f, 0xfc, 0x3f, 
	0xfc, 0x3f, 0xfe, 0x01, 0x80, 0x7f, 0xfc, 0x3f, 0xfc, 0x3f, 0xfc, 0x00, 0x00, 0x3f, 0xfc, 0x3f, 
	0xfc, 0x3f, 0xf8, 0x00, 0x00, 0x1f, 0xfc, 0x3f, 0xfc, 0x3f, 0xe0, 0x00, 0x00, 0x07, 0xfc, 0x3f, 
	0xfc, 0x3f, 0xc0, 0x78, 0x1e, 0x03, 0xfc, 0x3f, 0xfc, 0x3f, 0x00, 0xfc, 0x3f, 0x00, 0xfc, 0x3f, 
	0xfc, 0x3e, 0x03, 0xff, 0xff, 0xc0, 0x7c, 0x3f, 0xfc, 0x3c, 0x07, 0xff, 0xff, 0xe0, 0x3c, 0x3f, 
	0xfc, 0x30, 0x0f, 0xff, 0xff, 0xf0, 0x0c, 0x3f, 0xfc, 0x20, 0x3f, 0xff, 0xff, 0xfc, 0x04, 0x3f, 
	0xfc, 0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x3f, 0xfc, 0x01, 0xff, 0xff, 0xff, 0xff, 0x80, 0x3f, 
	0xfc, 0x03, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x3f, 0xfc, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x3f, 
	0xfc, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x3f, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3f, 
	0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3f, 
	0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 
	0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0x7f, 0xff, 0xff, 0xfa, 0xff, 0xff, 0xff, 0xfe, 0x37, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xfe, 
	0x34, 0x55, 0xc0, 0x8a, 0xe4, 0x44, 0x54, 0x18, 0x50, 0x45, 0xd2, 0xca, 0xec, 0x5c, 0x15, 0x06, 
	0x41, 0xc9, 0xd6, 0xaa, 0xe9, 0xdd, 0xd9, 0x76, 0x64, 0x4b, 0xd6, 0x8a, 0xec, 0x44, 0x59, 0x18, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

//happy
const unsigned char happy [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


// 'sad', 64x64px
const unsigned char sad [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'sun'
const unsigned char sun [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x1f, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0x9f, 0xf8, 0x8f, 0xfc, 0xff, 0xff, 0xff, 0xff, 0x87, 0xf9, 0x8f, 0xf0, 0xff, 0xff, 
	0xff, 0xff, 0x81, 0xf1, 0xc7, 0xc0, 0xff, 0xff, 0xff, 0xff, 0x90, 0xf3, 0xe7, 0x80, 0xff, 0xff, 
	0xff, 0xff, 0x98, 0x23, 0xe2, 0x0c, 0xff, 0xff, 0xff, 0xff, 0x9e, 0x07, 0xf0, 0x3c, 0xff, 0xff, 
	0xff, 0xff, 0x9f, 0x84, 0x90, 0xfc, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xcc, 0x99, 0x7c, 0xff, 0xff, 
	0xff, 0xff, 0x9e, 0x7f, 0xff, 0x3c, 0xff, 0xff, 0xff, 0xff, 0x9b, 0x70, 0x07, 0x4c, 0xff, 0xff, 
	0xfe, 0x00, 0x13, 0x80, 0x00, 0xcc, 0x00, 0x3f, 0xfc, 0x00, 0x1e, 0x03, 0xe0, 0x3c, 0x00, 0x1f, 
	0xfe, 0x00, 0x1c, 0x3f, 0xfc, 0x1c, 0x00, 0x3f, 0xfe, 0x3f, 0xf8, 0xff, 0xff, 0x0f, 0xfe, 0x7f, 
	0xff, 0x1f, 0x31, 0xff, 0xff, 0xc6, 0x7c, 0x7f, 0xff, 0x9f, 0x23, 0xff, 0xff, 0xe2, 0x7c, 0xff, 
	0xff, 0x8f, 0xc7, 0xff, 0xff, 0xf1, 0xf8, 0xff, 0xff, 0xcc, 0x8f, 0xff, 0xff, 0xf9, 0x91, 0xff, 
	0xff, 0xc4, 0x8f, 0xff, 0xff, 0xf8, 0x91, 0xff, 0xff, 0xe3, 0x9f, 0xff, 0xff, 0xfc, 0xe3, 0xff, 
	0xff, 0xe3, 0x1c, 0x3f, 0xfe, 0x3c, 0x63, 0xff, 0xff, 0xc3, 0x38, 0x1f, 0xfc, 0x1c, 0x61, 0xff, 
	0xff, 0x07, 0x38, 0x1f, 0xfc, 0x8e, 0x70, 0x7f, 0xfc, 0x1a, 0x39, 0x9f, 0xfc, 0x8e, 0x6c, 0x1f, 
	0xf0, 0x72, 0x38, 0x1f, 0xfc, 0x0e, 0x27, 0x07, 0xc1, 0xfe, 0x3c, 0x3f, 0xfe, 0x1e, 0x3f, 0xc1, 
	0xc1, 0xfe, 0x3f, 0xff, 0xff, 0xfe, 0x3f, 0xc1, 0xf0, 0x72, 0x3f, 0xf8, 0x1f, 0xfe, 0x27, 0x07, 
	0xfc, 0x1a, 0x3f, 0xe0, 0x03, 0xfe, 0x6c, 0x1f, 0xff, 0x0f, 0x3f, 0x80, 0x00, 0xfe, 0x78, 0x7f, 
	0xff, 0x83, 0x3f, 0x8f, 0xf8, 0xfc, 0x60, 0xff, 0xff, 0xe3, 0x1f, 0xbf, 0xfd, 0xfc, 0x63, 0xff, 
	0xff, 0xe3, 0x9f, 0xff, 0xff, 0xfc, 0xe3, 0xff, 0xff, 0xc4, 0x8f, 0xff, 0xff, 0xf8, 0x91, 0xff, 
	0xff, 0xcc, 0xcf, 0xff, 0xff, 0xf9, 0x91, 0xff, 0xff, 0x8f, 0xc7, 0xff, 0xff, 0xf1, 0xf8, 0xff, 
	0xff, 0x9f, 0x23, 0xff, 0xff, 0xe2, 0x7c, 0xff, 0xff, 0x1f, 0x31, 0xff, 0xff, 0xc6, 0x7c, 0x7f, 
	0xfe, 0x3f, 0xf8, 0xff, 0xff, 0x0f, 0xfe, 0x7f, 0xfe, 0x00, 0x1c, 0x3f, 0xfc, 0x1c, 0x00, 0x3f, 
	0xfc, 0x00, 0x1e, 0x03, 0xe0, 0x3c, 0x00, 0x1f, 0xfe, 0x00, 0x13, 0x80, 0x00, 0xcc, 0x00, 0x3f, 
	0xff, 0xff, 0x9a, 0x70, 0x07, 0x4c, 0xff, 0xff, 0xff, 0xff, 0x9e, 0x7f, 0xfe, 0x3c, 0xff, 0xff, 
	0xff, 0xff, 0x9f, 0xcc, 0x9b, 0x7c, 0xff, 0xff, 0xff, 0xff, 0x9f, 0x84, 0x90, 0xfc, 0xff, 0xff, 
	0xff, 0xff, 0x9e, 0x07, 0xf0, 0x3c, 0xff, 0xff, 0xff, 0xff, 0x98, 0x23, 0xe2, 0x0c, 0xff, 0xff, 
	0xff, 0xff, 0x80, 0xf3, 0xc7, 0x80, 0xff, 0xff, 0xff, 0xff, 0x83, 0xf1, 0xc7, 0xc0, 0xff, 0xff, 
	0xff, 0xff, 0x87, 0xf9, 0x8f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xf8, 0x9f, 0xfc, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfc, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// 'thermometer', 64x64px
const unsigned char thermometer [] PROGMEM = {
	0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x83, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x87, 0xe1, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x0e, 0x70, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0c, 0x30, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x0c, 0x30, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0c, 0x30, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x0c, 0x30, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0e, 0x70, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x0e, 0x70, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0c, 0x30, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x0c, 0x30, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0c, 0x30, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x0c, 0x30, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0c, 0x30, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x0c, 0x30, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0c, 0x30, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x0c, 0x30, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0c, 0x30, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x0c, 0x30, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0c, 0x30, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x9c, 0x30, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x30, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xf8, 0x70, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x70, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x80, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0xf0, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xfe, 0x07, 0xf8, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x1f, 0xf8, 0x3f, 0xff, 0xff, 
	0xff, 0xff, 0xfc, 0x3f, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x7f, 0xfe, 0x1f, 0xff, 0xff, 
	0xff, 0xff, 0xf8, 0x7c, 0x3e, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xf8, 0x1f, 0x1f, 0xff, 0xff, 
	0xff, 0xff, 0xf0, 0xf0, 0x0f, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xf0, 0x0f, 0x0f, 0xff, 0xff, 
	0xff, 0xff, 0xf0, 0xf0, 0x0f, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xf0, 0x0f, 0x0f, 0xff, 0xff, 
	0xff, 0xff, 0xf8, 0xf8, 0x1f, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x7c, 0x3e, 0x1f, 0xff, 0xff, 
	0xff, 0xff, 0xf8, 0x7f, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xfc, 0x3f, 0xff, 0xff, 
	0xff, 0xff, 0xfc, 0x1f, 0xf8, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x07, 0xe0, 0x7f, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xff
};


// 'water', 64x64px
const unsigned char water [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xfc, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x7f, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xf8, 0x07, 0xff, 
	0xff, 0xff, 0xff, 0xfe, 0x0f, 0xff, 0x83, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xc1, 0xff, 
	0xff, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xf8, 0x7f, 
	0xff, 0xff, 0xff, 0xe1, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xfc, 0x3f, 
	0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xc7, 0xff, 0xff, 0xff, 0x1f, 
	0xff, 0xff, 0xff, 0xc7, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x87, 0xff, 0xff, 0xff, 0x0f, 
	0xff, 0xff, 0xff, 0x8f, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0x8f, 
	0xff, 0xff, 0xff, 0x80, 0x3f, 0xff, 0xff, 0x8f, 0xff, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0x8f, 
	0xff, 0xff, 0xff, 0x8c, 0x00, 0xff, 0xff, 0x8f, 0xff, 0xff, 0xff, 0x0f, 0x80, 0xff, 0xff, 0x8f, 
	0xff, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0x1f, 0xfc, 0x7f, 0xff, 0x0f, 
	0xff, 0xff, 0xff, 0x1f, 0xfc, 0x7f, 0xff, 0x1f, 0xff, 0xc7, 0xff, 0x10, 0x00, 0x00, 0x07, 0x1f, 
	0xff, 0x83, 0xff, 0x10, 0x02, 0x00, 0x06, 0x3f, 0xff, 0x01, 0xff, 0x1f, 0xff, 0xff, 0xe6, 0x3f, 
	0xfe, 0x11, 0xff, 0x1f, 0xff, 0xff, 0xe4, 0x7f, 0xfc, 0x31, 0xff, 0x1f, 0xff, 0xff, 0xe0, 0x7f, 
	0xf8, 0x71, 0xff, 0x1f, 0xff, 0xff, 0xe0, 0xff, 0xf0, 0xe1, 0xff, 0x1f, 0xff, 0xff, 0xe1, 0xff, 
	0xe0, 0xc0, 0xff, 0x1f, 0xff, 0xff, 0xe3, 0xff, 0xe3, 0x80, 0x3f, 0x1f, 0xff, 0xff, 0xe7, 0xff, 
	0xe0, 0x0c, 0x1f, 0x1f, 0xf8, 0xff, 0xe7, 0xff, 0xe0, 0x0f, 0x06, 0x1f, 0xf0, 0x7f, 0xe7, 0xff, 
	0xf8, 0x07, 0x82, 0x1f, 0xe0, 0x3f, 0xe7, 0xff, 0xff, 0xc7, 0xc0, 0x1f, 0x82, 0x0f, 0xe7, 0xff, 
	0xff, 0xe3, 0xf0, 0x3f, 0x06, 0x07, 0xe7, 0xff, 0xff, 0xe1, 0xf8, 0x3e, 0x1f, 0xc7, 0xe7, 0xff, 
	0xff, 0xf0, 0xfc, 0x3e, 0x18, 0xc7, 0xe7, 0xff, 0xff, 0xf8, 0x7f, 0x3f, 0x08, 0x87, 0xe7, 0xff, 
	0xff, 0xfc, 0x7f, 0x3f, 0x8d, 0x8f, 0xe7, 0xff, 0xff, 0xfc, 0x3e, 0x3f, 0x8f, 0x8f, 0xe7, 0xff, 
	0xff, 0xfe, 0x1e, 0x3f, 0x80, 0x8f, 0xe7, 0xff, 0xff, 0xff, 0x0f, 0x7f, 0x80, 0x0f, 0xe7, 0xff, 
	0xff, 0xff, 0x86, 0x3f, 0xc0, 0x1f, 0xe7, 0xff, 0xff, 0xff, 0xc6, 0x3f, 0xff, 0xff, 0xe7, 0xff, 
	0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xe0, 0x3f, 0xff, 0xff, 0xe7, 0xff, 
	0xff, 0xff, 0xf0, 0x3f, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xe7, 0xff, 
	0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xc7, 0xff, 
	0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x07, 0xff, 
	0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// 'wifi', 64x64px
const unsigned char wifi [] PROGMEM = {
	0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x0f, 0xff, 0xff, 
	0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xfc, 0x00, 0xff, 0xfe, 0x00, 0x3f, 0xff, 
	0xff, 0xf0, 0x0f, 0xff, 0xff, 0xe0, 0x0f, 0xff, 0xff, 0xc0, 0x7f, 0xff, 0xff, 0xfe, 0x03, 0xff, 
	0xff, 0x01, 0xff, 0xff, 0xff, 0xff, 0x80, 0xff, 0xfe, 0x07, 0xff, 0xf8, 0x1f, 0xff, 0xe0, 0x7f, 
	0xf8, 0x1f, 0xfe, 0x00, 0x00, 0x7f, 0xf8, 0x1f, 0xf0, 0x7f, 0xf0, 0x00, 0x00, 0x0f, 0xfc, 0x0f, 
	0xe0, 0xff, 0x80, 0x01, 0x00, 0x01, 0xff, 0x07, 0xc3, 0xfe, 0x00, 0xff, 0xff, 0x00, 0x7f, 0x83, 
	0x07, 0xf8, 0x0f, 0xff, 0xff, 0xe0, 0x3f, 0xc1, 0x0f, 0xf0, 0x3f, 0xff, 0xff, 0xfc, 0x0f, 0xe0, 
	0x87, 0xc0, 0xff, 0xff, 0xff, 0xff, 0x03, 0xe1, 0xc3, 0x83, 0xff, 0xe0, 0x07, 0xff, 0xc1, 0xc3, 
	0xe1, 0x07, 0xfe, 0x00, 0x00, 0x7f, 0xe0, 0x87, 0xf0, 0x1f, 0xf0, 0x00, 0x00, 0x0f, 0xf8, 0x0f, 
	0xf8, 0x3f, 0xc0, 0x0f, 0xf0, 0x03, 0xfc, 0x1f, 0xf8, 0x7f, 0x00, 0xff, 0xff, 0x01, 0xfe, 0x3f, 
	0xfc, 0xfe, 0x07, 0xff, 0xff, 0xe0, 0x7f, 0x7f, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xf8, 0x3f, 0xff, 
	0xff, 0xf0, 0x7f, 0xff, 0xff, 0xfc, 0x1f, 0xff, 0xff, 0xe0, 0xff, 0xc0, 0x03, 0xff, 0x07, 0xff, 
	0xff, 0xe1, 0xfe, 0x00, 0x00, 0xff, 0x87, 0xff, 0xff, 0xe1, 0xf8, 0x00, 0x00, 0x3f, 0x07, 0xff, 
	0xff, 0xf0, 0xf0, 0x1f, 0xf8, 0x0e, 0x0f, 0xff, 0xff, 0xf8, 0x60, 0xff, 0xfe, 0x04, 0x1f, 0xff, 
	0xff, 0xfc, 0x03, 0xff, 0xff, 0x80, 0x3f, 0xff, 0xff, 0xfe, 0x07, 0xff, 0xff, 0xe0, 0x7f, 0xff, 
	0xff, 0xff, 0x0f, 0xfc, 0x7f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xc0, 0x07, 0xf9, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x7f, 0xff, 0xff, 
	0xff, 0xff, 0xfc, 0x0f, 0xf0, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x3f, 0xfc, 0x1f, 0xff, 0xff, 
	0xff, 0xff, 0xfc, 0x3f, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x1f, 0xf8, 0x7f, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x87, 0xe1, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xc3, 0xc3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0x87, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 
	0xff, 0xe0, 0xff, 0xff, 0xff, 0xf9, 0xff, 0x9f, 0xff, 0xcc, 0xcf, 0x3c, 0xe7, 0x99, 0xce, 0x1f, 
	0xff, 0xcf, 0x82, 0x10, 0x43, 0x09, 0x84, 0x1f, 0xff, 0xcf, 0xb2, 0xc3, 0x48, 0x69, 0x21, 0x9f, 
	0xff, 0xcf, 0xba, 0xc3, 0x40, 0x79, 0x01, 0x9f, 0xff, 0xcc, 0x92, 0xc3, 0x4e, 0x69, 0x3d, 0x9f, 
	0xff, 0xe0, 0x86, 0xc3, 0x63, 0x09, 0x84, 0x3f, 0xff, 0xfb, 0xef, 0xff, 0xf7, 0xbf, 0xdf, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

const unsigned char Calendar [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0x83, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xff, 0x01, 0xff, 0xff, 0xc0, 0x7f, 0xff, 
	0xff, 0xff, 0x00, 0xff, 0xff, 0xc0, 0x3f, 0xff, 0xff, 0xff, 0x10, 0xff, 0xff, 0xc0, 0x3f, 0xff, 
	0xff, 0xfe, 0x10, 0xff, 0xff, 0xc4, 0x3f, 0xff, 0xff, 0x00, 0x10, 0x00, 0x00, 0x04, 0x00, 0x3f, 
	0xfe, 0x00, 0x10, 0x00, 0x00, 0x04, 0x00, 0x3f, 0xfe, 0x00, 0x10, 0x00, 0x00, 0x04, 0x00, 0x3f, 
	0xfe, 0x1f, 0x10, 0xff, 0xff, 0xc4, 0x3e, 0x3f, 0xfe, 0x1f, 0x00, 0xff, 0xff, 0xc0, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x00, 0xff, 0xff, 0xc0, 0x3e, 0x3f, 0xfe, 0x1f, 0x01, 0xff, 0xff, 0xc0, 0x7e, 0x3f, 
	0xfe, 0x1f, 0x83, 0xff, 0xff, 0xf0, 0xfe, 0x3f, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 
	0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 
	0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 
	0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 
	0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 
	0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x3f, 0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 0xfe, 0x1f, 0x00, 0x10, 0x00, 0x00, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 0xfe, 0x1f, 0x1e, 0x1e, 0x3e, 0x3c, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x0a, 0x16, 0x14, 0x34, 0x3e, 0x3f, 0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 
	0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 0xfe, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x3f, 
	0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 
	0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3f, 0xfe, 0x10, 0x00, 0x00, 0x00, 0x00, 0x02, 0x3f, 
	0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 
	0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

void setup() {
  // Start the serial connection
  Serial.begin(115200);

  // Wait for the serial monitor to open
  while (!Serial);

  Serial.println("Connecting to Adafruit IO...");

  // Connect to Adafruit IO via MQTT
  io.connect();



  // Set up a message handler for the 'zap1' feed
  zap1->onMessage(handleZap1Message);
  zap2->onMessage(handleZap2Message);

  // Initialize DHT sensor
  dht.begin();

  Serial.println("Connected to Adafruit IO!");

  // Initialize display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
}
 int centerX = (SCREEN_WIDTH - 64) / 2; // Calculate X coordinate for centering

void loop() {
  // Keep the client connected to Adafruit IO and process incoming data
  io.run();

  // Read sensor data
  float temperature = dht.readTemperature(); // Read temperature
  float humidity = dht.readHumidity();       // Read humidity
  int soilMoistureValue = analogRead(soilMoisturePin); // Read soil moisture value

  // Print sensor readings to serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Soil Moisture: ");
  Serial.println(soilMoistureValue);

  // Check conditions for plant happiness
  if (temperature >= minTemperature && temperature <= maxTemperature &&
      humidity >= minHumidity && humidity <= maxHumidity &&
      soilMoistureValue >= minSoilMoisture) {
   
        display.clearDisplay();
        display.drawBitmap(centerX, 0,happy, 64, 64, SSD1306_WHITE); // Draw the calendar symbol
        display.display();
        delay(5000);
      display.clearDisplay();

  } else {
      
    if (temperature < minTemperature || temperature > maxTemperature) {


      display.clearDisplay();
      display.drawBitmap(centerX, 0,sad, 64, 64, SSD1306_WHITE); // Draw the calendar symbol
      display.display(); 
      delay(5000);
      display.clearDisplay();
   
       display.clearDisplay();
       display.drawBitmap(centerX, 0,sun, 64, 64, SSD1306_WHITE); // Draw the calendar symbol
        display.display();  
      delay(5000);
      display.clearDisplay();


    } else if (humidity < minHumidity || humidity > maxHumidity) {


      display.clearDisplay();
      display.drawBitmap(centerX, 0,sad, 64, 64, SSD1306_WHITE); // Draw the calendar symbol
      display.display();    
      delay(5000);
      display.clearDisplay();
    
        display.clearDisplay();
        display.drawBitmap(centerX, 0, thermometer, 64, 64, SSD1306_WHITE); // Draw the calendar symbol
        display.display();
      delay(5000);
      display.clearDisplay();


    } else if (soilMoisture < minSoilMoisture) {


      
      display.clearDisplay();
      display.drawBitmap(centerX, 0,sad, 64, 64, SSD1306_WHITE); // Draw the calendar symbol
      display.display();    
      delay(5000);
      delay(5000);
      display.clearDisplay();



     
    
        display.clearDisplay();
        display.drawBitmap(centerX, 0,water, 64, 64, SSD1306_WHITE); // Draw the calendar symbol
        display.display();
        delay(5000);
         display.clearDisplay();
        
    } 
  }

  delay(3000); // Delay before taking next readings
}

// Message handler for the 'zap1' feed
void handleZap1Message(AdafruitIO_Data *data) {
  // 'Calendar', 64x64px

  // Print the received value to the serial monitor
  Serial.print("Received value from 'zap1': ");
  Serial.println(data->value());

  // Check if the received value is "ON"
  if (strcmp(data->value(), "1") == 0) {

    Serial.println("Calendar Updated");
    display.clearDisplay();
    display.drawBitmap(centerX, 0,sad, 64, 64, SSD1306_WHITE); // Draw the calendar symbol
    display.display();    
    delay(5000);
    display.clearDisplay();
    
    
  } 
}


// Message handler for the 'zap2' feed
void handleZap2Message(AdafruitIO_Data *data) {
  // Print the received value to the serial monitor
  //mail

  Serial.print("Received value from 'zap2': ");
  Serial.println(data->value());

  // Check if the received value is "2"
  if (strcmp(data->value(), "2") == 0) {
      Serial.println("Received mail");
      display.clearDisplay();
      display.drawBitmap(centerX, 0,sad, 64, 64, SSD1306_WHITE); // Draw the calendar symbol
      display.display();    
      delay(5000);
      display.clearDisplay();

  
  } 
}




