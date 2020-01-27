/*

This file is (c) 2019 by HALaser Systems and released as Public Domain

For more information about the hardware this software can be used on please
refer to https://iot.halaser.de

*/

#include "DHTesp.h"
#include <WiFi.h>
#include <Wire.h>
#include "SSD1306.h"
#include <WiFiClient.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include "data.h"

#define BATTERY_SENSOR_ANALOG_PIN 39
#define LED_PIN                    4
#define WIFI_BUTTON               34

#define MAX_LEVEL 3600 // 2.9V -> 14.4V -> 100%
#define MIN_LEVEL 2200 // 1.95V -> 9.5V -> <10%

#define SAMPLE_FACTOR  20
#define DATA_SIZE     888
#define DSP_TIMEOUT  2000
#define WLAN_TIMEOUT 3500
#define WLAN_ID     "Campy"
#define WLAN_PWD    "" 

static WebServer server(80);
static IPAddress apIP(192,168,1,1);
static SSD1306 display(0x3C,21,22); 
static DHTesp   dht;
 
static uint8_t  batArr[DATA_SIZE+8];
static uint8_t  humArr[DATA_SIZE+8];
static int8_t   tempArr[DATA_SIZE+8];
 
static uint8_t  batArrL[DATA_SIZE+8];
static uint8_t  humArrL[DATA_SIZE+8];
static int8_t   tempArrL[DATA_SIZE+8];
 
static uint8_t  batArrXL[DATA_SIZE+8];
static uint8_t  humArrXL[DATA_SIZE+8];
static int8_t   tempArrXL[DATA_SIZE+8];
 
static uint8_t  cntL,cntXL;
static uint32_t batAvg,humAvg,cntAvg;
static int32_t  dspTimeoutCtr=DSP_TIMEOUT,wlanTimeoutCtr=-1;
static int32_t  tempAvg,uptimeCtr;
static bool     doSaveData=false;
static uint8_t  doWOff=0,doESPOff=0,doUpdateDsp=0;
static uint8_t  wifiButtonPressed=0;

static hw_timer_t * timer = NULL;
static portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
 

static void toggleWifi(void)
{
   if (wlanTimeoutCtr>0) wlanTimeoutCtr=-1;
   else wlanTimeoutCtr=WLAN_TIMEOUT;
   if (wlanTimeoutCtr==WLAN_TIMEOUT)
   {
      Serial.println("WiFi on");
      digitalWrite(LED_PIN,HIGH);
      WiFi.mode(WIFI_AP);
      WiFi.softAP(WLAN_ID,WLAN_PWD);
      WiFi.softAPConfig(apIP,apIP,IPAddress(255,255,255,0));
   }
   else
   {
      Serial.println("WiFi off");
      WiFi.mode(WIFI_OFF); 
      digitalWrite(LED_PIN,LOW);
   }
   doUpdateDsp=1;
}



static void eventTDsp()
{
   server.send(200,"text/html",tdspHTML);
   if (dspTimeoutCtr<0)
   {
      display.displayOn();
      dspTimeoutCtr=DSP_TIMEOUT;
   }
   else
   {
      display.displayOff();
      dspTimeoutCtr=-1;
   }
}


static void eventIndex()
{
   char sendBuffer[2000+1];

   if (dspTimeoutCtr>0)
    snprintf(sendBuffer,2000,indexHTML,batArr[DATA_SIZE-1]/2,tempArr[DATA_SIZE-1],humArr[DATA_SIZE-1]/2,
                                       uptimeCtr/12,"Display ausschalten");
   else
    snprintf(sendBuffer,2000,indexHTML,batArr[DATA_SIZE-1]/2,tempArr[DATA_SIZE-1],humArr[DATA_SIZE-1]/2,
                                       uptimeCtr/12,"Display einschalten");
   // TODO: add retry-later-header
   server.sendHeader("Retry-After","20");
   server.send(200,"text/html",sendBuffer);
}


static void eventWoff()
{
   server.send(200,"text/html",woffHTML);
   doWOff=1;
}


