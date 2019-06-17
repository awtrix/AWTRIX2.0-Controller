///////////////////////// AWTRIX CONFIG /////////////////////////

// Wifi Config

typedef struct {
    const char *ssid = "WB Home";
    const char *password = "G00dnight@Sleep";
    char *awtrix_server = "192.168.3.40";
} configData_t;

configData_t wifiConfig;


bool usbWifi = true; // true = usb...

//#define MATRIX_MODEV2

/// LDR Config
#define LDR_RESISTOR 1000 //ohms
#define LDR_PIN A0
#define LDR_PHOTOCELL LightDependentResistor::GL5516