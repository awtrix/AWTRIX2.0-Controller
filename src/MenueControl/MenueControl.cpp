#include <Arduino.h>
#include "MenueControl.h"
#include <String.h>

String MenueControl::getMenueString(int *zeiger,int *pressedTaster,int *minBrightness,int *maxBrightness){
    String returnString = "fail";
    //next step:
    switch(*pressedTaster){
        case 1:
            if(*zeiger!=0 && *zeiger%10 !=0){
                *zeiger = *zeiger - 1;
            }
            if(*zeiger==120){
                if(*maxBrightness>50){
                    *maxBrightness = *maxBrightness - 5;
                }
            }
            break;
        case 2:
            //middle taster
            if(*zeiger==0){
                *pressedTaster = 0;
                return "Menue";
            }  
            else {
                if(*zeiger<10 || *zeiger == 12){
                    *zeiger = *zeiger * 10;
                }
            }
            break;
        case 3:
            switch(*zeiger){
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                    *zeiger = *zeiger + 1;
                    break;

                case 10:
                case 11:
                    *zeiger = *zeiger + 1;
                    break;
            }
            break;
        default:
            break;
    }


    String tempString = "";
    switch(*zeiger){
			case -1:
			case 0:
				returnString = "Menue";
				break;
			case 1:
				returnString = "Bright";
				break;
			case 10:
				returnString = "auto: XXX";
				break;
			case 11:
                tempString = String(*minBrightness);
				returnString = "min: " + tempString;
				break;
			case 12:
            tempString = String(*maxBrightness);
				returnString = "max: " + tempString;
				break;	
			case 5:
				returnString = "stehen!";
			break;

            case 120:
                tempString = String(*maxBrightness);
                returnString = tempString;
                break;

		}
        *pressedTaster = 0;
    return returnString;
}