static void eventESPoff()
{
   server.send(200,"text/html",espoffHTML);
   doESPOff=1;
}



static void eventAskESPoff()
{
   server.send(200,"text/html",askespoffHTML);
}


static void drawGraph(void)
{
   char *sendBuffer;
   char  posBuffer[100+1];
   int   i;

   try
   {
   sendBuffer=(char*)malloc(50000);
   if (!sendBuffer) return;
   strncpy(sendBuffer,graphSVG1,50000);
   for (i=0; i<DATA_SIZE; i++)
   {
      if (batArr[i]<255)
      {
         snprintf(posBuffer,100,"%d,%d, ",i+102,(int)(523-(batArr[i]*2.56)));
         strncat(sendBuffer,posBuffer,50000);
      }
   }
   delay(0);
   strncat(sendBuffer,graphSVG2,50000);
   for (i=0; i<DATA_SIZE; i++)
   {
      if (batArr[i]<255) // use batArr to detect validity of temperature
      {
         snprintf(posBuffer,100,"%d,%d, ",i+102,(int)(306-(tempArr[i]*4.0)));
         strncat(sendBuffer,posBuffer,50000);
      }
   }
   delay(0);
   strncat(sendBuffer,graphSVG3,50000);
   for (i=0; i<DATA_SIZE; i++)
   {
      if (humArr[i]<255)
      {
         snprintf(posBuffer,100,"%d,%d, ",i+102,(int)(523-(humArr[i]*2.56)));
         strncat(sendBuffer,posBuffer,50000);
      }
   }
   delay(0);
   strncat(sendBuffer,graphSVG4,50000);
   server.send(200,"image/svg+xml",sendBuffer);
   free(sendBuffer);
   }
   catch (...)
   {
      Serial.println("Graph failed");
   }
}

 
static void drawGraphL(void)
{
   char *sendBuffer;
   char  posBuffer[100+1];
   int   i;

   try
   {
   sendBuffer=(char*)malloc(50000);
   if (!sendBuffer) return;
   strncpy(sendBuffer,graphSVG1L,50000);
   for (i=0; i<DATA_SIZE; i++)
   {
      if (batArrL[i]<255)
      {
         snprintf(posBuffer,100,"%d,%d, ",i+102,(int)(523-(batArrL[i]*2.56)));
         strncat(sendBuffer,posBuffer,50000);
      }
   }
   delay(0);
   strncat(sendBuffer,graphSVG2,50000);
   for (i=0; i<DATA_SIZE; i++)
   {
      if (batArrL[i]<255) // use batArr to detect validity of temperature
      {
         snprintf(posBuffer,100,"%d,%d, ",i+102,(int)(306-(tempArrL[i]*4.0)));
         strncat(sendBuffer,posBuffer,50000);
      }
   }
   delay(0);
   strncat(sendBuffer,graphSVG3,50000);
   for (i=0; i<DATA_SIZE; i++)
   {
      if (humArrL[i]<255)
      {
         snprintf(posBuffer,100,"%d,%d, ",i+102,(int)(523-(humArrL[i]*2.56)));
         strncat(sendBuffer,posBuffer,50000);
      }
   }
   delay(0);
   strncat(sendBuffer,graphSVG4,50000);
   server.send(200,"image/svg+xml",sendBuffer);
   free(sendBuffer);
   }
   catch (...)
   {
      Serial.println("GraphL failed");
   }
}

 
static void drawGraphXL(void)
{
   char *sendBuffer;
   char  posBuffer[100+1];
   int   i;

   try
   {
   sendBuffer=(char*)malloc(50000);
   if (!sendBuffer) return;
   strncpy(sendBuffer,graphSVG1XL,50000);
   for (i=0; i<DATA_SIZE; i++)
   {
      if (batArrXL[i]<255)
      {
         snprintf(posBuffer,100,"%d,%d, ",i+102,(int)(523-(batArrXL[i]*2.56)));
         strncat(sendBuffer,posBuffer,50000);
      }
   }
   delay(0);
   strncat(sendBuffer,graphSVG2,50000);
   for (i=0; i<DATA_SIZE; i++)
   {
      if (batArrXL[i]<255) // use batArr to detect validity of temperature
      {
         snprintf(posBuffer,100,"%d,%d, ",i+102,(int)(306-(tempArrXL[i]*4.0)));
         strncat(sendBuffer,posBuffer,50000);
      }
   }
   delay(0);
   strncat(sendBuffer,graphSVG3,50000);
   for (i=0; i<DATA_SIZE; i++)
   {
      if (humArrXL[i]<255)
      {
         snprintf(posBuffer,100,"%d,%d, ",i+102,(int)(523-(humArrXL[i]*2.56)));
         strncat(sendBuffer,posBuffer,50000);
      }
   }
   delay(0);
   strncat(sendBuffer,graphSVG4,50000);
   server.send(200,"image/svg+xml",sendBuffer);
   free(sendBuffer);
   }
   catch (...)
   {
      Serial.println("GraphXL failed");
   }
}

 
static void handleNotFound()
{
  server.send(404, "text/plain", "File Not Found\n\n");
}


