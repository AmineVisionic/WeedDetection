#include "SimulatorProgram.h"


int inputControlPin = 4; // Send High to heat the water
int outputSwitchPin = 7; // Send High to open the switch
int outputControlPin = 8; // Send High when the water is hot
int inputResistanceAnalog = A0; // Send High to heat the water

#define TEMPERATURE_HOT_MIN 75
#define TEMPERATURE_HOT_MAX 85

class RestistanceTemp {
  public:
  RestistanceTemp() {
    resistance=0;
    temp=0;
  };
    float resistance;
    float temp;
  };

#define RESISTANCE_MAP_SIZE 16
RestistanceTemp resistanceMap[RESISTANCE_MAP_SIZE];

void setResistanceMap() {
  resistanceMap[0].resistance=900;
  resistanceMap[0].temp=24;
  resistanceMap[1].resistance=961;
  resistanceMap[1].temp=30;
  resistanceMap[2].resistance=820;
  resistanceMap[2].temp=40;
  resistanceMap[3].resistance=781;
  resistanceMap[3].temp=50;
  resistanceMap[4].resistance=730;
  resistanceMap[4].temp=54;
  resistanceMap[5].resistance=692;
  resistanceMap[5].temp=60;
  resistanceMap[6].resistance=653;
  resistanceMap[6].temp=65;
  resistanceMap[7].resistance=622;
  resistanceMap[7].temp=70;
  resistanceMap[8].resistance=585;
  resistanceMap[8].temp=75;
  resistanceMap[9].resistance=544;
  resistanceMap[9].temp=80;
  resistanceMap[10].resistance=529;
  resistanceMap[10].temp=83;
  resistanceMap[11].resistance=507;
  resistanceMap[11].temp=85;
  resistanceMap[12].resistance=492;
  resistanceMap[12].temp=87;
  resistanceMap[13].resistance=482;
  resistanceMap[13].temp=89;
  resistanceMap[14].resistance=472;
  resistanceMap[14].temp=91;
  resistanceMap[15].resistance=462;
  resistanceMap[15].temp=93;
}

float mapResitanceToTemp(int resistance) {
  if(resistance >= resistanceMap[0].resistance) {
    return resistanceMap[0].temp;
  } else if (resistance <= resistanceMap[RESISTANCE_MAP_SIZE-1].resistance) {
    return resistanceMap[RESISTANCE_MAP_SIZE-1].temp;
  }
  else {
  int mapIndex=0;
    for(int i=0;i<RESISTANCE_MAP_SIZE-1;i++) {
      if (resistance <= resistanceMap[i].resistance && resistance >= resistanceMap[i+1].resistance) {
        mapIndex=i;
        break;
      }
    }
    return resistanceMap[mapIndex].temp + (resistanceMap[mapIndex+1].temp-resistanceMap[mapIndex].temp)*static_cast<float>(resistance-resistanceMap[mapIndex].resistance)/(resistanceMap[mapIndex+1].resistance-resistanceMap[mapIndex].resistance);
  }  
}

void setup() { 
  
  Serial.begin(9600);                           
  inString.reserve(10);

  pinMode(inputControlPin, INPUT);  
  pinMode(inputResistanceAnalog, INPUT);  
  
  pinMode(outputSwitchPin, OUTPUT);  
  pinMode(outputControlPin, OUTPUT);  

  setResistanceMap();
 }

void displayValue(int value) {
  char msg[255];
  sprintf(msg, "%d", value);
  Serialprint(msg);   
}

float getTemperature() {
  int resistance = AnalogRead(inputResistanceAnalog);
  return mapResitanceToTemp(resistance);  
}
 
int checkTemperature() {
  float temperature = getTemperature();
  return (temperature < TEMPERATURE_HOT_MIN) ? -1 : (
            (temperature > TEMPERATURE_HOT_MAX) ? 1 : 0);  
}
 
void loop() { 
  int checkTemperatureResult = checkTemperature();
  // Handle Ready for spray
  if(checkTemperatureResult>=0) {
    DigitalWrite(outputControlPin, HIGH);
  } else {
    DigitalWrite(outputControlPin, LOW);    
  }

  // Handle Control  
  int controlPinStatus = DigitalRead(inputControlPin);  
  if(controlPinStatus == HIGH) {
    if (checkTemperatureResult<=0) {
      DigitalWrite(outputSwitchPin, HIGH);       
    } else {
      DigitalWrite(outputSwitchPin, LOW);              
    }
  } else {
    DigitalWrite(outputSwitchPin, LOW);        
  }        

}
  