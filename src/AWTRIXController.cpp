#include <FS.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>
#include <Fonts/TomThumb.h>
#include <LightDependentResistor.h>
#include <Wire.h>
#include <SparkFun_APDS9960.h>
#include "SoftwareSerial.h"
#include <DFPlayerMini_Fast.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <DoubleResetDetect.h>
#include <Wire.h>
#include <BME280_t.h>
#include "Adafruit_HTU21DF.h"
#include <WiFiUdp.h>

// instantiate temp sensor
BME280<> BMESensor;
Adafruit_HTU21DF htu = Adafruit_HTU21DF();

int tempState = false;	// 0 = None ; 1 = BME280 ; 2 = htu21d
int audioState = false;   // 0 = false ; 1 = true
int gestureState = false; // 0 = false ; 1 = true
int ldrState = false;	 // 0 = None
int usbWifiState = false; // true = usb...
int pairingState = 0;	 //0 = not paired ; 1 = paired

String version = "0.9b";
char awtrix_server[16];
//int ID = 0;

IPAddress Server;

WiFiClient espClient;
PubSubClient client(espClient);

WiFiManager wifiManager;
//USP
WiFiUDP Udp;
unsigned int localUdpPort = 4210;
char incomingPacket[20];

//resetdetector
#define DRD_TIMEOUT 5.0
#define DRD_ADDRESS 0x00
DoubleResetDetect drd(DRD_TIMEOUT, DRD_ADDRESS);

bool firstStart = true;
int myTime;  //need for loop
int myTime2; //need for loop
int myCounter;
int myCounter2;
int TIME_FOR_SEARCHING_WIFI = 10000;

//USB Connection:
byte myBytes[1000];
unsigned int bufferpointer;

//Zum speichern...
int cfgStart = 0;

//flag for saving data
bool shouldSaveConfig = false;

/// LDR Config
#define LDR_RESISTOR 1000 //ohms
#define LDR_PIN A0
#define LDR_PHOTOCELL LightDependentResistor::GL5516
LightDependentResistor photocell(LDR_PIN, LDR_RESISTOR, LDR_PHOTOCELL);

// Gesture Sensor
#define APDS9960_INT D6
#define I2C_SDA D3
#define I2C_SCL D1
SparkFun_APDS9960 apds = SparkFun_APDS9960();
volatile bool isr_flag = 0;

#ifndef ICACHE_RAM_ATTR
#define ICACHE_RAM_ATTR IRAM_ATTR
#endif

bool updating = false;

// Audio
DFPlayerMini_Fast myMP3;
SoftwareSerial mySoftwareSerial(D7, D5); // RX, TX

// Matrix Settings
CRGB leds[256];
#ifdef MATRIX_MODEV2
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, 32, 8, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);
#else
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, 32, 8, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);
#endif

static byte c1; // Last character buffer
byte utf8ascii(byte ascii)
{
	if (ascii < 128) // Standard ASCII-set 0..0x7F handling
	{
		c1 = 0;
		return (ascii);
	}
	// get previous input
	byte last = c1; // get last char
	c1 = ascii;		// remember actual character
	switch (last)   // conversion depending on first UTF8-character
	{
	case 0xC2:
		return (ascii)-34;
		break;
	case 0xC3:
		return (ascii | 0xC0) - 34;
		break;
	case 0x82:
		if (ascii == 0xAC)
			return (0xEA);
	}
	return (0);
}

bool saveConfig()
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject &json = jsonBuffer.createObject();
	json["awtrix_server"] = awtrix_server;

	json["temp"] = tempState;
	json["usbWifi"] = usbWifiState;
	json["ldr"] = ldrState;
	json["gesture"] = gestureState;
	json["audio"] = audioState;

	json["paired"] = pairingState;

	File configFile = SPIFFS.open("/config.json", "w");
	if (!configFile)
	{
		Serial.println("failed to open config file for writing");
		return false;
	}
	json.printTo(Serial);
	json.printTo(configFile);
	configFile.close();
	//end save
	return true;
}

void debuggingWithMatrix(String text)
{
	matrix->setCursor(7, 6);
	matrix->clear();
	matrix->print(text);
	matrix->show();
}