static void updateStorage()
{
   int32_t i;

   if (cntAvg==0) return;
   for (i=1; i<DATA_SIZE; i++)
   {
      tempArr[i-1]=tempArr[i];
      batArr[i-1]=batArr[i];
      humArr[i-1]=humArr[i];
   }
   i=(int32_t)(tempAvg/cntAvg/100.0);
   if ((i>-120) && (i<120))
    tempArr[DATA_SIZE-1]=(int8_t)i;

   i=(int32_t)((batAvg*2.0)/cntAvg);
   if ((i>=0) && (i<=200))
    batArr[DATA_SIZE-1]=(uint8_t)i;

   i=(int32_t)(humAvg/cntAvg/50.0);
   if ((i>=0) && (i<=200))  
    humArr[DATA_SIZE-1]=(uint8_t)i;

   tempAvg=0;
   batAvg=0;
   humAvg=0;
   cntAvg=0;

   cntL++;
   if (cntL>=12)
   {
      int32_t val;
      
      cntL=0;
      for (i=1; i<DATA_SIZE; i++)
      {
         tempArrL[i-1]=tempArrL[i];
         batArrL[i-1]=batArrL[i];
         humArrL[i-1]=humArrL[i];
      }
      val=0;
      for (i=DATA_SIZE-12; i<DATA_SIZE; i++) val+=tempArr[i];
      val/=12;
      tempArrL[DATA_SIZE-1]=val;

      val=0;
      for (i=DATA_SIZE-12; i<DATA_SIZE; i++) val+=batArr[i];
      val/=12;
      batArrL[DATA_SIZE-1]=val;

      val=0;
      for (i=DATA_SIZE-12; i<DATA_SIZE; i++) val+=humArr[i];
      val/=12;
      humArrL[DATA_SIZE-1]=val;

      doSaveData=true;

      cntXL++;
      if (cntXL>=24)
      {
         cntXL=0;
         for (i=1; i<DATA_SIZE; i++)
         {
            tempArrXL[i-1]=tempArrXL[i];
            batArrXL[i-1]=batArrXL[i];
            humArrXL[i-1]=humArrXL[i];
         }
         val=0;
         for (i=DATA_SIZE-24; i<DATA_SIZE; i++) val+=tempArrL[i];
         val/=24;
         tempArrXL[DATA_SIZE-1]=val;

         val=0;
         for (i=DATA_SIZE-24; i<DATA_SIZE; i++) val+=batArrL[i];
         val/=24;
         batArrXL[DATA_SIZE-1]=val;

         val=0;
         for (i=DATA_SIZE-24; i<DATA_SIZE; i++) val+=humArrL[i];
         val/=24;
         humArrXL[DATA_SIZE-1]=val;
      }      
   }
}


static void IRAM_ATTR onTimer() 
{
   portENTER_CRITICAL_ISR(&timerMux);
   updateStorage();
   uptimeCtr++;
   if (dspTimeoutCtr>0) dspTimeoutCtr--;
   if (wlanTimeoutCtr>0) wlanTimeoutCtr--;
   doUpdateDsp=true;
   portEXIT_CRITICAL_ISR(&timerMux);
}



