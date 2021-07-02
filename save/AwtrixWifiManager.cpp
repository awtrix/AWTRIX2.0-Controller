#include "AwtrixWifiManager.h"

WiFiManager wifiManager;
bool static shouldSaveConfig;
char* server = "";
char* port = "";
int type = 0;

static WiFiManagerParameter custom_awtrix_server;
static WiFiManagerParameter custom_port;
static WiFiManagerParameter custom_matrix_type;

bool AwtrixWifiManager::getShouldSaveConfig()
{
    return shouldSaveConfig;
}

char* AwtrixWifiManager::getPort()
{
    return port;
}

void AwtrixWifiManager::configModeCallback(WiFiManager *myWiFiManager)
{
    //matrix.setTextToMatrix(true,(byte)0,(byte)255,(byte)50,3,6,"Hotspot");
}

void AwtrixWifiManager::saveConfigCallback()
{
    strcpy(server, custom_awtrix_server.getValue());
    type =  atoi(custom_matrix_type.getValue());
    strcpy(port, custom_port.getValue());
    
    //saveConfig();
    //ESP.reset();
	shouldSaveConfig = true;
}

String AwtrixWifiManager::getIpAddress()
{   
	return WiFi.localIP().toString();
}

void AwtrixWifiManager::init(char* awtrixServer, char* port){
    shouldSaveConfig = false;
    wifiManager.setAPStaticIPConfig(IPAddress(172, 217, 28, 1), IPAddress(172, 217, 28, 1), IPAddress(255, 255, 255, 0));
    WiFiManagerParameter custom_awtrix_server("server", "AWTRIX Host",awtrixServer , 16);
    WiFiManagerParameter custom_port("Port", "Matrix Port",port , 6);
    WiFiManagerParameter custom_matrix_type("matrixType", "MatrixType", "0", 1);
	// Just a quick hint
	WiFiManagerParameter host_hint("<small>AWTRIX Host IP (without Port)<br></small><br><br>");
	WiFiManagerParameter port_hint("<small>Communication Port (default: 7001)<br></small><br><br>");
	WiFiManagerParameter matrix_hint("<small>0: Columns; 1: Tiles; 2: Rows <br></small><br><br>");
	WiFiManagerParameter p_lineBreak_notext("<p></p>");

	wifiManager.setSaveConfigCallback(this->saveConfigCallback);
	wifiManager.setAPCallback(this->configModeCallback);

	wifiManager.addParameter(&p_lineBreak_notext);
	wifiManager.addParameter(&host_hint);
	wifiManager.addParameter(&custom_awtrix_server);
	wifiManager.addParameter(&port_hint);
	wifiManager.addParameter(&custom_port);
	wifiManager.addParameter(&matrix_hint);
	wifiManager.addParameter(&custom_matrix_type);
	wifiManager.addParameter(&p_lineBreak_notext);

	if (!wifiManager.autoConnect("AWTRIX Controller", "awtrixxx"))
	{
		//espControl.reset();
		delay(5000);
	}
}


