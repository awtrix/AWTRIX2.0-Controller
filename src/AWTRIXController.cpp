#include <FS.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>     // Replace with WebServer.h for ESP32
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
#include "awtrix-conf.h"
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <DoubleResetDetect.h>
#include <Wire.h>   

#include <BME280_t.h>

//#include "BMP280.h"

// instantiate BME sensor
BME280<> BMESensor;
//BMP280 BMESensor;

bool tempState = false;

String version = "0.9b";
char awtrix_server[16];
char connection[5];

WiFiClient espClient;
PubSubClient client(espClient);

//resetdetector
#define DRD_TIMEOUT 2.0
#define DRD_ADDRESS 0x00
DoubleResetDetect drd(DRD_TIMEOUT, DRD_ADDRESS);

bool firstStart = true;
int myTime;
int myCounter;
int TIME_FOR_SEARCHING_WIFI = 10000;

//USB Connection:
byte myBytes[1000];
unsigned int bufferpointer;

//Zum speichern...
int cfgStart = 0;

//flag for saving data
bool shouldSaveConfig = false;
char mqtt_server[40];
char mqtt_port[6] = "8080";
char blynk_token[33] = "YOUR_BLYNK_TOKEN";


LightDependentResistor photocell(LDR_PIN, LDR_RESISTOR, LDR_PHOTOCELL);
#define APDS9960_INT    D6
#define APDS9960_SDA    D3
#define APDS9960_SCL    D1
SparkFun_APDS9960 apds = SparkFun_APDS9960();
volatile bool isr_flag = 0;

#ifndef ICACHE_RAM_ATTR
#define ICACHE_RAM_ATTR IRAM_ATTR
#endif
bool updating = false;
DFPlayerMini_Fast myMP3;

SoftwareSerial mySoftwareSerial(D7, D5); // RX, TX

//SoftwareSerial mySoftwareSerial(D5, D4); // RX, TX

CRGB leds[256];
#ifdef MATRIX_MODEV2
  FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, 32, 8, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);
#else
  FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, 32, 8, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);
#endif

//Hotspot
//ESP8266WebServer Server;
//AutoConnect      Portal(Server);

static byte c1;  // Last character buffer
byte utf8ascii(byte ascii) {
	if ( ascii < 128 ) // Standard ASCII-set 0..0x7F handling
	{ c1 = 0;
		return ( ascii );
	}
	// get previous input
	byte last = c1;   // get last char
	c1 = ascii;       // remember actual character
	switch (last)     // conversion depending on first UTF8-character
	{ case 0xC2: return  (ascii) - 34;  break;
		case 0xC3: return  (ascii | 0xC0) - 34;  break;
		case 0x82: if (ascii == 0xAC) return (0xEA);
	}
	return  (0);
}

void debuggingWithMatrix(String text){
	matrix->setCursor(7, 6);
	matrix->clear();
	matrix->print(text);
	matrix->show();
}



String utf8ascii(String s) {
  String r = "";
  char c;
  for (int i = 0; i < s.length(); i++){
    c = utf8ascii(s.charAt(i));
    if (c != 0) r += c;
  }
  return r;
}