void saveData()
{
Serial.println("Saving data...");
   if (SPIFFS.begin(true))
   {
      File file=SPIFFS.open("/data.bin",FILE_WRITE);
      if (file)
      {
         file.write((const uint8_t*)tempArr,DATA_SIZE);
         file.write(batArr,DATA_SIZE);
         file.write(humArr,DATA_SIZE);
         file.write((const uint8_t*)tempArrL,DATA_SIZE);
         file.write(batArrL,DATA_SIZE);
         file.write(humArrL,DATA_SIZE);
         file.write((const uint8_t*)tempArrXL,DATA_SIZE);
         file.write(batArrXL,DATA_SIZE);
         file.write(humArrXL,DATA_SIZE);
         file.write(&cntL,1);
         file.write(&cntXL,1);
         file.close();
      }
      SPIFFS.end();
   }
   doSaveData=false;
}


static void getData(void)
{
   const  TempAndHumidity temphum=dht.getTempAndHumidity();  
   const  int      aIn=analogRead(BATTERY_SENSOR_ANALOG_PIN);
   const  uint16_t humidity=temphum.humidity*100;
   const  int16_t  temperature=(temphum.temperature*100)-200;
          uint8_t  bat;

   if (aIn<=MIN_LEVEL) bat=0;
   else if (aIn>=MAX_LEVEL) bat=100;
   else bat=(aIn-MIN_LEVEL)/((MAX_LEVEL-MIN_LEVEL)/100.0);

   if (humidity!=65535)
   {
      humAvg+=humidity;
      tempAvg+=temperature;
      batAvg+=bat;
      cntAvg++;
   }
}



void setup() 
{
   int i;

Serial.begin(115200);
Serial.println("Booting");
   dht.setup(13,DHTesp::DHT22);
   for (i=0; i<sizeof(wifiXBM); i++) wifiXBM[i]=~wifiXBM[i];
   for (i=0; i<DATA_SIZE; i++)
   {
      tempArr[i]=0;
      batArr[i]=255;
      humArr[i]=255;

      tempArrL[i]=0;
      batArrL[i]=255;
      humArrL[i]=255;

      tempArrXL[i]=0;
      batArrXL[i]=255;
      humArrXL[i]=255;
   }
Serial.println("Init display...");
   display.init();
   display.flipScreenVertically();
   display.setFont(ArialMT_Plain_24);
   cntL=0;
   cntXL=0;
   if ((SPIFFS.begin(true)) && (SPIFFS.exists("/data.bin")))
   {
      File file=SPIFFS.open("/data.bin",FILE_READ);
      file.readBytes((char*)tempArr,DATA_SIZE);
      file.readBytes((char*)batArr,DATA_SIZE);
      file.readBytes((char*)humArr,DATA_SIZE);
      file.readBytes((char*)tempArrL,DATA_SIZE);
      file.readBytes((char*)batArrL,DATA_SIZE);
      file.readBytes((char*)humArrL,DATA_SIZE);
      file.readBytes((char*)tempArrXL,DATA_SIZE);
      file.readBytes((char*)batArrXL,DATA_SIZE);
      file.readBytes((char*)humArrXL,DATA_SIZE);
      file.readBytes((char*)&cntL,1);
      file.readBytes((char*)&cntXL,1);
      file.close();
      SPIFFS.end();
   }
Serial.println("Data loaded");
   batAvg=0;
   humAvg=0;
   cntAvg=0;
   tempAvg=0;
   uptimeCtr=0;
   doUpdateDsp=1;
Serial.println("Variables");
   getData();
   updateStorage();
   getData();
   updateStorage();

   timer = timerBegin(0, 80, true);
   timerAttachInterrupt(timer, &onTimer, true);
   timerAlarmWrite(timer,3000000*5*SAMPLE_FACTOR,true);  //5 min
   timerAlarmEnable(timer);
Serial.println("Timer");
   pinMode(LED_PIN,OUTPUT);
   pinMode(WIFI_BUTTON,INPUT);
   toggleWifi();
   server.on("/",eventIndex);
   server.on("/espoff.html",eventESPoff);
   server.on("/askespoff.html",eventAskESPoff);
   server.on("/woff.html",eventWoff);
   server.on("/tdsp.html",eventTDsp);
   server.on("/graph.svg",drawGraph);
   server.on("/graphL.svg",drawGraphL);
   server.on("/graphXL.svg",drawGraphXL);
   server.onNotFound(handleNotFound);
   delay(500);
   server.begin();
Serial.println("...done!");
}


