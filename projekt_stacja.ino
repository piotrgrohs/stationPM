#include <LiquidCrystal.h>
#include "DHTesp.h"
#define DHTTYPE DHT22


class Sensor {
  public:
      bool stat = true;
      String temp;
      String hum;
      
};
Sensor sensor;
DHTesp dht; 
int dhtPin = 17;
LiquidCrystal lcd(4,23,5,18,19,2);


Sensor getTemperature() {
  TempAndHumidity newValues = dht.getTempAndHumidity();
  sensor.temp = String(newValues.temperature);
  sensor.hum = String(newValues.humidity);
  if (dht.getStatus() != 0) {
    sensor.stat = false;
    
  }
  return sensor;
  
}

void setup() { 
  dht.setup(dhtPin, DHTesp::DHT22);
  lcd.begin(16, 2);
  lcd.clear();
  sds.begin(); 
  
}

void loop(){ 
sensor = getTemperature();
delay(1000);
}