void hardwareAnimatedUncheck(int typ,int x,int y){
	int wifiCheckTime = millis();
	int wifiCheckPoints = 0;
	while(millis()-wifiCheckTime<2000){
		while(wifiCheckPoints<10){
			matrix->clear();
			switch(typ){
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

			switch(wifiCheckPoints){
				case 9:
					matrix->drawPixel(x,y+4,0xF800);
				case 8:
					matrix->drawPixel(x-1,y+3,0xF800);
				case 7:
					matrix->drawPixel(x-2,y+2,0xF800);
				case 6:
					matrix->drawPixel(x-3,y+1,0xF800);
				case 5:
					matrix->drawPixel(x-4,y,0xF800);
				case 4:
					matrix->drawPixel(x-4,y+4,0xF800);
				case 3:
					matrix->drawPixel(x-3,y+3,0xF800);
				case 2:
					matrix->drawPixel(x-2,y+2,0xF800);
				case 1:
					matrix->drawPixel(x-1,y+1,0xF800);
				case 0:
					matrix->drawPixel(x,y,0xF800);
				break;
				}
			wifiCheckPoints++;
			matrix->show();
			delay(100);
		}
	}
}

void hardwareAnimatedCheck(int typ,int x,int y){
	int wifiCheckTime = millis();
	int wifiCheckPoints = 0;
	while(millis()-wifiCheckTime<2000){
		while(wifiCheckPoints<7){
			matrix->clear();
			switch(typ){
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

			switch(wifiCheckPoints){
				case 6:
					matrix->drawPixel(x,y,0x07E0);
				case 5:
					matrix->drawPixel(x-1,y+1,0x07E0);
				case 4:
					matrix->drawPixel(x-2,y+2,0x07E0);
				case 3:
					matrix->drawPixel(x-3,y+3,0x07E0);
				case 2:
					matrix->drawPixel(x-4,y+4,0x07E0);
				case 1:
					matrix->drawPixel(x-5,y+3,0x07E0);
				case 0:
					matrix->drawPixel(x-6,y+2,0x07E0);
				break;
				}
			wifiCheckPoints++;
			matrix->show();
			delay(100);
		}
	}
}

void hardwareAnimatedSearchFast(int rounds,int x,int y){
	matrix->clear();
	matrix->setTextColor(0xFFFF);
	matrix->setCursor(1, 6);
	matrix->print("Server");

	switch(rounds){
		case 3:
			matrix->drawPixel(x,y,0xFFFF);
			matrix->drawPixel(x+1,y+1,0xFFFF);
			matrix->drawPixel(x+2,y+2,0xFFFF);
			matrix->drawPixel(x+3,y+3,0xFFFF);
			matrix->drawPixel(x+2,y+4,0xFFFF);
			matrix->drawPixel(x+1,y+5,0xFFFF);
			matrix->drawPixel(x,y+6,0xFFFF);
		case 2:
			matrix->drawPixel(x-1,y+2,0xFFFF);
			matrix->drawPixel(x,y+3,0xFFFF);
			matrix->drawPixel(x-1,y+4,0xFFFF);
			case 1:
			matrix->drawPixel(x-3,y+3,0xFFFF);
		case 0:
		break;
	}
	matrix->show();
}

void hardwareAnimatedSearch(int typ,int x,int y){
	for(int i=0;i<4;i++){
		matrix->clear();
		matrix->setTextColor(0xFFFF);
		if(typ==0){
			matrix->setCursor(7, 6);
			matrix->print("WiFi");
		} else if(typ==1){
			matrix->setCursor(1, 6);
			matrix->print("Server");
		}
		switch(i){
			case 3:
				matrix->drawPixel(x,y,0xFFFF);
				matrix->drawPixel(x+1,y+1,0xFFFF);
				matrix->drawPixel(x+2,y+2,0xFFFF);
				matrix->drawPixel(x+3,y+3,0xFFFF);
				matrix->drawPixel(x+2,y+4,0xFFFF);
				matrix->drawPixel(x+1,y+5,0xFFFF);
				matrix->drawPixel(x,y+6,0xFFFF);
			case 2:
				matrix->drawPixel(x-1,y+2,0xFFFF);
				matrix->drawPixel(x,y+3,0xFFFF);
				matrix->drawPixel(x-1,y+4,0xFFFF);
			case 1:
				matrix->drawPixel(x-3,y+3,0xFFFF);
			case 0:
			break;
		}
		matrix->show();
		delay(500);
	}
}

void utf8ascii(char* s) {
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


String GetChipID(){
	return String(ESP.getChipId());
}

int GetRSSIasQuality(int rssi){
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

void updateMatrix(byte payload[],int length){
	int y_offset = 5;

	if(firstStart){
		hardwareAnimatedCheck(1,30,2);
		firstStart=false;
	}

	switch(payload[0]){
		case 0:{
			//Command 0: DrawText

			//Prepare the coordinates
			uint16_t x_coordinate = int(payload[1]<<8)+int(payload[2]);
			uint16_t y_coordinate = int(payload[3]<<8)+int(payload[4]);

			//Serial.printf("X: %d - Y: %d\n",x_coordinate,y_coordinate);

			matrix->setCursor(x_coordinate+1, y_coordinate+y_offset);
			matrix->setTextColor(matrix->Color(payload[5],payload[6],payload[7]));

			String myText = "";
			char myChar;
			for(int i = 8;i<length;i++){
				char c = payload[i];
				myText += c;
			}
			//Serial.printf("Text: %s\n",myText.c_str());
			matrix->print(utf8ascii(myText));
			break;
		}
		case 1:{
			//Command 1: DrawBMP

			//Prepare the coordinates
			uint16_t x_coordinate = int(payload[1]<<8)+int(payload[2]);
			uint16_t y_coordinate = int(payload[3]<<8)+int(payload[4]);

			int16_t width = payload[5];
			int16_t height = payload[6];

			unsigned short colorData[width*height];

			for(int i = 0; i<width*height*2; i++){
				colorData[i/2] = (payload[i+7]<<8)+payload[i+1+7];
				i++;
			}

			for (int16_t j = 0; j < height; j++, y_coordinate++){
				for (int16_t i = 0; i < width; i++){
					matrix->drawPixel(x_coordinate + i, y_coordinate, (uint16_t)colorData[j*width+i]);
				}
			}
			break;
		}

		case 2:{
			//Command 2: DrawCircle

			//Prepare the coordinates
			uint16_t x0_coordinate = int(payload[1]<<8)+int(payload[2]);
			uint16_t y0_coordinate = int(payload[3]<<8)+int(payload[4]);
			uint16_t radius = payload[5];
			matrix->drawCircle(x0_coordinate, y0_coordinate, radius, matrix->Color(payload[6], payload[7], payload[8]));
			break;
		}
		case 3:{
			//Command 3: FillCircle

			//Prepare the coordinates
			uint16_t x0_coordinate = int(payload[1]<<8)+int(payload[2]);
			uint16_t y0_coordinate = int(payload[3]<<8)+int(payload[4]);
			uint16_t radius = payload[5];
			matrix->fillCircle(x0_coordinate, y0_coordinate, radius, matrix->Color(payload[6], payload[7], payload[8]));
			break;
		}
		case 4:{
			//Command 4: DrawPixel

			//Prepare the coordinates
			uint16_t x0_coordinate = int(payload[1]<<8)+int(payload[2]);
			uint16_t y0_coordinate = int(payload[3]<<8)+int(payload[4]);
			matrix->drawPixel(x0_coordinate, y0_coordinate, matrix->Color(payload[5], payload[6], payload[7]));
			break;
		}
		case 5:{
			//Command 5: DrawRect

			//Prepare the coordinates
			uint16_t x0_coordinate = int(payload[1]<<8)+int(payload[2]);
			uint16_t y0_coordinate = int(payload[3]<<8)+int(payload[4]);
			int16_t width = payload[5];
			int16_t height = payload[6];

			matrix->drawRect(x0_coordinate, y0_coordinate, width, height, matrix->Color(payload[7], payload[8], payload[9]));
			break;
		}
		case 6:{
			//Command 6: DrawLine

			//Prepare the coordinates
			uint16_t x0_coordinate = int(payload[1]<<8)+int(payload[2]);
			uint16_t y0_coordinate = int(payload[3]<<8)+int(payload[4]);
			uint16_t x1_coordinate = int(payload[5]<<8)+int(payload[6]);
			uint16_t y1_coordinate = int(payload[7]<<8)+int(payload[8]);
			matrix->drawLine(x0_coordinate, y0_coordinate, x1_coordinate, y1_coordinate, matrix->Color(payload[9],payload[10],payload[11]));
			break;
		}

		case 7:{
			//Command 7: FillMatrix

			matrix->fillScreen(matrix->Color(payload[1],payload[2],payload[3]));
			break;
		}

		case 8:{
			//Command 8: Show
			matrix->show();
			break;
		}
		case 9:{
			//Command 9: Clear
			matrix->clear();
			break;
		}
		case 10:{
			//Command 10: Play
			myMP3.volume(payload[3]);
			delay(10);
			myMP3.playFolder(payload[1],payload[2]);
			break;
		}
		case 11:{
			//Command 11: GetLux
			if(!usbWifi){
				StaticJsonBuffer<200> jsonBuffer;
				client.publish("matrixLux", String(photocell.getCurrentLux()).c_str());
			} else {
				StaticJsonBuffer<200> jsonBuffer;
				JsonObject& root = jsonBuffer.createObject();
				root["LUX"] = photocell.getCurrentLux();
				String JS;
				root.printTo(JS);
				Serial.println(String(JS));
			}
			break;
		}
		case 12:{
			//Command 12: GetMatrixInfo
			StaticJsonBuffer<200> jsonBuffer;
			JsonObject& root = jsonBuffer.createObject();
			root["version"] = version;
			root["wifirssi"] = String(WiFi.RSSI());
			root["wifiquality"] =GetRSSIasQuality(WiFi.RSSI());
			root["wifissid"] =WiFi.SSID();
			root["getIP"] =WiFi.localIP().toString();
			root["LUX"] =photocell.getCurrentLux();
			BMESensor.refresh();
			root["Temp"] = BMESensor.temperature;
			root["Hum"] = BMESensor.humidity;
			root["hPa"] =BMESensor.pressure;
			String JS;
			root.printTo(JS);
			if (!usbWifi){
				client.publish("matrixInfo", JS.c_str());
			} else {
				Serial.println(String(JS));
			}
			break;
		}
		case 13:{
  			matrix->setBrightness(payload[1]);
			break;
  		}
	}
}

void callback(char *topic, byte *payload, unsigned int length){
	updateMatrix(payload,length);
}

void reconnect(){
	if(!usbWifi){
		while (!client.connected()){
			String clientId = "AWTRIXController-";
			clientId += String(random(0xffff), HEX);
			hardwareAnimatedSearch(1,28,0);
			if (client.connect(clientId.c_str())){
				client.subscribe("awtrixmatrix/#");
				client.publish("matrixstate", "connected");
			}
		}
	}
}

void ICACHE_RAM_ATTR interruptRoutine() {
  isr_flag = 1;
}

void handleGesture() {
		String control;
    if (apds.isGestureAvailable()) {
    switch ( apds.readGesture() ) {
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
		#ifdef USB_CONNECTION
			StaticJsonBuffer<200> jsonBuffer;
			JsonObject& root = jsonBuffer.createObject();
			String JS;
			root.printTo(JS);
			Serial.println(String(JS));
		#else
			client.publish("control", control.c_str());
		#endif
  }
}

uint32_t Wheel(byte WheelPos, int pos) {
  if(WheelPos < 85) {
   return matrix->Color((WheelPos * 3)-pos, (255 - WheelPos * 3)-pos, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return matrix->Color((255 - WheelPos * 3)-pos, 0, (WheelPos * 3)-pos);
  } else {
   WheelPos -= 170;
   return matrix->Color(0, (WheelPos * 3)-pos, (255 - WheelPos * 3)-pos);
  }
}

void flashProgress(unsigned int progress, unsigned int total) {
    matrix->setBrightness(100);
    long num = 32 * 8 * progress / total;
    for (unsigned char y = 0; y < 8; y++) {
        for (unsigned char x = 0; x < 32; x++) {
            if (num-- > 0) matrix->drawPixel(x, 8 - y - 1, Wheel((num*16) & 255,0));
        }
    }
    matrix->setCursor(0, 6);
		matrix->setTextColor(matrix->Color(255, 255, 255));
    matrix->print("FLASHING");
    matrix->show();
}

void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void configModeCallback (WiFiManager *myWiFiManager) {
	Serial.println("Entered config mode");
	Serial.println(WiFi.softAPIP());
	//if you used auto generated SSID, print it
	Serial.println(myWiFiManager->getConfigPortalSSID());
	//matrix->clear();
	//matrix->setCursor(3, 6);
	//matrix->print("Hotspot");
	//matrix->show();
}

void setup(){
	WiFiManager wifiManager;
	if (drd.detect()) {
		Serial.println("** Double reset boot **");
		wifiManager.resetSettings();
		//SPIFFS.format();
	}
	Serial.setRxBufferSize(1024);
	Serial.begin(115200);
	if (SPIFFS.begin()) {
		//if file not exists
		if (!(SPIFFS.exists("/config.json"))) {
			SPIFFS.open("/config.json", "w+");
			Serial.println("make File...");
		}

      	File configFile = SPIFFS.open("/config.json", "r");
      	if (configFile) {
        	size_t size = configFile.size();
        	// Allocate a buffer to store contents of the file.
        	std::unique_ptr < char[] > buf(new char[size]);
        	configFile.readBytes(buf.get(), size);
        	DynamicJsonBuffer jsonBuffer;
        	JsonObject & json = jsonBuffer.parseObject(buf.get());
        	json.printTo(Serial);
        	if (json.success()) {
          		Serial.println("\nparsed json");
				String tempServer = json["awtrix_server"];
				String tempConnection = json["connection"];

				for(int i = 0;i<16;i++){
					awtrix_server[i] = tempServer[i];
				}
				for(int i = 0;i<5;i++){
					connection[i] = tempConnection[i];
				}
        	}
        	configFile.close();
      	}
	} else {
		Serial.println("mounting not possible");
	}
	Serial.printf("\nLoading from SPIFFS: %s - %s\n",awtrix_server,connection);

	//set the bool for the connection
	if((connection[0]==117)&&(connection[1]==115)&&(connection[2]==98)){
		usbWifi = true;
		Serial.println("USB loaded...");
	} else if((connection[0]==119)&&(connection[1]==105)&&(connection[2]==102)&&(connection[3]==105)){
		usbWifi = false;
		Serial.println("WiFi loaded...");
	} else {
		usbWifi = false;
		Serial.println("Wrong loaded...");
	}

	
	//WiFi.mode(WIFI_STA);
	//WiFi.begin("", "");
	wifiManager.setTimeout(1);
	wifiManager.autoConnect("AwtrixWiFiSetup2","awtrixxx");
	wifiManager.setTimeout(0);

	//Extra parameter for configuration the Awtrix-Client
	WiFiManagerParameter custom_server_ip("server", "server_ip", awtrix_server, 15);
	WiFiManagerParameter custom_connection("connection", "usb or wifi", connection, 4);
	wifiManager.setSaveConfigCallback(saveConfigCallback);
	wifiManager.addParameter(&custom_server_ip);
	wifiManager.addParameter(&custom_connection);

	matrix->begin();
	matrix->setTextWrap(false);
	matrix->setBrightness(80);
	matrix->setFont(&TomThumb);

	mySoftwareSerial.begin(9600);
	FastLED.addLeds<NEOPIXEL, D2>(leds, 256).setCorrection(TypicalLEDStrip);

	int wifiTimeout = millis();

	while (WiFi.status() != WL_CONNECTED){
		hardwareAnimatedSearch(0,24,0);

		if(millis()-wifiTimeout>TIME_FOR_SEARCHING_WIFI){
			matrix->clear();
			matrix->setCursor(3, 6);
			matrix->print("Hotspot");
			matrix->show();
			while (WiFi.status() != WL_CONNECTED){
				wifiManager.autoConnect("AwtrixWiFiSetup2","awtrixxx");
			}
		}
	}
		
		strcpy(awtrix_server, custom_server_ip.getValue());
		strcpy(connection, custom_connection.getValue());

		//check the connection and save the changeState if changed (need for restart)
		if(strcmp(connection,"usb")||strcmp(connection,"USB")){
			if(usbWifi==false){
				changeConnectiom = true;
			}
		} else if(strcmp(connection,"wifi")||strcmp(connection,"WIFI")){
			if(usbWifi==true){
				changeConnectiom = true;
			}
		} 
		
		if(shouldSaveConfig){
			Serial.println("saving config");

			//Check if the connection value is allowed -> if not -> usb!
			if(strcmp(connection,"usb")||strcmp(connection,"USB")){
				usbWifi = true;
				Serial.println("saved usb");

			} else if(strcmp(connection,"wifi")||strcmp(connection,"WIFI")){
				usbWifi = false;
				Serial.println("saved wifi");
			} else {
				usbWifi = false;	//default
				strcpy(connection,"usb");
				Serial.println("saved usb (default)");
			}

			DynamicJsonBuffer jsonBuffer;
			JsonObject& json = jsonBuffer.createObject();
			json["awtrix_server"] = awtrix_server;
			json["connection"] = connection;

			File configFile = SPIFFS.open("/config.json", "w");
			if (!configFile) {
				Serial.println("failed to open config file for writing");
			}
			json.printTo(Serial);
			json.printTo(configFile);
			configFile.close();
			//end save
			if(changeConnectiom){
				ESP.reset();
			}
		}
		hardwareAnimatedCheck(0,27,2);

	client.setServer(awtrix_server, 7001);
	client.setCallback(callback);


	photocell.setPhotocellPositionOnGround(false);

	myMP3.begin(mySoftwareSerial);

	tempState = BMESensor.begin(APDS9960_SDA,APDS9960_SCL); 
	if(tempState){
		//temp OK
		hardwareAnimatedCheck(2,28,2);
	} else {
		//temp NOK
		hardwareAnimatedUncheck(2,27,1);
	}

  	pinMode(APDS9960_INT, INPUT);
	attachInterrupt(APDS9960_INT, interruptRoutine, FALLING);
  	apds.init();
  	apds.enableGestureSensor(true);
  	ArduinoOTA.onStart([&]() {
		updating = true;
		matrix->clear();
  	});

	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
			flashProgress(progress, total);
	});

  	ArduinoOTA.begin();
	matrix->clear();
	matrix->setCursor(7,6);

	bufferpointer=0;

	myTime = millis()-500;
	myCounter = 0;
}

void loop() {
 	ArduinoOTA.handle();


	if(firstStart){
		if(millis()-myTime>500){
			hardwareAnimatedSearchFast(myCounter,28,0);
			myCounter++;
			if(myCounter==4){
				myCounter=0;
			}
			myTime = millis();
		}
	}

 	if (!updating) {
	 	if(usbWifi){
			while(Serial.available () > 0){
				myBytes[bufferpointer] = Serial.read();
				if ((myBytes[bufferpointer]==255)&&(myBytes[bufferpointer-1]==255)&&(myBytes[bufferpointer-2]==255)){
					updateMatrix(myBytes, bufferpointer);
					for(int i =0;i<bufferpointer;i++){
						myBytes[i]=0;
					}
					bufferpointer=0;
					break;
				} else {
					bufferpointer++;
				}
				if(bufferpointer==1000){
					bufferpointer=0;
				}
			}
		}
		else {
			if (!client.connected()){
				reconnect();
			}else{
				client.loop();
			}
		}
	if(isr_flag == 1) {
    detachInterrupt(APDS9960_INT);
    handleGesture();
    isr_flag = 0;
    attachInterrupt(APDS9960_INT, interruptRoutine, FALLING);
  }
}
}