void loop() 
{  
   static unsigned int cnt=0;

   if (wlanTimeoutCtr>0) server.handleClient();
   cnt++;
   if (digitalRead(WIFI_BUTTON))
   {
      if (wifiButtonPressed==100)
      {
         if (wlanTimeoutCtr<=0) // turn on the display too when WiFi is off and the button was pressed
         {
            dspTimeoutCtr=DSP_TIMEOUT;
            wlanTimeoutCtr=-1;
            display.displayOn();
            doUpdateDsp=1;
         }
         toggleWifi();
         wifiButtonPressed++;
      }
      else if (wifiButtonPressed<100)
       wifiButtonPressed++;
   }
   else wifiButtonPressed=0;
   if (((wlanTimeoutCtr<=0) && (cnt>5*SAMPLE_FACTOR) && (!digitalRead(WIFI_BUTTON))) ||
       ((wlanTimeoutCtr>0) && (cnt>25000*SAMPLE_FACTOR)))
   {    
      getData();
      cnt=0;
      if (doSaveData)
      {
         saveData();
      }
   }
   if (doWOff>0)
   {
      doWOff++;
      if (doWOff>250)
      {
         doUpdateDsp=1;
         doWOff=0;
         toggleWifi();
      }
   }
   if (doESPOff>0)
   {
      doESPOff++;
      if (doESPOff>250)
      {
         display.displayOff();
         esp_sleep_enable_timer_wakeup((uint64_t)(3600000000*1440*12)); // sleep for 12*30 days
         esp_sleep_enable_ext0_wakeup((gpio_num_t)WIFI_BUTTON,1);
         esp_deep_sleep_start();          
      }
   }

   if (wlanTimeoutCtr==0)
   {
      Serial.println("WiFi timeout");
      toggleWifi();
      wlanTimeoutCtr--;
      doUpdateDsp=1;
   }
   if (dspTimeoutCtr==0)
   {
      Serial.println("Display timeout");
      display.displayOff();
      dspTimeoutCtr--;
   }
   else if (dspTimeoutCtr>0) if (doUpdateDsp)
   {
      doUpdateDsp=0;
      display.clear();
      display.setBrightness(75);
      if (cntAvg>0)
      {
         char txt[50+1];

         display.setTextAlignment(TEXT_ALIGN_LEFT);
         snprintf(txt,50,"%d°C",(uint32_t)((tempAvg/100.0)/cntAvg));
         display.drawString(0,14,txt); // Innentemperatur

         snprintf(txt,50,"%d%%",(uint32_t)((batAvg*2.0)/cntAvg));
         display.drawString(0,38,txt); // Akku
         
         //display.setTextAlignment(TEXT_ALIGN_RIGHT);
         //display.drawString(128,14,"-19°C"); // außentemperatur
         //display.drawString(128,38,"40l");  // Tankinhalt         
      }
      else 
      {
         display.drawString(64,0,"- - -");
      }
       
      display.drawProgressBar(0,0,127-16,7,(int)(((dspTimeoutCtr*100.0)/DSP_TIMEOUT)+0.5));
      display.drawProgressBar(0,7,127-16,7,(int)(((wlanTimeoutCtr*100.0)/WLAN_TIMEOUT)+0.5));
      if (wlanTimeoutCtr>0)
      {
         display.drawXbm(127-13,1,13,13,(const uint8_t*)wifiXBM);
         Serial.println("WiFi Image");
      }
      display.display();
   }
   if ((wlanTimeoutCtr<=0) &&
       (!digitalRead(WIFI_BUTTON))) delay(200);
}
