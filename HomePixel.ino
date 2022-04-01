/*********************************************************************************
 *  MIT License
 *  
 *  Copyright (c) 2022 Julian N. Robbins
 *  
 *  https://github.com/jools-uk/HomePixel
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *  
 ********************************************************************************/
 
#include <WiFi.h>
#include "HomeSpan.h" 
#include "DEV_FastLEDPixel.h"      
//#include "esp_pm.h"
//#include "esp32-hal-cpu.h"

#define STRIP1_DATA_PIN 9 //6 pin configuration - using Download pin as data pin
//#define STRIP1_DATA_PIN 8 //full configuration - can use any data pin you like!
#define STRIP1_NUM_LEDS 120+30 //Counter mini strip
//#define STRIP1_NUM_LEDS 50 //Mirror fairy lights
//#define STRIP1_NUM_LEDS 100 //Xmas tree fairy lights
//#define STRIP1_NUM_LEDS 64 //Square

void setup() {

  //debug PINS for RMT lowlevel
/*
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
*/

  //Setup FastLEDs here:
  CRGB* stripPixels = (CRGB*) malloc(STRIP1_NUM_LEDS * sizeof(CRGB));
  CLEDController &strip1 = FastLED.addLeds<WS2812B,STRIP1_DATA_PIN,GRB>(stripPixels, STRIP1_NUM_LEDS)
    .setCorrection(TypicalLEDStrip)
    .setDither(true);

  //Set global 100% brightness
  FastLED.setBrightness(255);

  pinMode(STRIP1_DATA_PIN, OUTPUT);
  digitalWrite(STRIP1_DATA_PIN, LOW);
  
  Serial.begin(115200);

  //Following was useful when debug ESP32 RMT module
//  rtc_cpu_freq_config_t cpu_conf;
//  rtc_clk_cpu_freq_get_config(&cpu_conf);
//  Serial.printf("CPU Freq  : %d\n", cpu_conf.freq_mhz);
//  Serial.printf("CPU Source Freq  : %d\n", cpu_conf.source_freq_mhz);
//  Serial.printf("CPU Source  : %d\n", cpu_conf.source); //rtc_cpu_freq_src_t - XTAL,PLL,8M,XTL_D2
//  Serial.printf("CPU Source Div : %d\n", cpu_conf.div);
//  Serial.printf("XTAL Freq : %d\n", rtc_clk_xtal_freq_get());
//  Serial.printf("APB Freq  : %d\n", rtc_clk_apb_freq_get());
//  esp_pm_config_esp32c3_t pm;
//  pm.max_freq_mhz = 160;
//  pm.min_freq_mhz = 80;
//  pm.light_sleep_enable = 0;
//  esp_err_t res = esp_pm_configure(&pm);
//  esp_pm_get_configuration(&pm);
//  Serial.printf("Res: %s\n", esp_err_to_name(res) );
//  Serial.printf("PM Freq Max: %d\n", pm.max_freq_mhz);
//  Serial.printf("PM Freq Min: %d\n", pm.min_freq_mhz);
//  Serial.printf("PM Enable  : %d\n", pm.light_sleep_enable);
  

//  byte mac[6];
//  WiFi.macAddress(mac);

//LOG everything 
  homeSpan.setLogLevel(2);

  homeSpan.setStatusPin(19);
  homeSpan.enableOTA(false);
  
  homeSpan.begin(Category::Lighting,"HomePixel MultiLights");

  new SpanAccessory(); 
    new Service::AccessoryInformation();    
      new Characteristic::Name("StripSolid");    
      new Characteristic::Manufacturer("JoolsNet"); 
      new Characteristic::SerialNumber("123-ABC-10");  
      new Characteristic::Model("ESP32-C3 WS2812B");   
      new Characteristic::FirmwareRevision("0.9");  
      new Characteristic::Identify();               
    new Service::HAPProtocolInformation();          
      new Characteristic::Version("1.1.0");         
    new DEV_FastLEDPixel(strip1);
  
  new SpanAccessory(); 
    new Service::AccessoryInformation();    
      new Characteristic::Name("StripAlgo");    
      new Characteristic::Manufacturer("JoolsNet"); 
      new Characteristic::SerialNumber("123-ABC-11");  
      new Characteristic::Model("ESP32-C3 WS2812B");   
      new Characteristic::FirmwareRevision("0.9");  
      new Characteristic::Identify();               
    new Service::HAPProtocolInformation();          
      new Characteristic::Version("1.1.0");         
    new DEV_FastLEDStripRainbow(strip1);        
} // end of setup()

//////////////////////////////////////

void loop(){
  homeSpan.poll();
} // end of loop()