#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"

//IR
#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRsend.h>
#define IR_LED 4  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRsend irsend(IR_LED); 

#define LED_ON_IRCODE 0xFFC13E
#define LED_OFF_IRCODE 0xFF41BE
#define LED_BPLUS_IRCODE 0xFF01FE
#define LED_BMINUS_IRCODE 0xFF817E
#define LED_R_IRCODE 0xFF21DE
#define LED_G_IRCODE 0xFFA15E
#define LED_B_IRCODE 0xFF619E
#define LED_W_IRCODE 0xFFE11E
#define LED_FLASH_IRCODE 0xFFD12E
#define LED_STROBE_IRCODE 0xFFF10E
#define LED_FADE_IRCODE 0xFFC936
#define LED_SMOOTH_IRCODE 0xFFE916
#define LED_ORANGE_IRCODE 0xFF11EE

#define TV_ON_IRCODE 0xA90
#define TV_OFF_IRCODE 0xA90
#define TV_VOL_UP_IRCODE 0x490
#define TV_VOL_DOWN_IRCODE 0xC90
#define TV_MUTE_IRCODE 0x290
#define TV_LEFT_IRCODE 0x2D0
#define TV_RIGHT_IRCODE 0xCD0
#define TV_UP_IRCODE 0x2F0
#define TV_DOWN_IRCODE 0xAF0
#define TV_SELECT_IRCODE 0xA70
#define TV_INFO_IRCODE 0x5D0
#define TV_NETFLIX_IRCODE 0x1F58
#define TV_BACK_IRCODE 0x62E9
#define TV_HOME_IRCODE 0x70
#define TV_TVMODE_IRCODE 0x250
#define TV_PLAY_IRCODE 0x2CE9
#define TV_PAUSE_IRCODE 0x4CE9
#define TV_STOP_IRCODE 0xCE9
#define TV_TITLELIST_IRCODE 0x5358
#define TV_NEXTTITLE_IRCODE 0x6CE9
#define TV_PREVTITLE_IRCODE 0x1CE9
#define TV_FASTBACKWARD_IRCODE 0x1EE9
#define TV_FASTFORWARD_IRCODE 0x5EE9
#define TV_RECORD_IRCODE 0x2E9
#define TV_1_IRCODE 0x10
#define TV_2_IRCODE 0x810
#define TV_3_IRCODE 0x410
#define TV_4_IRCODE 0xC10
#define TV_5_IRCODE 0x210
#define TV_6_IRCODE 0xA10
#define TV_7_IRCODE 0x610
#define TV_8_IRCODE 0xE10
#define TV_9_IRCODE 0x110
#define TV_0_IRCODE 0x910
#define TV_ACTIONMENU_IRCODE 0x6923

#define AMPLIFIER_POWER_IRCODE 0xA81
#define AMPLIFIER_VOLUP_IRCODE 0x481
#define AMPLIFIER_VOLDOWN_IRCODE 0xC81
#define AMPLIFIER_CD_MODE_IRCODE 0xA41
#define AMPLIFIER_MDTAPE_MODE_IRCODE 0x961
#define AMPLIFIER_AUX_MODE_IRCODE 0xB81
#define AMPLIFIER_TVSAT_MODE_IRCODE 0x561


// prototypes
boolean connectWifi();

//on/off callbacks 
bool TABLE_LIGHTS_ON_FUNCTION();
bool TABLE_LIGHTS_OFF_FUNCTION();

bool TV_ON_FUNCTION();
bool TV_OFF_FUNCTION();
bool TV_VOL_DOWN_FUNCTION();
bool TV_VOL_UP_FUNCTION();
bool TV_MUTE_FUNCTION();
bool TV_LEFT_FUNCTION();
bool TV_RIGHT_FUNCTION();
bool TV_UP_FUNCTION();
bool TV_DOWN_FUNCTION();
bool TV_SELECT_FUNCTION();
bool TV_BACK_FUNCTION();
bool TV_INFO_FUNCTION();
bool TV_HOME_FUNCTION();
bool TV_NETFLIX_FUNCTION();
bool TV_PLAY_FUNCTION();
bool TV_PAUSE_FUNCTION();

