#include <LiquidCrystal.h>
#include "SdsDustSensor.h"
#include "DHTesp.h"
#include <Wire.h>
#include "Adafruit_SHT31.h"
#define DHTTYPE DHT22

TaskHandle_t Task1;
TaskHandle_t Task2;

class Sensor {
  public:
      bool stat = true;
      String temp;
      String hum;
      
};
Adafruit_SHT31 sht31 = Adafruit_SHT31();
Sensor sensor;
DHTesp dht; 
int dhtPin = 0;
LiquidCrystal lcd(2,4,5,18,19,23);
SdsDustSensor sds(Serial2);

Sensor getTemperature() {
  TempAndHumidity newValues = dht.getTempAndHumidity();
  sensor.temp = String(newValues.temperature,0);
  sensor.hum = String(newValues.humidity,0);
  sht31.begin(0x44);
  if (dht.getStatus() != 0) {
    sensor.stat = false;

  }
  return sensor;
  
}

void setup() {
  Serial.begin(9600); 
  Serial.print("Hello");
  Serial2.begin(9600);
  dht.setup(dhtPin, DHTesp::DHT22);
  lcd.begin(16, 2);
  lcd.print("PM & TEMP/HUM");
  delay(3000);
  lcd.clear();

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
                    Task2code,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
    delay(500); 
}

//Task1code: blinks an LED every 1000 ms
void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  String t;
  String h;
  for(;;){
    t = String(sht31.readTemperature(),0);
    h = String(sht31.readHumidity(),0);
    sensor = getTemperature();
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(sensor.temp);
    lcd.print("/");
    lcd.print(t);
    lcd.print(" H:");
    lcd.print(sensor.hum);
    lcd.print("/");
    lcd.print(h);
    delay(1000);
  } 
}

void Task2code( void * pvParameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    sds.wakeup();
    delay(30000); // working 30 seconds
    PmResult pm = sds.queryPm();
    if (pm.isOk()) {
      lcd.setCursor(0, 1);
      lcd.print("2.5:");lcd.print(pm.pm25,1);lcd.print(" 10:");lcd.print(pm.pm10,1);
      Serial.print("PM2.5 = ");
      Serial.print(pm.pm25);
      Serial.print(", PM10 = ");
      Serial.println(pm.pm10);
      
      // if you want to just print the measured values, you can use toString() method as well
      Serial.println(pm.toString());
    } else {
      Serial.print("Could not read values from sensor, reason: ");
      Serial.println(pm.statusToString());
    }
  
    WorkingStateResult state = sds.sleep();
    if (state.isWorking()) {
      Serial.println("Problem with sleeping the sensor.");
    } else {
      Serial.println("Sensor is sleeping");
      delay(60000); // wait 1 minute
    }
    }
}

void loop() {
  
}
