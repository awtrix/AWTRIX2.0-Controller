///////////////////////// AWTRIX CONFIG /////////////////////////

// Wifi Config

typedef struct {
    const char *ssid = "MPZ-Labor";
    const char *password = "MPZMPZMPZ";
    char *awtrix_server = "192.168.99.101";
} configData_t;

configData_t wifiConfig;

#define USB_CONNECTION
//#define MATRIX_MODEV2

/// LDR Config
#define LDR_RESISTOR 1000 //ohms
#define LDR_PIN A0
#define LDR_PHOTOCELL LightDependentResistor::GL5516