bool LED_ON_FUNCTION();
bool LED_OFF_FUNCTION();
bool LED_BPLUS_FUNCTION();
bool LED_BMINUS_FUNCTION();
bool LED_R_FUNCTION();
bool LED_G_FUNCTION();
bool LED_B_FUNCTION();
bool LED_W_FUNCTION();
bool LED_FLASH_FUNCTION();
bool LED_STROBE_FUNCTION();
bool LED_FADE_FUNCTION();
bool LED_SMOOTH_FUNCTION();
bool LED_ORANGE_FUNCTION();

// Change this before you flash
const char* ssid = "";
const char* password = "";

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;

uint16_t HorizonPower_rawData[78] = {   171,171,19,19,19,19,19,19,19,19,19,57,19,57,19,19,19,19,19,19,19,57,19,19,19,57,19,19,19,57,19,19,19,19,21,171,19,19,19,57,19,19,19,57,19,19,19,19,19,19,19,19,19,19,19,19,19,57,19,19,19,19,19,57,19,19,19,19,19,57,19,57,19,57,19,57,21,226 };


bool isTABLE_LIGHTS_ON = false;
bool isTV_ON = false;
bool isLED_ON = false;
bool isTV_MOVIE_PAUSED = false; 

Switch *TABLE_LIGHTS_ON_OFF = NULL;
Switch *TV_ON_OFF = NULL;
Switch *TV_VOL_UP_DOWN = NULL;
Switch *TV_LEFT_RIGHT = NULL;
Switch *TV_UP_DOWN = NULL;
Switch *TV_SELECT_BACK = NULL;
//Switch *TV_NETFLIX_HOME = NULL;
Switch *TV_MUTE_HOME = NULL;
Switch *LED_ON_OFF = NULL;
Switch *LED_ORANGE_FLASH = NULL;
Switch *TV_PLAY_PAUSE = NULL;
Switch *TV_TVMODE_NETFLIX = NULL;

void setup()
{
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW); 
  Serial.begin(9600);
   
  // Initialise wifi connection
  wifiConnected = connectWifi();
  
  if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your switches here. Max 10
    // Format: Alexa invocation name, local port no, on callback, off callback
  
    TABLE_LIGHTS_ON_OFF = new Switch("TISCHLICHT AN/AUS", 80, TABLE_LIGHTS_ON_FUNCTION, TABLE_LIGHTS_OFF_FUNCTION);
    TV_ON_OFF = new Switch("TV AN/AUS", 81, TV_ON_FUNCTION, TV_OFF_FUNCTION);
    TV_VOL_UP_DOWN = new Switch("TV LAUTER/LEISER", 82, TV_VOL_UP_FUNCTION, TV_VOL_DOWN_FUNCTION);
    TV_LEFT_RIGHT = new Switch("TV LINKS/RECHTS", 83, TV_LEFT_FUNCTION, TV_RIGHT_FUNCTION); 
    TV_UP_DOWN = new Switch("TV HOCH/RUNTER", 84, TV_UP_FUNCTION, TV_DOWN_FUNCTION);  
    TV_SELECT_BACK = new Switch("TV AUSWAHL/ZURÜCK", 85, TV_SELECT_FUNCTION, TV_BACK_FUNCTION); 
    TV_PLAY_PAUSE = new Switch("TV PLAY/PAUSE", 86, TV_PAUSE_FUNCTION, TV_PLAY_FUNCTION); 
    //TV_NETFLIX_HOME = new Switch("TV NETFLIX/HOME", 90, TV_NETFLIX_FUNCTION, TV_HOME_FUNCTION);  
    TV_MUTE_HOME = new Switch("TV STUMM/HOME", 87, TV_MUTE_FUNCTION, TV_HOME_FUNCTION);   
    LED_ON_OFF = new Switch("LED AN/AUS", 88, LED_ON_FUNCTION, LED_OFF_FUNCTION);
    LED_ORANGE_FLASH = new Switch("LED ORANGE/FLASH", 89, LED_ORANGE_FUNCTION, LED_FLASH_FUNCTION);   
    TV_TVMODE_NETFLIX = new Switch("TV TV/NETFLIX", 90, TV_TVMODE_FUNCTION, TV_NETFLIX_FUNCTION);   

    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*TABLE_LIGHTS_ON_OFF);
    upnpBroadcastResponder.addDevice(*TV_ON_OFF);
    upnpBroadcastResponder.addDevice(*TV_VOL_UP_DOWN);  
    upnpBroadcastResponder.addDevice(*TV_LEFT_RIGHT); 
    upnpBroadcastResponder.addDevice(*TV_UP_DOWN);  
    upnpBroadcastResponder.addDevice(*TV_SELECT_BACK);  
    upnpBroadcastResponder.addDevice(*TV_MUTE_HOME);  
    upnpBroadcastResponder.addDevice(*TV_PLAY_PAUSE);
    //upnpBroadcastResponder.addDevice(*TV_NETFLIX_HOME);  
    upnpBroadcastResponder.addDevice(*LED_ON_OFF);
    upnpBroadcastResponder.addDevice(*LED_ORANGE_FLASH);
    upnpBroadcastResponder.addDevice(*TV_TVMODE_NETFLIX);
  }

  //IR
  irsend.begin();
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);  
}
 