String utf8ascii(String s)
{
	String r = "";
	char c;
	for (int i = 0; i < s.length(); i++)
	{
		c = utf8ascii(s.charAt(i));
		if (c != 0)
			r += c;
	}
	return r;
}

void hardwareAnimatedUncheck(int typ, int x, int y)
{
	int wifiCheckTime = millis();
	int wifiCheckPoints = 0;
	while (millis() - wifiCheckTime < 2000)
	{
		while (wifiCheckPoints < 10)
		{
			matrix->clear();
			switch (typ)
			{
			case 0:
				matrix->setCursor(7, 6);
				matrix->print("WiFi");
				break;
			case 1:
				matrix->setCursor(1, 6);
				matrix->print("Server");
				break;
			case 2:
				matrix->setCursor(7, 6);
				matrix->print("Temp");
				break;
			}

			switch (wifiCheckPoints)
			{
			case 9:
				matrix->drawPixel(x, y + 4, 0xF800);
			case 8:
				matrix->drawPixel(x - 1, y + 3, 0xF800);
			case 7:
				matrix->drawPixel(x - 2, y + 2, 0xF800);
			case 6:
				matrix->drawPixel(x - 3, y + 1, 0xF800);
			case 5:
				matrix->drawPixel(x - 4, y, 0xF800);
			case 4:
				matrix->drawPixel(x - 4, y + 4, 0xF800);
			case 3:
				matrix->drawPixel(x - 3, y + 3, 0xF800);
			case 2:
				matrix->drawPixel(x - 2, y + 2, 0xF800);
			case 1:
				matrix->drawPixel(x - 1, y + 1, 0xF800);
			case 0:
				matrix->drawPixel(x, y, 0xF800);
				break;
			}
			wifiCheckPoints++;
			matrix->show();
			delay(100);
		}
	}
}

void hardwareAnimatedCheck(int typ, int x, int y)
{
	int wifiCheckTime = millis();
	int wifiCheckPoints = 0;
	while (millis() - wifiCheckTime < 2000)
	{
		while (wifiCheckPoints < 7)
		{
			matrix->clear();
			switch (typ)
			{
			case 0:
				matrix->setCursor(7, 6);
				matrix->print("WiFi");
				break;
			case 1:
				matrix->setCursor(1, 6);
				matrix->print("Server");
				break;
			case 2:
				matrix->setCursor(7, 6);
				matrix->print("Temp");
				break;
			case 3:
				matrix->setCursor(3, 6);
				matrix->print("Audio");
				break;
			case 4:
				matrix->setCursor(3, 6);
				matrix->print("Gest.");
				break;
			case 5:
				matrix->setCursor(7, 6);
				matrix->print("LDR");
				break;
			}

			switch (wifiCheckPoints)
			{
			case 6:
				matrix->drawPixel(x, y, 0x07E0);
			case 5:
				matrix->drawPixel(x - 1, y + 1, 0x07E0);
			case 4:
				matrix->drawPixel(x - 2, y + 2, 0x07E0);
			case 3:
				matrix->drawPixel(x - 3, y + 3, 0x07E0);
			case 2:
				matrix->drawPixel(x - 4, y + 4, 0x07E0);
			case 1:
				matrix->drawPixel(x - 5, y + 3, 0x07E0);
			case 0:
				matrix->drawPixel(x - 6, y + 2, 0x07E0);
				break;
			}
			wifiCheckPoints++;
			matrix->show();
			delay(100);
		}
	}
}

void hardwareAnimatedSearchFast(int rounds, int x, int y)
{
	matrix->clear();
	matrix->setTextColor(0xFFFF);
	matrix->setCursor(1, 6);
	matrix->print("Server");

	switch (rounds)
	{
	case 3:
		matrix->drawPixel(x, y, 0xFFFF);
		matrix->drawPixel(x + 1, y + 1, 0xFFFF);
		matrix->drawPixel(x + 2, y + 2, 0xFFFF);
		matrix->drawPixel(x + 3, y + 3, 0xFFFF);
		matrix->drawPixel(x + 2, y + 4, 0xFFFF);
		matrix->drawPixel(x + 1, y + 5, 0xFFFF);
		matrix->drawPixel(x, y + 6, 0xFFFF);
	case 2:
		matrix->drawPixel(x - 1, y + 2, 0xFFFF);
		matrix->drawPixel(x, y + 3, 0xFFFF);
		matrix->drawPixel(x - 1, y + 4, 0xFFFF);
	case 1:
		matrix->drawPixel(x - 3, y + 3, 0xFFFF);
	case 0:
		break;
	}
	matrix->show();
}

