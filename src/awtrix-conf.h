///////////////////////// AWTRIX CONFIG /////////////////////////

// Wifi Config
const char *ssid = "xxxx";
const char *password = "xxxx";
char *awtrix_server = "192.168.178.39";

/// LDR Config
#define LDR_RESISTOR 10000 //ohms
#define LDR_PIN A0
#define LDR_PHOTOCELL LightDependentResistor::GL5516

/// Matrix Config
#define MATRIX_PIN D2
//uncomment following line to use Matrixtype 2
//#define MATRIX_MODEV2

// Gesture Config
#define GESTURE false
//Power APDS9960 only with 3.3V!
#define APDS9960_INT    D6
#define APDS9960_SDA    D3
#define APDS9960_SCL    D1

