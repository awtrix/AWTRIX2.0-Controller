#include "Storage.h"

void Storage::init(){
    if(EEPROM.begin(256)){
        Serial.println("EEPROM successfully initilized...");
    }

    port = (uint16_t)EEPROM.readShort(EEPROM_PORT);
    matrixType = EEPROM.readByte(EEPROM_MATRIX_TYPE);
    matrixCorrection = EEPROM.readByte(EEPROM_MATRIX_CORRECTION);

    char tempChar[15];
    EEPROM.readBytes(EEPROM_AWTRIX_SERVER,tempChar,15);
    awtrixServer = tempChar;
    awtrixServer = awtrixServer.substring(0,15);

    Serial.println("-------------------");
    Serial.println("Get from Storage: ");
    Serial.print("Port: ");
    Serial.println(port);
    Serial.print("MatrixType: ");
    Serial.println(matrixType);
    Serial.print("MatrixCorrection: ");
    Serial.println(matrixCorrection);
    Serial.print("AwtrixServer: ");
    Serial.println(awtrixServer);
    Serial.println("-------------------");
}

bool Storage::saveConfig(){
    EEPROM.writeShort(EEPROM_PORT, port);
    EEPROM.writeShort(EEPROM_MATRIX_TYPE, matrixType);
    EEPROM.writeShort(EEPROM_MATRIX_CORRECTION, matrixCorrection);
    EEPROM.writeString(EEPROM_AWTRIX_SERVER, awtrixServer);
    EEPROM.commit();
}

void Storage::saveAwtrixServer(String newAwtrixServer, bool forceSave){
    awtrixServer = newAwtrixServer;
    if(forceSave){
        char tempChar[15];
        awtrixServer.toCharArray(tempChar,15);
        EEPROM.writeBytes(EEPROM_AWTRIX_SERVER,tempChar,15);
        EEPROM.writeString(EEPROM_AWTRIX_SERVER, awtrixServer);
        EEPROM.commit();
    }
}

void Storage::saveMatrixType(byte newMatrixType, bool forceSave){
    matrixType = newMatrixType;
    if(forceSave){
        EEPROM.writeShort(EEPROM_MATRIX_TYPE, matrixType);
        EEPROM.commit();
    }
}

void Storage::saveMatrixCorrection(byte newMatrixCorrection, bool forceSave){
    matrixCorrection = newMatrixCorrection;
    if(forceSave){
        EEPROM.writeShort(EEPROM_MATRIX_CORRECTION, matrixCorrection);
        EEPROM.commit();
    }
}

void Storage::savePort(uint16_t newPort, bool forceSave){
    port = newPort;
    if(forceSave){
        EEPROM.writeShort(EEPROM_PORT, port);
        EEPROM.commit();
    }
}

String Storage::getAwtrixServer(){
    return awtrixServer;
}

byte Storage::getMatrixType(){
    return matrixType;
}

byte Storage::getMatrixCorrection(){
    return matrixCorrection;
}

uint16_t Storage::getPort(){
    return 0;
}