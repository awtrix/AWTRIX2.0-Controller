// AWTRIX Controller
// Copyright (C) 2020
// by Blueforcer & Mazze2000

#include <Arduino.h>
#include "MenueControl/MenueControl.h"
#include "ESP.cpp"
#include "Mainboard.h"
#include "Matrix.h"
#include "Storage.h"
#include "Temperature.h"
#include <WiFiManager.h>

#include <WiFiClient.h>
#include <PubSubClient.h>

#include <DFMiniMp3.h>

ESPControl espControl;
Mainboard mainboard;
Matrix matrix;
Storage storage;
Temperature temperature;

String version = "0.50";

IPAddress Server;
WiFiClient espClient;
PubSubClient client(espClient);

WiFiManager wifiManager;

class Mp3Notify; 
typedef DFMiniMp3<HardwareSerial, Mp3Notify> DfMp3; 
DfMp3 dfmp3(Serial2);

class Mp3Notify
{

};

//flag for saving data
bool shouldSaveConfig = false;

int ldrState = 1000;		// 0 = None
bool USBConnection = false; // true = usb...
bool WIFIConnection = false;
bool notify=false;
int connectionTimout;
int matrixTempCorrection = 0;

bool firstStart = true;
int myTime;	 //need for loop
int myTime2; //need for loop
int myTime3; //need for loop3
int myCounter;
int myCounter2;
//boolean getLength = true;
//int prefix = -5;

bool ignoreServer = false;

//Reset time (Touch Taster)
int resetTime = 6000; //in milliseconds

boolean awtrixFound = false;
int myPointer[14];
uint32_t messageLength = 0;
uint32_t SavemMessageLength = 0;

bool updating = false;

//USB Connection:
byte myBytes[1000];
int bufferpointer;

//Zum speichern...
int cfgStart = 0;

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
	switch (last)	// conversion depending on first UTF8-character
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
    storage.saveAwtrixServer(storage.getAwtrixServer(),false);
    storage.saveMatrixType(storage.getMatrixType(),false);
    storage.saveMatrixCorrection(storage.getMatrixCorrection(),false);
    storage.savePort(storage.getPort(),false);
    storage.saveConfig();
	return true;
}


void sendToServer(String s)
{
	if (USBConnection)
	{
		uint32_t laenge = s.length();
		Serial.printf("%c%c%c%c%s", (laenge & 0xFF000000) >> 24, (laenge & 0x00FF0000) >> 16, (laenge & 0x0000FF00) >> 8, (laenge & 0x000000FF), s.c_str());
	}
	else
	{
		client.publish("matrixClient", s.c_str());
	}
}

void logToServer(String s)
{
	StaticJsonBuffer<400> jsonBuffer;
	JsonObject &root = jsonBuffer.createObject();
	root["type"] = "log";
	root["msg"] = s;
	String JS;
	root.printTo(JS);
	sendToServer(JS);
}


String utf8ascii(String s)
{
	String r = "";
	char c;
	for (unsigned int i = 0; i < s.length(); i++)
	{
		c = utf8ascii(s.charAt(i));
		if (c != 0)
			r += c;
	}
	return r;
}

void utf8ascii(char *s)
{
	int k = 0;
	char c;
	for (unsigned int i = 0; i < strlen(s); i++)
	{
		c = utf8ascii(s[i]);
		if (c != 0)
			s[k++] = c;
	}
	s[k] = 0;
}

