#include <FS.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>     // Replace with WebServer.h for ESP32
//#include <AutoConnect.h>
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

String version = "0.9b"; 

WiFiClient espClient;
PubSubClient client(espClient);

//UDP Settings:
WiFiUDP Udp;
unsigned int localUdpPort = 7005;
char packetBuffer[6];

bool firstStart = true;
int myTime;
int myCounter;
int TIME_FOR_SEARCHING_WIFI = 1;

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

void hardwareAnimatedCheck(int typ,int x,int y){
	int wifiCheckTime = millis();
	int wifiCheckPoints = 0;
	while(millis()-wifiCheckTime<2000){
		while(wifiCheckPoints<7){
			matrix->clear();
			
			if(typ==0){
				matrix->setCursor(7, 6);
				matrix->print("WiFi");
			} else if(typ==1){
				matrix->setCursor(1, 6);
				matrix->print("Server");
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
	int y_offset = 5;
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

void setup(){
	Serial.begin(9600);
	//SPIFFS.format();

	if (SPIFFS.begin()) {
    	Serial.println("mounted file system");
		if (!SPIFFS.exists("/config.json")) {
			SPIFFS.open("/config.json", "w+");
			Serial.println("making new file");
		}
		if (SPIFFS.exists("/config.json")) {
			//file exists, reading and loading
			Serial.println("reading config file");
			File configFile = SPIFFS.open("/config.json", "r");
			if (configFile) {
				Serial.println("opened config file");
				size_t size = configFile.size();
				// Allocate a buffer to store contents of the file.
				std::unique_ptr<char[]> buf(new char[size]);

				configFile.readBytes(buf.get(), size);
				DynamicJsonBuffer jsonBuffer;
				JsonObject& json = jsonBuffer.parseObject(buf.get());
				json.printTo(Serial);
				if (json.success()) {
					Serial.println("\nparsed json");

					strcpy(wifiConfig.awtrix_server, json["mqtt_server"]);
					//strcpy(mqtt_port, json["mqtt_port"]);
					//strcpy(blynk_token, json["blynk_token"]);

					if(json["ip"]) {
						Serial.println("setting custom ip from config");
						//static_ip = json["ip"];
						//strcpy(static_ip, json["ip"]);
						//strcpy(static_gw, json["gateway"]);
						//strcpy(static_sn, json["subnet"]);
						//strcat(static_ip, json["ip"]);
						//static_gw = json["gateway"];
						//static_sn = json["subnet"];
						//Serial.println(static_ip);
						/*            Serial.println("converting ip");
						IPAddress ip = ipFromCharArray(static_ip);
						Serial.println(ip);*/
					} else {
						Serial.println("no custom ip in config");
					}
				} else {
					Serial.println("failed to load json config");
				}
			}
		} else {
			Serial.println("file not found");
		}
  	} else {
    	Serial.println("failed to mount FS");
	}


	

	//Serial.println("Hey, I´m your Awtrix!\n");
	//Serial.printf("Stored ServerIP: %s\n",wifiConfig.awtrix_server);

	//Extra parameter for configuration the Awtrix-Client
	WiFiManagerParameter custom_server_ip("server", "server_ip", wifiConfig.awtrix_server, 20);

	WiFiManager wifiManager;

	wifiManager.setSaveConfigCallback(saveConfigCallback);
	
	wifiManager.addParameter(&custom_server_ip);

	matrix->begin();
	matrix->setTextWrap(false);
	matrix->setBrightness(80);
	matrix->setFont(&TomThumb);

	mySoftwareSerial.begin(9600);
	FastLED.addLeds<NEOPIXEL, D2>(leds, 256).setCorrection(TypicalLEDStrip);
	
	if(usbWifi)
		Serial.begin(115200);
	else {
		//Serial.begin(9600);
		//Serial.printf("Stored ServerIP: %s\n",wifiConfig.awtrix_server);

		WiFi.mode(WIFI_STA);
		WiFi.begin(ssid, password);
		int wifiTimeout = millis();
		while (WiFi.status() != WL_CONNECTED){
			hardwareAnimatedSearch(0,24,0);

			if(millis()-wifiTimeout>TIME_FOR_SEARCHING_WIFI){
				matrix->clear();
				matrix->setCursor(3, 6);
				matrix->print("Hotspot");
				matrix->show();

				//AutoConnectConfig  Config;
				//Config.title = "Awtrix";
				//Config.apid = "AwtrixSetup";
				//Config.psk = "awtrixxx";
				//Config.apip = IPAddress(8,8,8,8);
				
				//ACCheckbox("myCheckbox","myCheckbox","Hallo Welt",true);
				//AutoConnectAux aux1("/awtrix_setup","Awtrix Setting",true);
				//ACText(header,"<H1>Awtrix Setup</H1>");
				//ACText(caption,"<H4>On this page you can configure your Awtrix without changing the code.</H4>");
				//AutoConnectInput input("input", "", "<H3>Server-IP:</H3>", "Awtrix Server");
				//AutoConnectRadio radio("radio", { "USB", "WiFi" }, "<H3>Connection:</H3>", AC_Vertical, 1);
				//AutoConnectButton button("button", "SAVE", "myFunction()");

				//aux1.add({header, caption,input,radio,button});
				//Portal.join(aux1);

				//Portal.config(Config);
				

				//Portal.begin();

				while (WiFi.status() != WL_CONNECTED)
				{	
					//Portal.handleClient();		
					wifiManager.autoConnect("AwtrixWiFiSetup2","awtrixxx");
				}
			}
		}

		strcpy(wifiConfig.awtrix_server, custom_server_ip.getValue());

		if(shouldSaveConfig){
			Serial.println("saving config");
			DynamicJsonBuffer jsonBuffer;
			JsonObject& json = jsonBuffer.createObject();
			json["mqtt_server"] = wifiConfig.awtrix_server;
			//json["mqtt_port"] = mqtt_port;
			//json["blynk_token"] = blynk_token;

			File configFile = SPIFFS.open("/config.json", "w");
			if (!configFile) {
				Serial.println("failed to open config file for writing");
			}
			json.printTo(Serial);
			json.printTo(configFile);
			configFile.close();
			//end save
			ssid = "222";
		}
		hardwareAnimatedCheck(0,27,2);

		client.setServer(wifiConfig.awtrix_server, 7001);
		client.setCallback(callback);
	}

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
