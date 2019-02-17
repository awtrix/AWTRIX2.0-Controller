////////////////////////////////////////////////////////////////
///////////////////////// Config begin /////////////////////////
// Wifi Config
const char *ssid = "xxxxx";
const char *password = "xxxxxx";
char *awtrix_server = "192.168.178.39";

/// LDR Config
#define LDR_RESISTOR 10000 //ohms
#define LDR_PIN A0
#define LDR_PHOTOCELL LightDependentResistor::GL5516

/// Matrix Config
#define MATRIX_PIN D2
//uncomment following line to use Matrixtype 2
//#define MATRIX_MODEV2