void hardwareAnimatedSearch(int typ, int x, int y)
{
	for (int i = 0; i < 4; i++)
	{
		matrix->clear();
		matrix->setTextColor(0xFFFF);
		if (typ == 0)
		{
			matrix->setCursor(7, 6);
			matrix->print("WiFi");
		}
		else if (typ == 1)
		{
			matrix->setCursor(1, 6);
			matrix->print("Server");
		}
		switch (i)
		{
		case 3:
			matrix->drawPixel(x, y, 0x22ff);
			matrix->drawPixel(x + 1, y + 1, 0x22ff);
			matrix->drawPixel(x + 2, y + 2, 0x22ff);
			matrix->drawPixel(x + 3, y + 3, 0x22ff);
			matrix->drawPixel(x + 2, y + 4, 0x22ff);
			matrix->drawPixel(x + 1, y + 5, 0x22ff);
			matrix->drawPixel(x, y + 6, 0x22ff);
		case 2:
			matrix->drawPixel(x - 1, y + 2, 0x22ff);
			matrix->drawPixel(x, y + 3, 0x22ff);
			matrix->drawPixel(x - 1, y + 4, 0x22ff);
		case 1:
			matrix->drawPixel(x - 3, y + 3, 0x22ff);
		case 0:
			break;
		}
		matrix->show();
		delay(500);
	}
}

void utf8ascii(char *s)
{
	int k = 0;
	char c;
	for (int i = 0; i < strlen(s); i++)
	{
		c = utf8ascii(s[i]);
		if (c != 0)
			s[k++] = c;
	}
	s[k] = 0;
}

String GetChipID()
{
	return String(ESP.getChipId());
}

int GetRSSIasQuality(int rssi)
{
	int quality = 0;

	if (rssi <= -100)
	{
		quality = 0;
	}
	else if (rssi >= -50)
	{
		quality = 100;
	}
	else
	{
		quality = 2 * (rssi + 100);
	}
	return quality;
}

unsigned long startTime = 0;
unsigned long endTime = 0;
unsigned long duration;

