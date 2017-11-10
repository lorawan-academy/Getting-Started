#include <TheThingsNetwork.h>
#include <CayenneLPP.h>

// Set your AppEUI and AppKey
const char *appEui = "0000000000000000";
const char *appKey = "00000000000000000000000000000000";

#define loraSerial Serial2
#define debugSerial SerialUSB

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan REPLACE_ME

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);
CayenneLPP lpp(51);
uint8_t led = 0;

void setup()
{
  loraSerial.begin(57600);
  debugSerial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000);

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);

  // Set callback for incoming messages
  ttn.onMessage(message);
}

void loop() {
  float temp = getTemperature();

  debugSerial.println("-- LOOP");

  lpp.reset();
  lpp.addTemperature(1, temp);
  lpp.addDigitalOutput(2, led); 

  ttn.sendBytes(lpp.getBuffer(), lpp.getSize());

  // Delay between readings
  // 60 000 = 1 minute
  delay(10000);
}


float getTemperature()
{
  //10mV per C, 0C is 500mV
  float mVolts = (float)analogRead(TEMP_SENSOR) * 3300.00 / 1023.00;
  float temp = (mVolts - 500.00) / 10.0;
  
  debugSerial.print((mVolts - 500) / 10);
  debugSerial.println(" Celcius");
  return float(temp);
}



void message(const byte *payload, size_t size, port_t port){
  debugSerial.println("-- MESSAGE");

  if (size != 4 && payload[0] != 2 && payload[1] != 00) {
    debugSerial.println("Order not supported");
  }

  
  if (payload[2] > 0) {
    debugSerial.println("LED ON");
    led = 1;
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    debugSerial.println("LED OFF");
    led = 0;
    digitalWrite(LED_BUILTIN, LOW);
  }

}

