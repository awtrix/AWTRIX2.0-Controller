#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>     // Replace with WebServer.h for ESP32
#include <AutoConnect.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <FS.h>
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

String version = "0.9b"; 

#ifndef USB_CONNECTION
	WiFiClient espClient;
	PubSubClient client(espClient);
#endif

//UDP Settings:
WiFiUDP Udp;
unsigned int localUdpPort = 7005;
char packetBuffer[6];

bool firstStart = true;


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
ESP8266WebServer Server;
AutoConnect      Portal(Server);

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
  for (int i = 0; i < s.length(); i++)
  {
    c = utf8ascii(s.charAt(i));
    if (c != 0) r += c;
  }
  return r;
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

#ifndef USB_CONNECTION
void callback(char *topic, byte *payload, unsigned int length)
{
	int y_offset = 5;

	if(firstStart){
		firstStart=false;
		int mydelay = millis();
		int serverCheckPoints = 0;
		while(millis()-mydelay<2000){
			while(serverCheckPoints<7){
				matrix->clear();
				matrix->setCursor(1, 6);
				matrix->print("Server");
				switch(serverCheckPoints){
					case 6:
						matrix->drawPixel(30,2,0x07E0);
					case 5:
						matrix->drawPixel(29,3,0x07E0);
					case 4:
						matrix->drawPixel(28,4,0x07E0);
					case 3:
						matrix->drawPixel(27,5,0x07E0);
					case 2:
						matrix->drawPixel(26,6,0x07E0);
					case 1:
						matrix->drawPixel(25,5,0x07E0);
					case 0:
						matrix->drawPixel(24,4,0x07E0);
					break;
				}
				serverCheckPoints++;
				matrix->show();
				delay(100);
			}
		}
	}

	switch(payload[0]){
		case 0:{
			//Command 0: DrawText

			//Prepare the coordinates
			uint16_t x_coordinate = int(payload[1]<<8)+int(payload[2]);
			uint16_t y_coordinate = int(payload[3]<<8)+int(payload[4]);

			matrix->setCursor(x_coordinate+1, y_coordinate+y_offset);
			matrix->setTextColor(matrix->Color(payload[5],payload[6],payload[7])); 
		
			String myText = "";
			char myChar;
			for(int i = 8;i<length;i++){
				char c = payload[i];
				myText += c;
			}
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
			client.publish("matrixLux", String(photocell.getCurrentLux()).c_str());
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
			String JS;
			root.printTo(JS);
			client.publish("matrixInfo", JS.c_str());
			break;
		}
	}
}

void reconnect()
{
	while (!client.connected())
	{
		String clientId = "AWTRIXController-";
    clientId += String(random(0xffff), HEX);
		if (client.connect(clientId.c_str()))
		{
			client.subscribe("awtrixmatrix/#");
			client.publish("matrixstate", "connected");
		}
		else
		{
			delay(5000);
		}
	}
}
#else
void processing(byte payload[],int length)
{
int y_offset = 5;

//byte payload[length];
//for(int i=0; i<length;i++){
//	payload[i] = message[i];
//}

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
				colorData[i/2] = payload[i+7]<<8+payload[i+1+7];
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
			Serial.println(String(photocell.getCurrentLux()).c_str());
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
			String JS;
			root.printTo(JS);
			Serial.println((String)JS);
			break;
		}
	}
}
#endif

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

byte myBytes[1000];
int bufferpointer;

bool checkForServer(){
		//Serial.printf("Get...\n");
		Udp.read(packetBuffer,6);
		for(int i=0;i<6;i++){
			printf("%c",packetBuffer[i]);
		}
		printf("\n");
		

	char test[20];
	if((packetBuffer[0]==123)&&(packetBuffer[1]==1)){
		sprintf(wifiConfig.awtrix_server,"%03d.%03d.%03d.%03d",int(packetBuffer[2]),int(packetBuffer[3]),int(packetBuffer[4]),int(packetBuffer[5]));
		return true;
	} 
	return false;
}

String feelsOn(AutoConnectAux& aux, PageArgument& args) {

	// Get the AutoConnectInput named "feels".
	// The where() function returns an uri string of the AutoConnectAux that triggered this handler.
	AutoConnectAux* hello = Portal.aux(Portal.where());
	AutoConnectInput& feels = hello->getElement<AutoConnectInput>("feels");

	strcpy(wifiConfig.awtrix_server, feels.value.c_str());
	Serial.println(feels.value);
	return String("");
}


