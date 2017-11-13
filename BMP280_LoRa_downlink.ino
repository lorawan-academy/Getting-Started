#include <TheThingsNetwork.h>

// Set your AppEUI and AppKey
const char *appEui = "0000000000000000";
const char *appKey = "00000000000000000000000000000000";

#define loraSerial Serial2
#define debugSerial SerialUSB

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan REPLACE_ME

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// using I2C
Adafruit_BMP280 bme;

void setup()
{
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000)
    ;

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);
 
  // Set callback for incoming messages
  ttn.onMessage(message);
  pinMode(LED_BUILTIN, OUTPUT);

  if (!bme.begin())
  {  
    debugSerial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
}

void loop()
{
  uint16_t temp = getTemperature();

  debugSerial.print("Temperature = ");
  debugSerial.print(bme.readTemperature());
  debugSerial.println(" *C");
  debugSerial.print("Pressure = ");
  debugSerial.print(bme.readPressure() / 100); // 100 Pa = 1 millibar
  debugSerial.println(" mb");

  uint16_t pressure = bme.readPressure() -87000;
  
  byte payload[4];
  payload[0] = highByte(temp);
  payload[1] = lowByte(temp);
  payload[2] = highByte(pressure);
  payload[3] = lowByte(pressure);
  
  ttn.sendBytes(payload, sizeof(payload));

  // Delay between readings - 10 000 = 10 secons
  delay(10000);
}


uint16_t getTemperature()
{
  //10mV per C, 0C is 500mV
  float mVolts = (float)analogRead(TEMP_SENSOR) * 3300.0 / 1023.0;
  int temp = (mVolts - 500) * 10;
  
  debugSerial.print((mVolts - 500) / 10);
  debugSerial.println(" Celcius");
  return int(temp);
}


void message(const uint8_t *payload, size_t size, port_t port)
{
  if (payload[0] == 0) {
    digitalWrite(LED_BUILTIN, LOW);
  }

  else {
    digitalWrite(LED_BUILTIN, HIGH);
  }

}

