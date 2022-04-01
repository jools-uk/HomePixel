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

////////////////////////////////////
//   DEVICE-SPECIFIC LED SERVICES //
////////////////////////////////////

//This is important on the ESPC3 - FastLED code is a bit buggy otherwise
#define FASTLED_RMT_BUILTIN_DRIVER 1
#include <FastLED.h>


// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void pride(int num_pixels, CRGB* leds)
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < num_pixels; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (num_pixels-1) - pixelnumber;
    
    nblend(leds[pixelnumber], newcolor, 64);
  }
}

      
//////////////////////////////////

struct DEV_FastLEDPixel : Service::LightBulb {

  int m_numpixels;
  CRGB* m_fastLeds;
  
  SpanCharacteristic *m_power;                   // reference to the On Characteristic
  SpanCharacteristic *m_H;                       // reference to the Hue Characteristic
  SpanCharacteristic *m_S;                       // reference to the Saturation Characteristic
  SpanCharacteristic *m_V;                       // reference to the Brightness Characteristic
  
  
  DEV_NeoPixelStrip(CLEDController &controller) : Service::LightBulb(){       // constructor() method
    //We were passed a controller, so get FastLED info from there
    m_numpixels = controller.size();
    m_fastLeds = controller.leds();
        
    m_power=new Characteristic::On();                    
    m_H=new Characteristic::Hue(0);              // instantiate the Hue Characteristic with an initial value of 0 out of 360
    m_S=new Characteristic::Saturation(0);       // instantiate the Saturation Characteristic with an initial value of 0%
    m_V=new Characteristic::Brightness(100);     // instantiate the Brightness Characteristic with an initial value of 100%
    m_V->setRange(5,100,1);                      // sets the range of the Brightness to be from a min of 5%, to a max of 100%, in steps of 1%
    
//    char cBuf[128];
//    sprintf(cBuf,"Configuring NeoPixel: Pin=(%d)\n",m_pin);
//    Serial.print(cBuf);
    
  } // end constructor

  boolean update(){                         // update() method

    boolean p;
    float v, h, s, r, g, b;

    h=m_H->getVal<float>();                      // get and store all current values.  Note the use of the <float> template to properly read the values
    s=m_S->getVal<float>();
    v=m_V->getVal<float>();                      // though H and S are defined as FLOAT in HAP, V (which is brightness) is defined as INT, but will be re-cast appropriately
    p=m_power->getVal();

    char cBuf[128];
//    sprintf(cBuf,"Updating NeoPixel: Pin=(%d): ",m_pin);
//    LOG1(cBuf);

    if(m_power->updated()){
      p=m_power->getNewVal();
      sprintf(cBuf,"Power=%s->%s, ",m_power->getVal()?"true":"false",p?"true":"false");
    } else {
      sprintf(cBuf,"Power=%s, ",p?"true":"false");
    }
    LOG1(cBuf);
      
    if(m_H->updated()){
      h=m_H->getNewVal<float>();
      sprintf(cBuf,"H=%.0f->%.0f, ",m_H->getVal<float>(),h);
    } else {
      sprintf(cBuf,"H=%.0f, ",h);
    }
    LOG1(cBuf);

    if(m_S->updated()){
      s=m_S->getNewVal<float>();
      sprintf(cBuf,"S=%.0f->%.0f, ",m_S->getVal<float>(),s);
    } else {
      sprintf(cBuf,"S=%.0f, ",s);
    }
    LOG1(cBuf);

    if(m_V->updated()){
      v=m_V->getNewVal<float>();
      sprintf(cBuf,"V=%.0f->%.0f  ",m_V->getVal<float>(),v);
    } else {
      sprintf(cBuf,"V=%.0f  ",v);
    }
    LOG1(cBuf);

    CRGB newcolor = CHSV( (uint8_t)(h/359*255), (uint8_t)(s/100*255), (uint8_t)(v/100*255 * p));
    sprintf(cBuf,"RGB=(%d,%d,%d)\n", newcolor.r, newcolor.g, newcolor.b);
    LOG1(cBuf);
    
    for(int i=0; i<m_numpixels; i++) {
      m_fastLeds[i] = newcolor; 
    }

    FastLED.show();
  
    return(true);
  } // update
};

struct DEV_FastLEDPixelRainbow : Service::LightBulb {
  
  int m_pin;
  int m_numpixels;
  CRGB* m_fastLeds;
  TaskHandle_t taskHandle = NULL;;
  
  SpanCharacteristic *m_power;                        // reference to the On Characteristic
  SpanCharacteristic *m_level;                        // Reference to the Brightness Characteristic instantiated below  
  
  DEV_NeoPixelStripRainbow(CLEDController &controller) : Service::LightBulb(){       // constructor() method
    m_numpixels = controller.size();
    m_fastLeds = controller.leds();

    m_power = new Characteristic::On();                    
    m_level = new Characteristic::Brightness(100);   // Instantiate the Brightness Characteristic 
    m_level->setRange(5,100,1);                      // This sets the range of the Brightness to be from a min of 5%, to a max of 100%, in steps of 1%
 
//    Serial.printf("Configuring NeoPixel: Pin=(%d)\n",m_pin);
    
  } // end constructor

  boolean update(){                         // update() method

    boolean p;
    float l;

    p=m_power->getVal();
    l=m_level->getVal();

    char cBuf[128];
//    sprintf(cBuf,"Updating NeoPixel: Pin=(%d): ",m_pin);
//    LOG1(cBuf);

    if(m_level->updated()){
      l=m_level->getNewVal<float>();
      sprintf(cBuf,"level=%.0f->%.0f  \n",m_level->getVal<float>(),l);
      FastLED.setBrightness(l);
    } else {
      sprintf(cBuf,"level=%.0f  \n",l);
    }
    LOG1(cBuf);

    if(m_power->updated()){
      p=m_power->getNewVal();
      sprintf(cBuf,"Change Power State: Power=%s->%s, \n",m_power->getVal()?"true":"false",p?"true":"false");
      LOG1(cBuf);
      if(p){
        //start task for inner loop, if not already running!
        sprintf(cBuf,"**Powering ON\n");
        LOG1(cBuf);
        if(taskHandle == NULL){
          sprintf(cBuf,"Creating new innerLoopTask\n");
          LOG1(cBuf);
          xTaskCreate(innerLoopTask, "task_name", 2048, this, 1, &taskHandle); //This seems quite a large stack size! 2
        };
      } else {
        sprintf(cBuf,"**Powering OFF\n");
        LOG1(cBuf);
        //end inner loop if running
        if(taskHandle != NULL){
          sprintf(cBuf,"Killing new innerLoopTask\n");
          LOG1(cBuf);
//          vTaskDelete(taskHandle);
          taskHandle = NULL;
        };        
        FastLED.clear(true);
      }
    } else {
      sprintf(cBuf,"No change in Power State: Power=%s, \n",p?"true":"false");
      LOG1(cBuf);
    }
    

    return(true);                               // return true  
  } // update

  static void innerLoopTask(void* pvParameters){
    DEV_NeoPixelStripRainbow* pSelf = (DEV_NeoPixelStripRainbow*)pvParameters;
    while(pSelf->m_power->getVal()){
      pride(pSelf->m_numpixels, pSelf->m_fastLeds);
      FastLED.show();
      vTaskDelay(50/portTICK_PERIOD_MS); 
    }
    vTaskDelete(NULL);
  }
};