void updateMatrix(byte payload[], int length)
{
	int y_offset = 5;

	if (firstStart)
	{
		hardwareAnimatedCheck(1, 30, 2);
		firstStart = false;
	}

	switch (payload[0])
	{
	case 0:
	{
		//Command 0: DrawText

		//Prepare the coordinates
		uint16_t x_coordinate = int(payload[1] << 8) + int(payload[2]);
		uint16_t y_coordinate = int(payload[3] << 8) + int(payload[4]);

		//Serial.printf("X: %d - Y: %d\n",x_coordinate,y_coordinate);

		matrix->setCursor(x_coordinate + 1, y_coordinate + y_offset);
		matrix->setTextColor(matrix->Color(payload[5], payload[6], payload[7]));

		String myText = "";
		for (int i = 8; i < length; i++)
		{
			char c = payload[i];
			myText += c;
		}
		//Serial.printf("Text: %s\n",myText.c_str());
		matrix->print(utf8ascii(myText));
		break;
	}
	case 1:
	{
		//Command 1: DrawBMP

		//Prepare the coordinates
		uint16_t x_coordinate = int(payload[1] << 8) + int(payload[2]);
		uint16_t y_coordinate = int(payload[3] << 8) + int(payload[4]);

		int16_t width = payload[5];
		int16_t height = payload[6];

		unsigned short colorData[width * height];

		for (int i = 0; i < width * height * 2; i++)
		{
			colorData[i / 2] = (payload[i + 7] << 8) + payload[i + 1 + 7];
			i++;
		}

		for (int16_t j = 0; j < height; j++, y_coordinate++)
		{
			for (int16_t i = 0; i < width; i++)
			{
				matrix->drawPixel(x_coordinate + i, y_coordinate, (uint16_t)colorData[j * width + i]);
			}
		}
		break;
	}

	case 2:
	{
		//Command 2: DrawCircle

		//Prepare the coordinates
		uint16_t x0_coordinate = int(payload[1] << 8) + int(payload[2]);
		uint16_t y0_coordinate = int(payload[3] << 8) + int(payload[4]);
		uint16_t radius = payload[5];
		matrix->drawCircle(x0_coordinate, y0_coordinate, radius, matrix->Color(payload[6], payload[7], payload[8]));
		break;
	}
	case 3:
	{
		//Command 3: FillCircle

		//Prepare the coordinates
		uint16_t x0_coordinate = int(payload[1] << 8) + int(payload[2]);
		uint16_t y0_coordinate = int(payload[3] << 8) + int(payload[4]);
		uint16_t radius = payload[5];
		matrix->fillCircle(x0_coordinate, y0_coordinate, radius, matrix->Color(payload[6], payload[7], payload[8]));
		break;
	}
	case 4:
	{
		//Command 4: DrawPixel

		//Prepare the coordinates
		uint16_t x0_coordinate = int(payload[1] << 8) + int(payload[2]);
		uint16_t y0_coordinate = int(payload[3] << 8) + int(payload[4]);
		matrix->drawPixel(x0_coordinate, y0_coordinate, matrix->Color(payload[5], payload[6], payload[7]));
		break;
	}
	case 5:
	{
		//Command 5: DrawRect

		//Prepare the coordinates
		uint16_t x0_coordinate = int(payload[1] << 8) + int(payload[2]);
		uint16_t y0_coordinate = int(payload[3] << 8) + int(payload[4]);
		int16_t width = payload[5];
		int16_t height = payload[6];

		matrix->drawRect(x0_coordinate, y0_coordinate, width, height, matrix->Color(payload[7], payload[8], payload[9]));
		break;
	}
	case 6:
	{
		//Command 6: DrawLine

		//Prepare the coordinates
		uint16_t x0_coordinate = int(payload[1] << 8) + int(payload[2]);
		uint16_t y0_coordinate = int(payload[3] << 8) + int(payload[4]);
		uint16_t x1_coordinate = int(payload[5] << 8) + int(payload[6]);
		uint16_t y1_coordinate = int(payload[7] << 8) + int(payload[8]);
		matrix->drawLine(x0_coordinate, y0_coordinate, x1_coordinate, y1_coordinate, matrix->Color(payload[9], payload[10], payload[11]));
		break;
	}

	case 7:
	{
		//Command 7: FillMatrix

		matrix->fillScreen(matrix->Color(payload[1], payload[2], payload[3]));
		break;
	}

	case 8:
	{
		//Command 8: Show
		matrix->show();
		break;
	}
	case 9:
	{
		//Command 9: Clear
		matrix->clear();
		break;
	}
	case 10:
	{
		//Command 10: Play
		myMP3.volume(payload[3]);
		delay(10);
		myMP3.playFolder(payload[1], payload[2]);
		break;
	}
	case 11:
	{
		//Command 11: reset
		ESP.reset();
		break;
	}
	case 12:
	{
		//Command 12: GetMatrixInfo
		StaticJsonBuffer<400> jsonBuffer;
		JsonObject &root = jsonBuffer.createObject();
		root["type"] = "MatrixInfo";
		root["version"] = version;
		root["wifirssi"] = String(WiFi.RSSI());
		root["wifiquality"] = GetRSSIasQuality(WiFi.RSSI());
		root["wifissid"] = WiFi.SSID();
		root["IP"] = WiFi.localIP().toString();
		root["LUX"] = photocell.getCurrentLux();
		BMESensor.refresh();
		if (tempState == 1)
		{
			root["Temp"] = BMESensor.temperature;
			root["Hum"] = BMESensor.humidity;
			root["hPa"] = BMESensor.pressure;
		}
		else if (tempState == 2)
		{
			root["Temp"] = htu.readTemperature();
			root["Hum"] = htu.readHumidity();
			root["hPa"] = NULL;
		}
		else
		{
			root["Temp"] = NULL;
			root["Hum"] = NULL;
			root["hPa"] = NULL;
		}

		String JS;
		root.printTo(JS);
		if (!usbWifiState)
		{
			client.publish("matrixClient", JS.c_str());
		}
		else
		{
			Serial.println(String(JS));
		}
		break;
	}
	case 13:
	{
		matrix->setBrightness(payload[1]);
		break;
	}

	case 14:
	{
		wifiManager.resetSettings();
		ESP.reset();
	}
	}
}