void loop()
{
   if(wifiConnected){
    upnpBroadcastResponder.serverLoop();      
    TABLE_LIGHTS_ON_OFF->serverLoop();
    TV_ON_OFF->serverLoop();
    TV_VOL_UP_DOWN->serverLoop();
    TV_LEFT_RIGHT->serverLoop();
    TV_UP_DOWN->serverLoop();
    TV_SELECT_BACK->serverLoop();
    TV_PLAY_PAUSE->serverLoop();
    //TV_NETFLIX_HOME->serverLoop();
    TV_MUTE_HOME->serverLoop();
    LED_ON_OFF->serverLoop();
    LED_ORANGE_FLASH->serverLoop(); 
    TV_TVMODE_NETFLIX->serverLoop(); 
  }
}

bool TABLE_LIGHTS_ON_FUNCTION() {
  Serial.println("Licht an ...");
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  isTABLE_LIGHTS_ON = true;    
  return isTABLE_LIGHTS_ON;
}

bool TABLE_LIGHTS_OFF_FUNCTION() {
  Serial.println("Licht aus ...");
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW); 
  isTABLE_LIGHTS_ON = false;
  return isTABLE_LIGHTS_ON;
}

bool LED_ON_FUNCTION() {
  Serial.println("LED an ...");
  irsend.sendNEC(0xFFC13E, 32);
  isLED_ON = true;    
  return isLED_ON;
}

bool LED_OFF_FUNCTION() {
  Serial.println("LED an ...");
  irsend.sendNEC(0xFF41BE, 32);   
  isLED_ON = true;    
  return isLED_ON;
}

bool LED_BPLUS_FUNCTION() {
  irsend.sendNEC(LED_BPLUS_IRCODE, 32);
  return false;
}

bool LED_BMINUS_FUNCTION() {
  irsend.sendNEC(LED_BMINUS_IRCODE, 32);
  return false;
}

bool LED_R_FUNCTION() {
  irsend.sendNEC(LED_R_IRCODE, 32);
  return false;
}

bool LED_G_FUNCTION() {
  irsend.sendNEC(LED_G_IRCODE, 32);
  return false;
}

bool LED_B_FUNCTION() {
  irsend.sendNEC(LED_B_IRCODE, 32);
  return false;
}

bool LED_W_FUNCTION() {
  irsend.sendNEC(LED_W_IRCODE, 32);
  return false;
}

bool LED_FLASH_FUNCTION() {
  irsend.sendNEC(LED_FLASH_IRCODE, 32);
  return false;
}

