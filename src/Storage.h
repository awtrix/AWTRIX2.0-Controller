#include <Arduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

class Storage
{
    private:
        int EEPROM_PORT = 0;
        int EEPROM_MATRIX_TYPE = 2;
        int EEPROM_MATRIX_CORRECTION = 3;
        int EEPROM_AWTRIX_SERVER = 4;

        String awtrixServer;
        byte matrixType = 0;
        byte matrixCorrection = 0;
        uint16_t port;

    public:
        void init();
        bool saveConfig();
        void saveAwtrixServer(String newAwtrixServer, bool forceSave);
        void saveMatrixType(byte newMatrixType, bool forceSave);
        void saveMatrixCorrection(byte matrixCorrection, bool forceSave);
        void savePort(uint16_t port, bool forceSave);
        String getAwtrixServer();
        byte getMatrixType();
        byte getMatrixCorrection();
        uint16_t getPort();
};