void callback(char *topic, byte *payload, unsigned int length)
{
	updateMatrix(payload, length);
}

void reconnect()
{
	if (!usbWifiState)
	{
		while (!client.connected())
		{
			String clientId = "AWTRIXController-";
			clientId += String(random(0xffff), HEX);
			hardwareAnimatedSearch(1, 28, 0);
			if (client.connect(clientId.c_str()))
			{
				client.subscribe("awtrixmatrix/#");
				client.publish("matrixClient", "connected");
			}
		}
	}
}

void ICACHE_RAM_ATTR interruptRoutine()
{
	isr_flag = 1;
}

void handleGesture()
{
	String control;
	if (apds.isGestureAvailable())
	{
		switch (apds.readGesture())
		{
		case DIR_UP:
			control = "UP";
			break;
		case DIR_DOWN:
			control = "DOWN";
			break;
		case DIR_LEFT:
			control = "LEFT";
			break;
		case DIR_RIGHT:
			control = "RIGHT";
			break;
		case DIR_NEAR:
			control = "NEAR";
			break;
		case DIR_FAR:
			control = "FAR";
			break;
		default:
			control = "NONE";
		}
		StaticJsonBuffer<200> jsonBuffer;
		JsonObject &root = jsonBuffer.createObject();
		root["type"] = "gesture";
		root["gesture"] = control;
		String JS;
		root.printTo(JS);

#ifdef USB_CONNECTION
		Serial.println(String(JS));
#else
		client.publish("matrixClient", control.c_str());
#endif
	}
}

uint32_t Wheel(byte WheelPos, int pos)
{
	if (WheelPos < 85)
	{
		return matrix->Color((WheelPos * 3) - pos, (255 - WheelPos * 3) - pos, 0);
	}
	else if (WheelPos < 170)
	{
		WheelPos -= 85;
		return matrix->Color((255 - WheelPos * 3) - pos, 0, (WheelPos * 3) - pos);
	}
	else
	{
		WheelPos -= 170;
		return matrix->Color(0, (WheelPos * 3) - pos, (255 - WheelPos * 3) - pos);
	}
}

void flashProgress(unsigned int progress, unsigned int total)
{
	matrix->setBrightness(100);
	long num = 32 * 8 * progress / total;
	for (unsigned char y = 0; y < 8; y++)
	{
		for (unsigned char x = 0; x < 32; x++)
		{
			if (num-- > 0)
				matrix->drawPixel(x, 8 - y - 1, Wheel((num * 16) & 255, 0));
		}
	}
	matrix->setCursor(0, 6);
	matrix->setTextColor(matrix->Color(255, 255, 255));
	matrix->print("FLASHING");
	matrix->show();
}

void saveConfigCallback()
{
	Serial.println("Should save config");
	shouldSaveConfig = true;
}

void configModeCallback(WiFiManager *myWiFiManager)
{
	Serial.println("Entered config mode");
	Serial.println(WiFi.softAPIP());
	Serial.println(myWiFiManager->getConfigPortalSSID());
}