bool LED_STROBE_FUNCTION() {
  irsend.sendNEC(LED_STROBE_IRCODE, 32);
  return false;
}

bool LED_FADE_FUNCTION() {
  irsend.sendNEC(LED_FADE_IRCODE, 32);
  return false;
}

bool LED_SMOOTH_FUNCTION() {
  irsend.sendNEC(LED_SMOOTH_IRCODE, 32);
  return false;
}

bool LED_ORANGE_FUNCTION() {
  irsend.sendNEC(LED_ORANGE_IRCODE, 32);
  return false;
}

bool TV_ON_FUNCTION() {
  Serial.println("Fernseher an ...");
  irsend.sendSony(0xa90, 12, 2);
  isTV_ON = true;
  return isTV_ON;
}

bool TV_OFF_FUNCTION() {
  Serial.println("Fernseher aus ...");
  irsend.sendSony(0xa90, 12, 2);
  isTV_ON = false;
  return isTV_ON;
}

bool TV_VOL_DOWN_FUNCTION() {
  irsend.sendSony(TV_VOL_DOWN_IRCODE, 12, 2);
  delay(300);
  irsend.sendSony(TV_VOL_DOWN_IRCODE, 12, 2);
  delay(300);
  irsend.sendSony(TV_VOL_DOWN_IRCODE, 12, 2);
  return false;
}

bool TV_VOL_UP_FUNCTION() {
  irsend.sendSony(TV_VOL_UP_IRCODE, 12, 2);
  delay(300);
  irsend.sendSony(TV_VOL_UP_IRCODE, 12, 2);
  delay(300);
  irsend.sendSony(TV_VOL_UP_IRCODE, 12, 2);
  return false;
}

bool TV_MUTE_FUNCTION() {
  irsend.sendSony(TV_MUTE_IRCODE, 12, 2);
  return false;
}

bool TV_LEFT_FUNCTION() {
  irsend.sendSony(TV_LEFT_IRCODE, 12, 2);
  return false;
}

bool TV_RIGHT_FUNCTION() {
  irsend.sendSony(TV_RIGHT_IRCODE, 12, 2);
  return false;
}

bool TV_UP_FUNCTION() {
  irsend.sendSony(TV_UP_IRCODE, 12, 2);
  return false;
}

bool TV_DOWN_FUNCTION() {
  irsend.sendSony(TV_DOWN_IRCODE, 12, 2);
  return false;
}

bool TV_SELECT_FUNCTION() {
  irsend.sendSony(TV_SELECT_IRCODE, 12, 2);
  return false;
}

bool TV_INFO_FUNCTION() {
  irsend.sendSony(TV_INFO_IRCODE, 12, 2);
  return false;
}

bool TV_NETFLIX_FUNCTION() {
  irsend.sendSony(TV_NETFLIX_IRCODE, 12, 2);
  return false;
}

bool TV_BACK_FUNCTION() {
  irsend.sendSony(TV_BACK_IRCODE, 12, 2);
  return false;
}

bool TV_HOME_FUNCTION() {
  irsend.sendSony(TV_HOME_IRCODE, 12, 2);
  return false;
}

bool TV_TVMODE_FUNCTION() {
  irsend.sendSony(TV_TVMODE_IRCODE, 12, 2);
  return false;
}

bool TV_PLAY_FUNCTION() {
  irsend.sendSony(TV_PLAY_IRCODE, 12, 2);
  isTV_MOVIE_PAUSED = false;
  return isTV_MOVIE_PAUSED;
}

bool TV_PAUSE_FUNCTION() {
  irsend.sendSony(TV_PAUSE_IRCODE, 12, 2);
  isTV_MOVIE_PAUSED = true;
  return isTV_MOVIE_PAUSED;
}

bool HorizonOff() {
  Serial.println("Horizon Box aus ...");
  irsend.sendRaw(HorizonPower_rawData, 78, 38); 
  isTV_ON = false;
  return isTV_ON;
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10){
      state = false;
      break;
    }
    i++;
  }
  
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}