int hexcolorToInt(char upper, char lower)
{
	int uVal = (int)upper;
	int lVal = (int)lower;
	uVal = uVal > 64 ? uVal - 55 : uVal - 48;
	uVal = uVal << 4;
	lVal = lVal > 64 ? lVal - 55 : lVal - 48;
	return uVal + lVal;
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


void reconnect()
{
	String clientId = "AWTRIXController-";
	clientId += String(random(0xffff), HEX);
	//hardwareAnimatedSearch(1, 28, 0);
	if (client.connect(clientId.c_str()))
	{
		client.subscribe("awtrixmatrix/#");
		client.publish("matrixClient", "connected");
        matrix.fillScreen(0,0,0);
	}
}

/*
void ICACHE_RAM_ATTR interruptRoutine()
{
	isr_flag = 1;
}
*/

void flashProgress(unsigned int progress, unsigned int total)
{
	matrix.flashProgress(progress, total);
}

void debuggingWithMatrix(String text)
{
    matrix.setTextToMatrix(true,(byte)0,(byte)255,(byte)50,7,6,text);
}

void saveConfigCallback()
{
	shouldSaveConfig = true;
}

void configModeCallback(WiFiManager *myWiFiManager)
{
    matrix.setTextToMatrix(true,(byte)0,(byte)255,(byte)50,3,6,"Hotspot");
}

void updateMatrix(byte payload[], int length)
{
	if (!ignoreServer)
	{
		int y_offset = 5;
		if (firstStart)
		{
			//hardwareAnimatedCheck(1, 30, 2);
			firstStart = false;
		}

		connectionTimout = millis();

		switch (payload[0])
		{
		case 0:
		{
			//Command 0: DrawText

			//Prepare the coordinates
			uint16_t x_coordinate = int(payload[1] << 8) + int(payload[2]);
			uint16_t y_coordinate = int(payload[3] << 8) + int(payload[4]);

			String myText = "";
			for (int i = 8; i < length; i++)
			{
				char c = payload[i];
				myText += c;
			}

            matrix.setTextToMatrix(true,(byte)payload[5],(byte)payload[6],(byte)payload[7],x_coordinate + 1,y_coordinate + y_offset,utf8ascii(myText));
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
                    matrix.drawPixel(x_coordinate + i, y_coordinate, (uint16_t)colorData[j * width + i]);
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
			matrix.drawCircle(x0_coordinate, y0_coordinate, radius, payload[6], payload[7], payload[8]);
			break;
		}
		case 3:
		{
			//Command 3: FillCircle

			//Prepare the coordinates
			uint16_t x0_coordinate = int(payload[1] << 8) + int(payload[2]);
			uint16_t y0_coordinate = int(payload[3] << 8) + int(payload[4]);
			uint16_t radius = payload[5];
			matrix.fillCircle(x0_coordinate, y0_coordinate, radius, payload[6], payload[7], payload[8]);
			break;
		}
		case 4:
		{
			//Command 4: DrawPixel

			//Prepare the coordinates
			uint16_t x0_coordinate = int(payload[1] << 8) + int(payload[2]);
			uint16_t y0_coordinate = int(payload[3] << 8) + int(payload[4]);
            //matrix.drawPixel(x_coordinate + i, y_coordinate, (uint16_t)colorData[j * width + i]);
			matrix.drawPixel(x0_coordinate, y0_coordinate, payload[5], payload[6], payload[7]);
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
			matrix.drawRect(x0_coordinate, y0_coordinate, width, height, payload[7], payload[8], payload[9]);
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
			matrix.drawLine(x0_coordinate, y0_coordinate, x1_coordinate, y1_coordinate, payload[9], payload[10], payload[11]);
			break;
		}

		case 7:
		{
			//Command 7: FillMatrix
            matrix.fillScreen(payload[1], payload[2], payload[3]);
			break;
		}

		case 8:
		{
			//Command 8: Show
			if (notify){
                matrix.drawPixel(31, 0, 200, 0, 0);
			}
			break;
		}
		case 9:
		{
			//Command 9: Clear
			matrix.clear();
			break;
		}
		case 10:
		{
			//deprecated
			//Command 10: Play
			
  
			dfmp3.setVolume(payload[2]);
			delay(10);
			dfmp3.playMp3FolderTrack(payload[1]);
		
			break;
		}
		case 11:
		{
			//Command 11: reset
			espControl.reset();
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
			if (ldrState != 0)
			{
				//root["LUX"] = photocell.getCurrentLux();
			}
			else
			{
				root["LUX"] = 0;
			}

            root["Temp"] = temperature.getTemperature();
            root["Hum"] = temperature.getHumidity();
            root["hPa"] = temperature.getPresure();

			String JS;
			root.printTo(JS);
			sendToServer(JS);
			break;
		}
		case 13:
		{
			matrix.setBrightness(payload[1]);
			break;
		}
		case 14:
		{
			//tempState = (int)payload[1];
			//audioState = (int)payload[2];
			//gestureState = (int)payload[3];
			ldrState = int(payload[1] << 8) + int(payload[2]);
			matrixTempCorrection = (int)payload[3];
            matrix.setTextToMatrix(true,0,255,50,6,6,"SAVED");
			delay(2000);
			if (saveConfig())
			{
				espControl.reset();
			}
			break;
		}
		case 15:
		{
            matrix.setTextToMatrix(true,0,255,50,6,6,"RESET!");
			delay(1000);
			storage.dropStorage();
			wifiManager.resetSettings();
			espControl.reset();
			break;
		}
		case 16:
		{
			sendToServer("ping");
			break;
		}
		case 17:
		{
			
			//Command 17: Volume
			dfmp3.setVolume(payload[1]);
			break;
		}
		case 18:
		{
			//Command 18: Play
			
			dfmp3.playMp3FolderTrack(payload[1]);
			break;
		}
		case 19:
		{
			//Command 18: Stop
			dfmp3.stopAdvertisement();
			delay(50);
			dfmp3.stop();
			break;
		}
		case 20:
		{
			//change the connection...
			USBConnection = false;
			WIFIConnection = false;
			firstStart = true;
			break;
		}
		case 21:
		{
			//multicolor...
			uint16_t x_coordinate = int(payload[1] << 8) + int(payload[2]);
			uint16_t y_coordinate = int(payload[3] << 8) + int(payload[4]);
			matrix.setCursor(x_coordinate + 1, y_coordinate + y_offset);

			String myJSON = "";
			for (int i = 5; i < length; i++)
			{
				myJSON += (char)payload[i];
			}
			//Serial.println("myJSON: " + myJSON + " ENDE");
			DynamicJsonBuffer jsonBuffer;
			JsonArray &array = jsonBuffer.parseArray(myJSON);
			if (array.success())
			{
				//Serial.println("Array erfolgreich geöffnet... =)");
				for (int i = 0; i < (int)array.size(); i++)
				{
					String tempString = array[i]["t"];
					String colorString = array[i]["c"];
					JsonArray &color = jsonBuffer.parseArray(colorString);
					if (color.success())
					{
						//Serial.println("Color erfolgreich geöffnet... =)");
						String myText = "";
						int r = color[0];
						int g = color[1];
						int b = color[2];
						//Serial.println("Test: " + tempString + " / Color: " + r + "/" + g + "/" + b);
						matrix.setTextColor(r, g, b);
						for (int y = 0; y < (int)tempString.length(); y++)
						{
							myText += (char)tempString[y];
						}
						matrix.print(utf8ascii(myText));
					}
				}
			}
			break;
		}
		case 22:
		{
			//Text
			//scrollSpeed
			//icon
			//color
			//multicolor (textData?)
			//moveIcon
			//repeatIcon
			//duration
			//repeat
			//rainbow
			//progress
			//progresscolor
			//progressBackgroundColor
			//soundfile

			String myJSON = "";
			for (int i = 1; i < length; i++)
			{
				myJSON += (char)payload[i];
			}
			DynamicJsonBuffer jsonBuffer;
			JsonObject &json = jsonBuffer.parseObject(myJSON);

			String tempString = json["text"];
			String colorString = json["color"];

			JsonArray &color = jsonBuffer.parseArray(colorString);
			int r = color[0];
			int g = color[1];
			int b = color[2];
			int scrollSpeed = (int)json["scrollSpeed"];

			Serial.println("Scrollspeed: " + (String)(scrollSpeed));

			int textlaenge;
			while (true)
			{
				matrix.setCursor(32, 6);
				matrix.print(utf8ascii(tempString));
				textlaenge = (int)matrix.getCursorX() - 32;
				for (int i = 31; i > (-textlaenge); i--)
				{
					int starzeit = millis();
                    matrix.setTextToMatrix(true, r, g, b, i, 6, utf8ascii(tempString));

					client.loop();
					int endzeit = millis();
					if ((scrollSpeed + starzeit - endzeit) > 0)
					{
						delay(scrollSpeed + starzeit - endzeit);
					}
				}
				connectionTimout = millis();
				break;
			}
			Serial.println("Textlänge auf Matrix: " + (String)(textlaenge));
			Serial.println("Test: " + tempString + " / Color: " + r + "/" + g + "/" + b);
			break;
		}
		case 23:
		{
			//Command 23: DrawFilledRect

			//Prepare the coordinates
			uint16_t x0_coordinate = int(payload[1] << 8) + int(payload[2]);
			uint16_t y0_coordinate = int(payload[3] << 8) + int(payload[4]);
			int16_t width = payload[5];
			int16_t height = payload[6];
			matrix.fillRect(x0_coordinate, y0_coordinate, width, height, payload[7], payload[8], payload[9]);
			break;
		}
		case 24:
		{
			
			dfmp3.loopGlobalTrack(payload[1]);
			break;
		}
		case 25:
		{
			dfmp3.playAdvertisement(payload[1]);
			break;
		}
		case 26:
		{
			notify=payload[1];
			break;
		}
		}
	}
}

void callback(char *topic, byte *payload, unsigned int length)
{
	WIFIConnection = true;
	updateMatrix(payload, length);
}

void setup(){
    Serial.begin(115200);

    mainboard.init();
    matrix.init(0,0);
    storage.init();
    temperature.init();

    matrix.setTextToMatrix(true,(byte)255,(byte)0,(byte)255,9,6,"BOOT");
    delay(2000);

    wifiManager.setAPStaticIPConfig(IPAddress(172, 217, 28, 1), IPAddress(172, 217, 28, 1), IPAddress(255, 255, 255, 0));
	WiFiManagerParameter custom_awtrix_server("server", "AWTRIX Host", storage.getAwtrixServerChar(), 16);
	WiFiManagerParameter custom_port("Port", "Matrix Port", storage.getPortChar(), 6);
	WiFiManagerParameter custom_matrix_type("matrixType", "MatrixType", "0", 1);
	// Just a quick hint
	WiFiManagerParameter host_hint("<small>AWTRIX Host IP (without Port)<br></small><br><br>");
	WiFiManagerParameter port_hint("<small>Communication Port (default: 7001)<br></small><br><br>");
	WiFiManagerParameter matrix_hint("<small>0: Columns; 1: Tiles; 2: Rows <br></small><br><br>");
	WiFiManagerParameter p_lineBreak_notext("<p></p>");

	wifiManager.setSaveConfigCallback(saveConfigCallback);
	wifiManager.setAPCallback(configModeCallback);

	wifiManager.addParameter(&p_lineBreak_notext);
	wifiManager.addParameter(&host_hint);
	wifiManager.addParameter(&custom_awtrix_server);
	wifiManager.addParameter(&port_hint);
	wifiManager.addParameter(&custom_port);
	wifiManager.addParameter(&matrix_hint);
	wifiManager.addParameter(&custom_matrix_type);
	wifiManager.addParameter(&p_lineBreak_notext);

	//wifiManager.setCustomHeadElement("<style>html{ background-color: #607D8B;}</style>");

	//hardwareAnimatedSearch(0, 24, 0);

	if (!wifiManager.autoConnect("AWTRIX Controller", "awtrixxx"))
	{
		//reset and try again, or maybe put it to deep sleep
		espControl.reset();
		delay(5000);
	}

	//is needed for only one hotpsot!
	WiFi.mode(WIFI_STA);

    if (shouldSaveConfig)
	{
        storage.saveAwtrixServer(custom_awtrix_server.getValue(),false);
        storage.saveMatrixType(atoi(custom_matrix_type.getValue()),false);
        storage.savePort(atoi(custom_port.getValue()),false);
		storage.saveConfig();
		espControl.reset();
	}

    client.setServer(storage.getAwtrixServerChar(), storage.getPort());
	client.setCallback(callback);

	ignoreServer = false;

	connectionTimout = millis();

    //temperature.printReadings();
    //mainboard.controlLED(!mainboard.ledState);

}

void loop()
{
	//server.handleClient();
	//ArduinoOTA.handle();

	//is needed for the server search animation
	if (firstStart && !ignoreServer)
	{
		if (millis() - myTime > 500)
		{
			matrix.serverSearch(myCounter, 0, 28, 0);
			myCounter++;
			if (myCounter == 4)
			{
				myCounter = 0;
			}
			myTime = millis();
		}
	}

	//not during the falsh process
	if (!updating)
	{
		if (USBConnection || firstStart)
		{
			int x = 100;
			while (x >= 0)
			{
				x--;
				//USB
				if (Serial.available() > 0)
				{
					//read and fill in ringbuffer
					myBytes[bufferpointer] = Serial.read();
					messageLength--;
					for (int i = 0; i < 14; i++)
					{
						if ((bufferpointer - i) < 0)
						{
							myPointer[i] = 1000 + bufferpointer - i;
						}
						else
						{
							myPointer[i] = bufferpointer - i;
						}
					}
					//prefix from "awtrix" == 6?
					if (myBytes[myPointer[13]] == 0 && myBytes[myPointer[12]] == 0 && myBytes[myPointer[11]] == 0 && myBytes[myPointer[10]] == 6)
					{
						//"awtrix" ?
						if (myBytes[myPointer[9]] == 97 && myBytes[myPointer[8]] == 119 && myBytes[myPointer[7]] == 116 && myBytes[myPointer[6]] == 114 && myBytes[myPointer[5]] == 105 && myBytes[myPointer[4]] == 120)
						{
							messageLength = (int(myBytes[myPointer[3]]) << 24) + (int(myBytes[myPointer[2]]) << 16) + (int(myBytes[myPointer[1]]) << 8) + int(myBytes[myPointer[0]]);
							SavemMessageLength = messageLength;
							awtrixFound = true;
						}
					}

					if (awtrixFound && messageLength == 0)
					{
						byte tempData[SavemMessageLength];
						int up = 0;
						for (int i = SavemMessageLength - 1; i >= 0; i--)
						{
							if ((bufferpointer - i) >= 0)
							{
								tempData[up] = myBytes[bufferpointer - i];
							}
							else
							{
								tempData[up] = myBytes[1000 + bufferpointer - i];
							}
							up++;
						}
						USBConnection = true;
						updateMatrix(tempData, SavemMessageLength);
						awtrixFound = false;
					}
					bufferpointer++;
					if (bufferpointer == 1000)
					{
						bufferpointer = 0;
					}
				}
				else
				{
					break;
				}
			}
		}
		//Wifi
		if (WIFIConnection || firstStart)
		{
			//Serial.println("wifi oder first...");
			if (!client.connected())
			{
				//Serial.println("nicht verbunden...");
				reconnect();
				if (WIFIConnection)
				{
					USBConnection = false;
					WIFIConnection = false;
					firstStart = true;
				}
			}
			else
			{
				client.loop();
			}
		}

		if (millis() - connectionTimout > 20000)
		{
            Serial.println("Connection Timeout...");
			USBConnection = false;
			WIFIConnection = false;
			firstStart = true;
		}
	}
}