void setup()
{
	delay(2000);
	FastLED.addLeds<NEOPIXEL, D2>(leds, 256).setCorrection(TypicalLEDStrip);
	matrix->begin();
	matrix->setTextWrap(false);
	matrix->setBrightness(80);
	matrix->setFont(&TomThumb);

	if (drd.detect())
	{
		Serial.println("** Double reset boot **");
		matrix->clear();
		matrix->setTextColor(matrix->Color(255, 0, 0));
		matrix->setCursor(6, 6);
		matrix->print("RESET!");
		matrix->show();
		delay(3000);
		wifiManager.resetSettings();
		if (SPIFFS.begin())
		{
			SPIFFS.remove("/config.json");
			SPIFFS.end();
		}
	}

	wifiManager.setAPStaticIPConfig(IPAddress(172, 217, 28, 1), IPAddress(172, 217, 28, 1), IPAddress(255, 255, 255, 0));

	Serial.setRxBufferSize(1024);
	Serial.begin(115200);

	if (SPIFFS.begin())
	{
		//if file not exists
		if (!(SPIFFS.exists("/config.json")))
		{
			SPIFFS.open("/config.json", "w+");
			Serial.println("make File...");
		}

		File configFile = SPIFFS.open("/config.json", "r");
		if (configFile)
		{
			size_t size = configFile.size();
			// Allocate a buffer to store contents of the file.
			std::unique_ptr<char[]> buf(new char[size]);
			configFile.readBytes(buf.get(), size);
			DynamicJsonBuffer jsonBuffer;
			JsonObject &json = jsonBuffer.parseObject(buf.get());
			json.printTo(Serial);
			if (json.success())
			{
				Serial.println("\nparsed json");
				String temporaer = json["awtrix_server"];
				for (int i = 0; i < 16; i++)
				{
					awtrix_server[i] = temporaer[i];
				}
				usbWifiState = json["connection"].as<int>();
				audioState = json["audio"].as<int>();
				gestureState = json["gesture"].as<int>();
				ldrState = json["ldr"].as<int>();
				tempState = json["temp"].as<int>();
				pairingState = json["paired"].as<int>();
			}
			configFile.close();
		}
	}
	else
	{
		Serial.println("mounting not possible");
	}

	Serial.println("Loading from SPIFFS:");
	Serial.println(awtrix_server);
	if (usbWifiState)
	{
		Serial.println("Connection: USB");
	}
	else
	{
		Serial.println("Connection: WiFi");
	}
	if (audioState)
	{
		Serial.println("Audio: true");
	}
	else
	{
		Serial.println("Audio: false");
	}
	if (gestureState)
	{
		Serial.println("Gesture: true");
	}
	else
	{
		Serial.println("Gesture: false");
	}
	switch (tempState)
	{
	case 0:
		Serial.println("Temp: None");
		break;
	case 1:
		Serial.println("Temp: BME280");
		break;
	case 2:
		Serial.println("Temp: htu21d");
		break;
	}
	Serial.printf("LDR: %d\n", ldrState);

	wifiManager.setTimeout(1);
	wifiManager.autoConnect("Awtrix Controller", "awtrixxx");
	wifiManager.setTimeout(0);
	wifiManager.setSaveConfigCallback(saveConfigCallback);

	int wifiTimeout = millis();

	while (WiFi.status() != WL_CONNECTED)
	{
		hardwareAnimatedSearch(0, 24, 0);

		if (millis() - wifiTimeout > TIME_FOR_SEARCHING_WIFI)
		{
			matrix->clear();
			matrix->setCursor(3, 6);
			matrix->setTextColor(matrix->Color(0, 255, 50));
			matrix->print("Hotspot");
			matrix->show();
			while (WiFi.status() != WL_CONNECTED)
			{
				wifiManager.autoConnect("AWTRIX Controller", "awtrixxx");
			}
		}
	}

	Udp.begin(localUdpPort);

	if (shouldSaveConfig)
	{
		Serial.println("saving config");
		saveConfig();
		ESP.reset();
	}

	hardwareAnimatedCheck(0, 27, 2);

	//for testing...
	//tempState = 1;
	//audioState= true;
	//gestureState = true;
	//ldrState = 1000;

	//Checking periphery
	Wire.begin(I2C_SDA, I2C_SCL);
	if (tempState == 1)
	{
		if (BMESensor.begin())
		{
			//temp OK
			hardwareAnimatedCheck(2, 29, 2);
		}
		else
		{
			//temp NOK
			hardwareAnimatedUncheck(2, 27, 1);
		}
	}
	else if (tempState == 2)
	{
		if(htu.begin()){
			hardwareAnimatedCheck(2, 29, 2);
		} else {
			hardwareAnimatedUncheck(2, 27, 1);
		}
	}

	if (audioState)
	{
		mySoftwareSerial.begin(9600);
		myMP3.begin(mySoftwareSerial);
		hardwareAnimatedCheck(3, 29, 2);
	}
	if (gestureState)
	{
		pinMode(APDS9960_INT, INPUT);
		attachInterrupt(APDS9960_INT, interruptRoutine, FALLING);
		apds.init();
		apds.enableGestureSensor(true);
		hardwareAnimatedCheck(4, 29, 2);
	}
	if (ldrState)
	{
		photocell.setPhotocellPositionOnGround(false);
		hardwareAnimatedCheck(5, 29, 2);
	}

	ArduinoOTA.onStart([&]() {
		updating = true;
		matrix->clear();
	});

	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		flashProgress(progress, total);
	});

	ArduinoOTA.begin();
	matrix->clear();
	matrix->setCursor(7, 6);

	bufferpointer = 0;

	myTime = millis() - 500;
	myTime2 = millis() - 1000;
	myCounter = 0;
	myCounter2 = 0;

	client.setServer(awtrix_server, 7001);
	client.setCallback(callback);
}

