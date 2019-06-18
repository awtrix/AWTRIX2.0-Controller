///////////////////////// AWTRIX CONFIG /////////////////////////

// Wifi Config

const char *ssid = "22";
const char *password = "";


typedef struct {
    char *awtrix_server = "";
    bool connection = false;    //Wifi = false ...
} configData_t;

configData_t wifiConfig;


bool usbWifi = false; // true = usb...

//#define MATRIX_MODEV2

/// LDR Config
#define LDR_RESISTOR 1000 //ohms
#define LDR_PIN A0
#define LDR_PHOTOCELL LightDependentResistor::GL5516