void setup()
{
	#ifndef USB_CONNECTION
		Serial.begin(9600);
	#endif
	mySoftwareSerial.begin(9600);
	FastLED.addLeds<NEOPIXEL, D2>(leds, 256).setCorrection(TypicalLEDStrip);
	Serial.println("Hey, I´m your Awtrix!\n");

	//WiFiManager wifiManager;
	
	WiFi.mode(WIFI_STA);
	WiFi.begin(wifiConfig.ssid, wifiConfig.password);
	matrix->begin();
	matrix->setTextWrap(false);
	matrix->setBrightness(80);
	matrix->setFont(&TomThumb);
	int wifiTimeout = millis();
	String wifiString = "WiFi";
	int wifiPoints = 0;
	while (WiFi.status() != WL_CONNECTED)
	{
		matrix->clear();
		matrix->setTextColor(0xFFFF);
		matrix->setCursor(7, 6);
		matrix->print("WiFi");

		switch(wifiPoints){
			case 3:
				wifiPoints=-1;
				matrix->drawPixel(24,0,0xFFFF);
				matrix->drawPixel(25,1,0xFFFF);
				matrix->drawPixel(26,2,0xFFFF);
				matrix->drawPixel(27,3,0xFFFF);
				matrix->drawPixel(26,4,0xFFFF);
				matrix->drawPixel(25,5,0xFFFF);
				matrix->drawPixel(24,6,0xFFFF);
			case 2: 
				matrix->drawPixel(23,2,0xFFFF);
				matrix->drawPixel(24,3,0xFFFF);
				matrix->drawPixel(23,4,0xFFFF);
			case 1: 
				matrix->drawPixel(21,3,0xFFFF);
			case 0: 
			break;	
		}	
		matrix->show();
		delay(500);	
		wifiPoints++;

		if(millis()-wifiTimeout>10000){
			matrix->clear();
			matrix->setCursor(3, 6);
			matrix->print("Hotspot");
			matrix->show();

			AutoConnectConfig  Config;
			Config.title = "Awtrix Setup";
			Config.apid = "AwtrixSetup";
			Config.psk = "awtrixxx";
			Portal.config(Config);
			
			/*
			ACText(header, "On this page you can configure your Awtrix.");
			ACText(caption1, "The hotspot appears only with unsuccessful wlan connection");
			AutoConnectInput input("input", "", "Server", "MQTT broker server");
			ACSubmit(save, "SAVE", "/mqtt_save");
			AutoConnectRadio radio("radio", { "Awtrix_1", "Awtrix_2", "Awtrix_3" }, "Awtrix Name", AC_Vertical, 1);
			AutoConnectAux  aux1("/awtrix_setting", "Awtrix Setting",true, { header, caption1, radio, save});
			ACText(caption2, "Save parameters");
			Portal.join({ aux1 });
			

			const static char addonJson[] PROGMEM = R"raw(
			[
			{
				"title": "Hello",
				"uri": "/hello",
				"menu": true,
				"element": [
				{
					"name": "feels",
					"type": "ACInput",
					"label": "Server address"
				},
				{
					"name": "send",
					"type": "ACSubmit",
					"value": "Just it!",
					"uri": "/feels"
				}
				]
			},
			{
				"title": "Hello",
				"uri": "/feels",
				"menu": false,
				"element": [
				{
					"name": "echo",
					"type": "ACText",
					"style": "color:blue;font-family:verdana;font-size:300%;"
				}
				]
			}
			]
			)raw";

			Portal.load(addonJson);   
			Portal.on("/feels", feelsOn, AC_EXIT_AHEAD);
			*/
			Portal.begin();
			while (WiFi.status() != WL_CONNECTED)
			{	
				Portal.handleClient();

			}
			//wifiManager.autoConnect("AwtrixWiFiSetup");
		}
	}

	//show wifi connected
	int wifiCheckTime = millis();
	int wifiCheckPoints = 0;
	while(millis()-wifiCheckTime<2000){
		while(wifiCheckPoints<7){
			matrix->clear();
			matrix->setCursor(7, 6);
			matrix->print("WiFi");
			switch(wifiCheckPoints){
				case 6:
					matrix->drawPixel(27,2,0x07E0);
				case 5:
					matrix->drawPixel(26,3,0x07E0);
				case 4:
					matrix->drawPixel(25,4,0x07E0);
				case 3:
					matrix->drawPixel(24,5,0x07E0);
				case 2:
					matrix->drawPixel(23,6,0x07E0);
				case 1:
					matrix->drawPixel(22,5,0x07E0);
				case 0:
					matrix->drawPixel(21,4,0x07E0);
				break;
			}
			wifiCheckPoints++;
			matrix->show();
			delay(100);
		}
	}
	
	//Connection to Server
	#ifdef USB_CONNECTION
		Serial.begin(115200);
	#else
		client.setServer(wifiConfig.awtrix_server, 7001);
		client.setCallback(callback);
	#endif
	/*
	bool clientConnected = false;
	while(!client.connected()){
		int ServerTimeout = millis();
		int serverPoints = 0;
		Udp.begin(localUdpPort);
		while (!client.connected()){
			matrix->clear();
			matrix->setTextColor(0xFFFF);
			matrix->setCursor(1, 6);
			matrix->print("Server");
			switch(serverPoints){
				case 3:
					serverPoints=-1;
					matrix->drawPixel(28,0,0xFFFF);
					matrix->drawPixel(29,1,0xFFFF);
					matrix->drawPixel(30,2,0xFFFF);
					matrix->drawPixel(31,3,0xFFFF);
					matrix->drawPixel(30,4,0xFFFF);
					matrix->drawPixel(29,5,0xFFFF);
					matrix->drawPixel(28,6,0xFFFF);
				case 2: 
					matrix->drawPixel(27,2,0xFFFF);
					matrix->drawPixel(28,3,0xFFFF);
					matrix->drawPixel(27,4,0xFFFF);
				case 1: 
					matrix->drawPixel(25,3,0xFFFF);
				case 0: 
				break;	
			}	
			matrix->show();
			delay(500);	
			serverPoints++;

			if (checkForServer()){
				clientConnected = true;
				matrix->clear();
				break;
			}
		}
		Udp.stop();
		reconnect();
		//client.setServer(wifiConfig.awtrix_server, 7001);
		//Serial.println("Hier bin ich...");
	}

	wifiCheckTime = millis();
	wifiCheckPoints = 0;
	while(millis()-wifiCheckTime<2000){
		while(wifiCheckPoints<7){
			matrix->clear();
			matrix->setCursor(1, 6);
			matrix->print("Server");
			switch(wifiCheckPoints){
				case 6:
					matrix->drawPixel(30,2,0x07E0);
				case 5:
					matrix->drawPixel(29,3,0x07E0);
				case 4:
					matrix->drawPixel(28,4,0x07E0);
				case 3:
					matrix->drawPixel(27,5,0x07E0);
				case 2:
					matrix->drawPixel(26,6,0x07E0);
				case 1:
					matrix->drawPixel(25,5,0x07E0);
				case 0:
					matrix->drawPixel(24,4,0x07E0);
				break;
			}
			wifiCheckPoints++;
			matrix->show();
			delay(100);
		}
	}
	*/
	client.publish("control", "Hallo Welt");

	photocell.setPhotocellPositionOnGround(false);

	myMP3.begin(mySoftwareSerial);

	Wire.begin(APDS9960_SDA,APDS9960_SCL);
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
	bufferpointer=0;
	matrix->clear();
	matrix->setCursor(7,6);
}

void loop() {
 ArduinoOTA.handle();
 if (!updating) {
	 #ifdef USB_CONNECTION
		//while (Serial.available () > 0) {
			//String message= Serial.readStringUntil(':');
			//processing(sizeof(message));
			
			if(Serial.available () > 0){
				//debuggingWithMatrix("Hallo");

				myBytes[bufferpointer] = Serial.read();
				if ((myBytes[bufferpointer]==255)&&(myBytes[bufferpointer-1]==255)&&(myBytes[bufferpointer-2]==255)){
					processing(myBytes, bufferpointer);
					bufferpointer=0;
				} else {
					bufferpointer++;
				}
				
				if(bufferpointer==1000){
					bufferpointer=0;
				}
			}
			
		//}
	#else
		if (!client.connected())
		{
			reconnect();
		}else{
			client.loop();
		}
	#endif
	if(isr_flag == 1) {
    detachInterrupt(APDS9960_INT);
    handleGesture();
    isr_flag = 0;
    attachInterrupt(APDS9960_INT, interruptRoutine, FALLING);
  }
}
}