void loop()
{
	ArduinoOTA.handle();
	while (pairingState == 0)
	{
		if (millis() - myTime2 > 1000)
		{
			switch (myCounter2)
			{
			case 0:
				matrix->clear();
				matrix->setCursor(3, 6);
				matrix->print("Need");
				matrix->show();
				Serial.println("[Pairing] Need");
				break;
			case 1:
				matrix->clear();
				matrix->setCursor(3, 6);
				matrix->print("pairing");
				matrix->show();
				Serial.println("[Pairing] pairing");
				break;
			case 2:
				matrix->clear();
				matrix->setCursor(3, 6);
				matrix->print("from");
				matrix->show();
				Serial.println("[Pairing] from");
				break;
			case 3:
				matrix->clear();
				matrix->setCursor(3, 6);
				matrix->print("Server");
				matrix->show();
				Serial.println("[Pairing] Server");
				break;
			}
			myCounter2++;
			if (myCounter2 == 4)
			{
				myCounter2 = 0;
			}
			myTime2 = millis();
		}

		int packetSize = Udp.parsePacket();
		if (packetSize)
		{
			// receive incoming UDP packets
			Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
			int len = Udp.read(incomingPacket, 255);
			if (len > 0)
			{
				incomingPacket[len] = 0;
			}

			Serial.println("Got data via UDP!");

			usbWifiState = (int)incomingPacket[0];
			tempState = (int)incomingPacket[1];
			audioState = (int)incomingPacket[2];
			gestureState = (int)incomingPacket[3];
			ldrState = int(incomingPacket[4] << 8) + int(incomingPacket[5]);

			IPAddress ip = IPAddress(incomingPacket[6], incomingPacket[7], incomingPacket[8], incomingPacket[9]);
			ip.toString().toCharArray(awtrix_server, 16);

			if ((int)incomingPacket[10] == 255 && (int)incomingPacket[11] == 255 && (int)incomingPacket[12] == 255)
			{
				matrix->clear();
				matrix->setCursor(6, 6);
				matrix->setTextColor(matrix->Color(0, 255, 50));
				matrix->print("PAIRED!");
				matrix->show();
				delay(3000);
				pairingState = 1;
				if (saveConfig())
				{
					ESP.reset();
				}
				else
				{
					Serial.println("[UpdateMatrix UDP] Fail to Save the File...");
				}
			}
			else
			{
				Serial.println("[UDP Enddelimitter] Not the right delimitter...");
			}
		}
	}

	if (firstStart)
	{
		if (millis() - myTime > 500)
		{
			hardwareAnimatedSearchFast(myCounter, 28, 0);
			myCounter++;
			if (myCounter == 4)
			{
				myCounter = 0;
			}
			myTime = millis();
		}
	}

	if (!updating)
	{
		if (usbWifiState)
		{
			while (Serial.available() > 0)
			{
				myBytes[bufferpointer] = Serial.read();
				if ((myBytes[bufferpointer] == 255) && (myBytes[bufferpointer - 1] == 255) && (myBytes[bufferpointer - 2] == 255))
				{
					updateMatrix(myBytes, bufferpointer);
					for (int i = 0; i < bufferpointer; i++)
					{
						myBytes[i] = 0;
					}
					bufferpointer = 0;
					break;
				}
				else
				{
					bufferpointer++;
				}
				if (bufferpointer == 1000)
				{
					bufferpointer = 0;
				}
			}
		}
		else
		{
			if (!client.connected())
			{
				reconnect();
			}
			else
			{
				client.loop();
			}
		}
		if (isr_flag == 1)
		{
			detachInterrupt(APDS9960_INT);
			handleGesture();
			isr_flag = 0;
			attachInterrupt(APDS9960_INT, interruptRoutine, FALLING);
		}
	}
}