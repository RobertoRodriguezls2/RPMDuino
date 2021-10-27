#include <Arduino.h>
#include <FastLED.h>
#include "BluetoothSerial.h"
#include "ELMduino.h"

#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

TaskHandle_t Task1;
TaskHandle_t Task2;



//*********************************************
/*
 * This is the final working version of the ShiftLight.
 * Works using an I2C display without lag and has little to no flicker 
 * 
 */


#define NUM_LEDS 9
#define BRIGHTNESS 200
#define ELM_PORT   SerialBT
#define DEBUG_PORT Serial
CRGBArray<NUM_LEDS> leds;
BluetoothSerial SerialBT;

ELM327 myELM327;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ 16, /* clock=*/ 15, /* data=*/ 4);
uint32_t rpm = 0;
int minRPM = 700;
int maxRPM = 3500;
int freq = 0;
int maxRepeats = 50;
char cstr[16];
// function prototypes
void Task1code( void * pvParameters );
void Task2code( void * pvParameters );

void setup() {
  Serial.begin(115200);
  u8g2.begin();

#if LED_BUILTIN
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
#endif

  DEBUG_PORT.begin(115200);
  //SerialBT.setPin("1234");
  ELM_PORT.begin("ArduHUD", true);

  if (!ELM_PORT.connect("OBDII"))
  {
    DEBUG_PORT.println("Couldn't connect to OBD scanner - Phase 1");

    while (1);
  }

  if (!myELM327.begin(ELM_PORT, true, 2000))
  {
    Serial.println("Couldn't connect to OBD scanner - Phase 2");

    while (1);
  }

  Serial.println("Connected to ELM327");

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    Task1code,   /* Task function. */
    "Task1",     /* name of task. */
    50000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task1,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */
  delay(500);

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
    Task2code,   /* Task function. */
    "Task2",     /* name of task. */
    50000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task2,      /* Task handle to keep track of created task */
    1);          /* pin task to core 1 */
  delay(500);
}

//Task1code: Handles reading in the data from the obd2 adapter
void Task1code( void * pvParameters ) {
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());



  for (;;) {



    //************************************************** Blutooth code
    float tempRPM = myELM327.rpm();


    if (myELM327.status == ELM_SUCCESS)
    {
      rpm = (uint32_t)tempRPM;
      Serial.print("RPM: "); Serial.println(rpm);
      freq = rpm / 60;

    }
    else
      myELM327.printError();

    vTaskDelay( 5 / portTICK_PERIOD_MS );
  }
}

//Task2code: Handles leds and printing the data
void Task2code( void * pvParameters ) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  FastLED.addLeds<NEOPIXEL, 13>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();



  for (;;) {
    
    int num = rpm;
    char cstr[16];
    itoa(num, cstr, 10);
    u8g2.clearBuffer();
    //u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setFont(u8g2_font_luBS18_tf);
    u8g2.drawStr(0, 24, cstr);
    u8g2.sendBuffer();
    fill_solid( leds, NUM_LEDS, CRGB(0, 0, 0));//**&& these 2 lines cause flickering during the test at home but are smooth while in car
    FastLED.show();//**&& but are required for the display and led to work while at home
    Shift(freq);

  }
}

void loop() {



}



void GreenZone(int rpm)
{
  if (rpm >= 11 && rpm <= 13)
  {
    leds[0] = CRGB::Green;
    FastLED.show();
  }
  if (rpm >= 14 && rpm <= 17)
  {
    leds[0] = CRGB::Green;
    leds[1] = CRGB::Green;
    FastLED.show();
    
  }
  if (rpm >= 18 && rpm <= 24)
  {
    leds[0] = CRGB::Green;
    leds[1] = CRGB::Green;
    leds[2] = CRGB::Green;
    FastLED.show();
  }
  if (rpm >= 25 && rpm <= 28)
  {
    leds[0] = CRGB::Green;
    leds[1] = CRGB::Green;
    leds[2] = CRGB::Green;
    leds[3] = CRGB::Green;
    FastLED.show();
  }
}

void YellowZone(int rpm)
{
  if (rpm >= 29 && rpm <= 32)
  {
    leds[0] = CRGB::Green;
    leds[1] = CRGB::Green;
    leds[2] = CRGB::Green;
    leds[3] = CRGB::Green;
    leds[4] = CRGB::Yellow;
    FastLED.show();
  }
  if (rpm >= 33 && rpm <= 36)
  {
    leds[0] = CRGB::Green;
    leds[1] = CRGB::Green;
    leds[2] = CRGB::Green;
    leds[3] = CRGB::Green;
    leds[4] = CRGB::Yellow;
    leds[5] = CRGB::Yellow;
    FastLED.show();
  }
  if (rpm >= 37 && rpm <= 40)
  {
    leds[0] = CRGB::Green;
    leds[1] = CRGB::Green;
    leds[2] = CRGB::Green;
    leds[3] = CRGB::Green;
    leds[4] = CRGB::Yellow;
    leds[5] = CRGB::Yellow;
    leds[6] = CRGB::Yellow;
    FastLED.show();
  }
}
void RedZone(int rpm)
{
  if (rpm >= 41 && rpm <= 44)
  {
    leds[0] = CRGB::Green;
    leds[1] = CRGB::Green;
    leds[2] = CRGB::Green;
    leds[3] = CRGB::Green;
    leds[4] = CRGB::Yellow;
    leds[5] = CRGB::Yellow;
    leds[6] = CRGB::Yellow;
    leds[7] = CRGB::Red;
    FastLED.show();
  }
  if (rpm >= 45 && rpm <= 49)
  {
    leds[0] = CRGB::Green;
    leds[1] = CRGB::Green;
    leds[2] = CRGB::Green;
    leds[3] = CRGB::Green;
    leds[4] = CRGB::Yellow;
    leds[5] = CRGB::Yellow;
    leds[6] = CRGB::Yellow;
    leds[7] = CRGB::Red;
    leds[8] = CRGB::Red;
    FastLED.show();
  }
  if (rpm >= 50 && rpm <= 53)
  {
    leds[0] = CRGB::Green;
    leds[1] = CRGB::Green;
    leds[2] = CRGB::Green;
    leds[3] = CRGB::Green;
    leds[4] = CRGB::Yellow;
    leds[5] = CRGB::Yellow;
    leds[6] = CRGB::Yellow;
    leds[7] = CRGB::Red;
    leds[8] = CRGB::Red;
    leds[9] = CRGB::Red;
    FastLED.show();
  }
}

void HighZone(int rpm)
{


  if (rpm >= 54)
  {

    fill_solid( leds, NUM_LEDS, CRGB::Purple);
    FastLED.show();
    fill_solid( leds, NUM_LEDS, CRGB::Black);
    FastLED.show();





  }
}

void Shift(int rpm)
{
  GreenZone(rpm);
  YellowZone(rpm);
  RedZone(rpm);
  HighZone(rpm);
}
