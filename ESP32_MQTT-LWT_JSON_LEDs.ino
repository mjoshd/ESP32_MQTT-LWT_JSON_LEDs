/*
  .______   .______    __    __   __    __          ___      __    __  .___________.  ______   .___  ___.      ___   .___________. __    ______   .__   __.
  |   _  \  |   _  \  |  |  |  | |  |  |  |        /   \    |  |  |  | |           | /  __  \  |   \/   |     /   \  |           ||  |  /  __  \  |  \ |  |
  |  |_)  | |  |_)  | |  |  |  | |  |__|  |       /  ^  \   |  |  |  | `---|  |----`|  |  |  | |  \  /  |    /  ^  \ `---|  |----`|  | |  |  |  | |   \|  |
  |   _  <  |      /  |  |  |  | |   __   |      /  /_\  \  |  |  |  |     |  |     |  |  |  | |  |\/|  |   /  /_\  \    |  |     |  | |  |  |  | |  . `  |
  |  |_)  | |  |\  \-.|  `--'  | |  |  |  |     /  _____  \ |  `--'  |     |  |     |  `--'  | |  |  |  |  /  _____  \   |  |     |  | |  `--'  | |  |\   |
  |______/  | _| `.__| \______/  |__|  |__|    /__/     \__\ \______/      |__|      \______/  |__|  |__| /__/     \__\  |__|     |__|  \______/  |__| \__|

  Thanks much to @corbanmailloux for providing a great framework for implementing flash/fade with HomeAssistant https://github.com/corbanmailloux/esp-mqtt-rgb-led
  
  ################################################################################################################################################################
 
  To use this code you will need the following dependencies: 
  
  - Support for the ESP32 boards. 
    Follow the instructions at https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md
  
  - You will also need to download the follow libraries by going to Sketch -> Include Libraries -> Manage Libraries
    Working as of November 2018:
        FastLED       by Daniel Garcia    v3.2.1      or      FastLED fork by Sam Guyer https://github.com/samguyer/FastLED
        ArduinoJSON   by Benoit Blanchon  v5.13.3
        PubSubClient  by Nick O'Leary     v2.7.0

  - Differences from esp8266 version can be found by searching for lines containing "***ESP8266 > ESP32 Conversion"


  _   _                 _          ____             __ _       
 | | | | __ _ ___ ___  (_) ___    / ___|___  _ __  / _(_) __ _ 
 | |_| |/ _` / __/ __| | |/ _ \  | |   / _ \| '_ \| |_| |/ _` |
 |  _  | (_| \__ \__ \_| | (_) | | |__| (_) | | | |  _| | (_| |
 |_| |_|\__,_|___/___(_)_|\___/   \____\___/|_| |_|_| |_|\__, |
                                                         |___/ 
####################
#configuration.yaml
####################
# Include Files:
automation: !include automations.yaml
group: !include groups.yaml
input_number: !include input_numbers.yaml
input_select: !include input_selects.yaml
light: !include lights.yaml

# enable mqtt
mqtt:
  broker: !secret mqtt_broker_ip
  port: 1883
  keepalive: 60
  username: !secret mqtt_username
  password: !secret mqtt_password
  protocol: 3.1
  birth_message:
    topic: "tele/hass1/LWT"
    payload: "Online"
    qos: 1
    retain: false
  will_message:
    topic: "tele/hass1/LWT"
    payload: "Offline"
    qos: 1
    retain: false



####################
# automations.yaml:
####################
- action:
  - data_template:
      topic: led/2811/back_porch/cmnd
      payload: '{"transition":{{ trigger.to_state.state | int }}}'
    service: mqtt.publish
  alias: LED Animation Speed
  initial_state: true
  hide_entity: false
  condition: []
  id: led_animation_speed
  trigger:
  - entity_id: input_number.led_animation_speed
    platform: state
- action:
  - data_template:
      topic: led/2811/back_porch/cmnd
      payload: '{"state": "ON", "effect":"{{ trigger.to_state.state | string }}"}'
    service: mqtt.publish
  alias: LED Effect
  initial_state: true
  hide_entity: true
  condition: []
  id: led_effect
  trigger:
  - entity_id: input_select.led_effect
    platform: state
- id: maintain_led_effect_dropdown
  alias: Maintain LED Effect Dropdown
  trigger:
  - entity_id: light.back_porch_leds
    platform: state
  condition: []
  action:
  - data_template:
      entity_id: input_select.led_effect
      option: '{{ state_attr("light.back_porch_leds", "effect") }}'
    service: input_select.select_option
- id: led_locator
  alias: LED Locator
  trigger:
  - entity_id: input_number.led_location
    platform: state
  condition: []
  action:
  - data_template:
      topic: led/2811/back_porch/cmnd
      payload: '{"location":{{ trigger.to_state.state | int }}}'
    service: mqtt.publish



####################
# groups.yaml
####################
Back Porch LEDs:
  - light.back_porch_leds
  - input_number.led_animation_speed
  - input_select.led_effect
  - input_number.led_location
  


####################
# input_numbers.yaml
####################
led_animation_speed:
  name: LED Animation Speed
  initial: 150
  min: 1
  max: 150
  step: 10

led_location:
  name: LED Location
  initial: 0
  min: 0
  max: 500
  step: 1
  


####################
# input_selects.yaml
####################
led_effect:
  name: LED Effect
  initial: Solid
  options:
    - "BPM"
    - "Candy Cane"
    - "Confetti"
    - "Cylon Rainbow"
    - "Dots" 
    - "Fire"
    - "Glitter"
    - "Juggle"
    - "Lightning"
    - "Noise"
    - "Police All"
    - "Police One"
    - "Rainbow"
    - "Rainbow With Glitter"
    - "Ripple"
    - "Sinelon"
    - "Solid"
    - "Twinkle"
    - "Christmas"
    - "Thanksgiving"
    - "Turkey Day"
    - "Halloween"
    - "Pumpkin"
    - "Halloween Eyes"
    - "Rainbow Cycle"
    - "LED Locator"



####################
# lights.yaml:
####################
- platform: mqtt_json
  name: "Back Porch LEDs"
  state_topic: "led/2811/back_porch"
  command_topic: "led/2811/back_porch/cmnd"
  availability_topic: "led/2811/back_porch/lwt"
  payload_not_available: "Offline"
  payload_available: "Online"
  optimistic: false
  brightness: true
  color_temp: true
  effect: true
  flash: true
  rgb: true
  qos: 1
  effect_list:
    - "BPM"
    - "Candy Cane"
    - "Confetti"
    - "Cylon Rainbow"
    - "Dots" 
    - "Fire"
    - "Glitter"
    - "Juggle"
    - "Lightning"
    - "Noise"
    - "Police All"
    - "Police One"
    - "Rainbow"
    - "Rainbow With Glitter"
    - "Ripple"
    - "Sinelon"
    - "Solid"
    - "Twinkle"
    - "Christmas"
    - "Thanksgiving"
    - "Turkey Day"
    - "Halloween"
    - "Pumpkin"
    - "Halloween Eyes"
    - "Rainbow Cycle"
    - "LED Locator"

*/




//#define FASTLED_INTERRUPT_RETRY_COUNT 0 ***ESP8266 > ESP32 Conversion  https://github.com/FastLED/FastLED/issues/367#issuecomment-317574376

#include <ArduinoJson.h>
#include <esp_wifi.h>    //#include <ESP8266WiFi.h> ***ESP8266 > ESP32 Conversion
#include <PubSubClient.h>
#include <ESPmDNS.h>     //#include <ESP8266mDNS.h> ***ESP8266 > ESP32 Conversion
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "FastLED.h"

/************ WIFI and MQTT Information (CHANGE THESE FOR YOUR SETUP) ******************/
#define WIFI_SSID           "wifi ssid"                  //<----- change me
#define WIFI_PASSWORD       "wifi password"              //<----- change me
#define MQTT_SERVER         "mqtt broker ip"             //<----- change me
#define MQTT_USERNAME       "mqtt username"              //<----- change me
#define MQTT_PASSWORD       "mqtt password"              //<----- change me
#define MQTT_PORT           1883



/**************************** FOR OTA **************************************************/
#define HOSTNAME            "led-2811-back_porch"        //<----- change me
#define OTA_PASSWORD        "mqtt ota password"          //<----- change me
#define OTA_PORT            8266



/************* MQTT TOPICS (change these topics as you wish)  **************************/
#define STATE_TOPIC_LIGHT   "led/2811/back_porch"        //<----- change me
#define STATE_TOPIC_RETAIN  false

#define CMND_TOPIC_LIGHT    "led/2811/back_porch/cmnd"   //<----- change me

#define LWT_TOPIC_LIGHT     "led/2811/back_porch/lwt"    //<----- change me
#define LWT_QOS             1
#define LWT_RETAIN          true
#define LWT_OFFLINE         "Offline"
#define LWT_ONLINE          "Online"

#define LWT_TOPIC_HASSIO    "tele/hass1/LWT"
#define LWT_ONLINE_HASSIO   "Online"

#define MQTT_CLEANSESSION   true

/****************************************FOR JSON***************************************/
const int BUFFER_SIZE = JSON_OBJECT_SIZE(10);
#define MQTT_MAX_PACKET_SIZE 512



/*********************************** FastLED Defintions ********************************/
#define NUM_PIXELS          150
#define DATA_PIN            18       // 5 ***ESP8266 > ESP32 Conversion
#define CHIPSET             WS2811
//#define COLOR_ORDER       BRG     // led strip
#define COLOR_ORDER         RGB     // led string
#define MESSAGE_LED         17      // 2 = blue led on esp32 (may cause connectivity issues)



/******************************** GLOBALS for fade/flash *******************************/
struct CRGB leds[NUM_PIXELS];
const char* on_cmd = "ON";
const char* off_cmd = "OFF";
const char* effect = "Solid";
String effectString = "Solid";
String oldeffectString = "Solid";

byte realRed = 0;
byte realGreen = 0;
byte realBlue = 0;
byte red = 255;
byte green = 255;
byte blue = 255;
byte brightness = 255;


bool stateOn = false;
bool startFade = false;
bool onbeforeflash = false;
unsigned long lastLoop = 0;
//int transitionTime = 0;
int transitionTime = 60;  /* setting initial animation speed */
int effectSpeed = 0;
bool inFade = false;
int loopCount = 0;
int stepR, stepG, stepB;
int redVal, grnVal, bluVal;
int color_temp;

int newLocator, oldLocator;     /* From The Hookup */

bool flash = false;
bool startFlash = false;
int flashLength = 0;
unsigned long flashStartTime = 0;
byte flashRed = red;
byte flashGreen = green;
byte flashBlue = blue;
byte flashBrightness = brightness;



/********************************** GLOBALS for EFFECTS ******************************/
//RAINBOW
uint8_t thishue = 0;                                          // Starting hue value.
uint8_t deltahue = 10;

//CANDYCANE
CRGBPalette16 currentPalettestriped; //for Candy Cane
CRGBPalette16 gPal; //for fire

//NOISE
static uint16_t dist;         // A random number for our noise generator.
uint16_t scale = 30;          // Wouldn't recommend changing this on the fly, or the animation will be really blocky.
uint8_t maxChanges = 48;      // Value for blending between palettes.
CRGBPalette16 targetPalette(OceanColors_p);
CRGBPalette16 currentPalette(CRGB::Black);

//TWINKLE
#define DENSITY     80
int twinklecounter = 0;

//RIPPLE
uint8_t colour;                                               // Ripple colour is randomized.
int center = 0;                                               // Center of the current ripple.
int step = -1;                                                // -1 is the initializing step.
uint8_t myfade = 255;                                         // Starting brightness.
#define maxsteps 16                                           // Case statement wouldn't allow a variable.
uint8_t bgcol = 0;                                            // Background colour rotates.
int thisdelay = 20;                                           // Standard delay value.

//DOTS
uint16_t   count =   0;                                        // Count up to 65535 (uint8_t limited to 255) and then reverts to 0
uint16_t fadeval = 224;                                        // Trail behind the LED's. Lower => faster fade.
uint16_t bpm = 30;

//LIGHTNING
uint8_t frequency = 50;                                       // controls the interval between strikes
uint8_t flashes = 8;                                          //the upper limit of flashes per strike
unsigned int dimmer = 1;
uint8_t ledstart;                                             // Starting location of a flash
uint8_t ledlen;
int lightningcounter = 0;

//FUNKBOX
int idex = 0;                //-LED INDEX (0 to NUM_PIXELS-1
int TOP_INDEX = int(NUM_PIXELS / 2);
int thissat = 255;           //-FX LOOPS DELAY VAR
uint8_t thishuepolice = 0;
int antipodal_index(int i) {
  int iN = i + TOP_INDEX;
  if (i >= TOP_INDEX) {
    iN = ( i + TOP_INDEX ) % NUM_PIXELS;
  }
  return iN;
}

//FIRE
#define COOLING  55
#define SPARKING 120
bool gReverseDirection = false;

//BPM
uint8_t gHue = 0;

/* Begin From Dr. Zzs */
/*Gradient palette "Orange_to_Purple_gp", originally from
 * http://soliton.vm.bytemark.co.uk/pub/cpt-city/ds/icons/tn/Orange-to-Purple.png.index.html
 * converted for FastLED with gammas (2.6, 2.2, 2.5)
 * Size: 12 bytes of program space.
 */
DEFINE_GRADIENT_PALETTE( Orange_to_Purple_gp ) {
    0, 208, 50,   1,
  127, 146, 27,  45,
  255,  97, 12, 178
};


/* Gradient palette "bhw2_thanks_gp", originally from
 * http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw2/tn/bhw2_thanks.png.index.html
 * converted for FastLED with gammas (2.6, 2.2, 2.5)
 * Size: 36 bytes of program space.
 */
DEFINE_GRADIENT_PALETTE(bhw2_thanks_gp){
    0,   9,  5, 1,
   48,  25,  9, 1,
   76, 137, 27, 1,
   96,  98, 42, 1,
  124, 144, 79, 1,
  153,  98, 42, 1,
  178, 137, 27, 1,
  211,  23,  9, 1,
  255,   9,  5, 1
};


/* Gradient palette "bhw2_xmas_gp", originally from
 * http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw2/tn/bhw2_xmas.png.index.html
 * converted for FastLED with gammas (2.6, 2.2, 2.5)
 * Size: 48 bytes of program space.
 */
DEFINE_GRADIENT_PALETTE(bhw2_xmas_gp){
    0,   0,  12,  0,
   40,   0,  55,  0,
   66,   1, 117,  2,
   77,   1,  84,  1,
   81,   0,  55,  0,
  119,   0,  12,  0,
  153,  42,   0,  0,
  181, 121,   0,  0,
  204, 255,  12,  8,
  224, 121,   0,  0,
  244,  42,   0,  0,
  255,  42,   0,  0
};

CRGBPalette16 ThxPalettestriped;        //for Thanksgiving
CRGBPalette16 HalloweenPalettestriped;  //for Halloween
CRGBPalette16 HJPalettestriped;         //for Holly Jolly
/* End From Dr. Zzs */

String sTopic, sMessage;
WiFiClient espClient;
PubSubClient client(espClient);



/********************************** START SETUP*****************************************/
/*** begin standard setup -*/
void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.println("---------- setup() starting ----------");
/*- pause standard setup -*/


  pinMode(MESSAGE_LED, OUTPUT);
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_PIXELS);
  setupStripedPalette( CRGB::Red, CRGB::Red, CRGB::White, CRGB::White); //for CANDY CANE
  gPal = HeatColors_p; //for FIRE

/* Begin From Dr. Zzs */
  setupThxPalette( CRGB::OrangeRed, CRGB::Olive, CRGB::Maroon, CRGB::Maroon); //for Thanksgiving
  setupHalloweenPalette( CRGB::DarkOrange, CRGB::DarkOrange, CRGB::Indigo, CRGB::Indigo); //for Halloween
  setupHJPalette( CRGB::Red, CRGB::Red, CRGB::Green, CRGB::Green); //for Holly Jolly
/* End From Dr. Zzs */

/*- continue standard setup -*/
  ArduinoOTA.setPort(OTA_PORT);
  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.setPassword((const char *)OTA_PASSWORD);
  Serial.println("");
  Serial.println("Starting Node named " + String(HOSTNAME));
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
  ArduinoOTA.onStart([]() {
    Serial.println("Starting");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.println("");
  Serial.println("---------- setup() complete ----------");
  Serial.println("");
  reconnect();

/*- end standard setup ***/
}


/********************************** START SETUP WIFI*****************************************/
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println("");
  Serial.println("----- setup_wifi() starting -----");
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//  WiFi.setSleepMode(WIFI_NONE_SLEEP);   //esp8266 ***ESP8266 > ESP32 Conversion
  esp_wifi_set_ps(WIFI_PS_NONE);          //esp32   ***ESP8266 > ESP32 Conversion  https://esp-idf.readthedocs.io/en/latest/api-reference/wifi/esp_wifi.html#_CPPv215esp_wifi_set_ps14wifi_ps_type_t

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("----- setup_wifi() complete -----");
  Serial.println("");
}

/*
  SAMPLE PAYLOAD:
  {
    "brightness": 120,
    "color": {
      "r": 255,
      "g": 100,
      "b": 100
    },
    "flash": 2,
    "transition": 5,
    "state": "ON"
  }
*/



/********************************** START CALLBACK*****************************************/
void callback(char* topic, byte* payload, unsigned int length) {

  // blink MESSAGE_LED when message is received
  digitalWrite(MESSAGE_LED, HIGH);
  delay(150);       // delay in ms
  digitalWrite(MESSAGE_LED, LOW);
    
//  Serial.print("Message arrived [");
//  Serial.print(topic);
//  Serial.print("] ");

  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
//  Serial.println(message);

  sTopic = String(topic);
  sMessage = String(message);
  Serial.print( String("Received MQTT message: ") );
  Serial.println( sTopic + String(", ") + sMessage);
  
  if (!processJson(message)) {
    return;
  }

  if (stateOn) {
    realRed = map(red, 0, 255, 0, brightness);
    realGreen = map(green, 0, 255, 0, brightness);
    realBlue = map(blue, 0, 255, 0, brightness);
  }else{
    realRed = 0;
    realGreen = 0;
    realBlue = 0;
  }

//  Serial.println(effect);

  startFade = true;
  inFade = false; // Kill the current fade

  /*  Re-send "Online" status when Home Assistant reports "Online" 
   *  - may not be needed due to LWT message being retained 
   *  - may be more useful when using MQTT discovery (this sketch does not)
  */
  if ((sTopic==LWT_TOPIC_HASSIO)&&(sMessage==LWT_ONLINE_HASSIO)){
    client.publish(LWT_TOPIC_LIGHT, LWT_ONLINE, LWT_RETAIN);
    Serial.print( String("Published MQTT message: ") + LWT_TOPIC_LIGHT + String(", ") );
    Serial.println( LWT_ONLINE + String(", ") + LWT_RETAIN );
    SEND_STATE();
  }else{
    SEND_STATE();
  }

/*************************** Testing Wireless Reboot ************************************/
  /* Reboot command */
  if ((sTopic==CMND_TOPIC_LIGHT)&&(sMessage=="reboot")){
    SOFTWARE_REBOOT();
  }
/*************************** Testing Wireless Reboot ************************************/
}



/********************************** START PROCESS JSON*****************************************/
bool processJson(char* message) {
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(message);

  if (!root.success()) {
    Serial.println("parseObject() failed, message was not JSON formatted.");
//    return false;
  }
/*************************** Testing Wireless Reboot ************************************/
  if (root.containsKey("reboot")) {
    SOFTWARE_REBOOT();
  }
/*************************** Testing Wireless Reboot ************************************/

  if (root.containsKey("state")){
    if (strcmp(root["state"], on_cmd) == 0){
      stateOn = true;
    }else if(strcmp(root["state"], off_cmd) == 0){
      stateOn = false;
      onbeforeflash = false;
      setColor(0, 0, 0);
    }
  }

  // If "flash" is included, treat RGB and brightness differently
  if(root.containsKey("flash")){
    flashLength = (int)root["flash"] * 1000;
    oldeffectString = effectString;

    if(root.containsKey("brightness")){
      flashBrightness = root["brightness"];
    }else{
      flashBrightness = brightness;
    }

    if(root.containsKey("color")){
      flashRed = root["color"]["r"];
      flashGreen = root["color"]["g"];
      flashBlue = root["color"]["b"];
    }else{
      flashRed = red;
      flashGreen = green;
      flashBlue = blue;
    }

    if(root.containsKey("effect")){
      effect = root["effect"];
      effectString = effect;
      twinklecounter = 0; //manage twinklecounter
    }

    if(root.containsKey("transition")){
      transitionTime = root["transition"];
    }else if( effectString == "Solid"){
      transitionTime = 0;
    }
    
    flashRed = map(flashRed, 0, 255, 0, flashBrightness);
    flashGreen = map(flashGreen, 0, 255, 0, flashBrightness);
    flashBlue = map(flashBlue, 0, 255, 0, flashBrightness);
    flash = true;
    startFlash = true;
    
  }else{ // Not flashing
    flash = false;
    
    if(stateOn){   //if the light is turned on and the light isn't flashing
      onbeforeflash = true;
    }

    if(root.containsKey("color")){
      red = root["color"]["r"];
      green = root["color"]["g"];
      blue = root["color"]["b"];
    }
    
    if(root.containsKey("color_temp")){
      //temp comes in as mireds, need to convert to kelvin then to RGB
      color_temp = root["color_temp"];
      unsigned int kelvin  = 1000000 / color_temp;
      temp2rgb(kelvin);
    }

    if(root.containsKey("brightness")){
      brightness = root["brightness"];
    }

    if(root.containsKey("effect")){
      effect = root["effect"];
      effectString = effect;
      twinklecounter = 0; //manage twinklecounter
    }

    if(root.containsKey("transition")){
      transitionTime = root["transition"];
    }else if(effectString == "Solid"){
      transitionTime = 0;
    }

    if(root.containsKey("location")){
      newLocator = root["location"];    /* From The Hookup */
    }

  }

  return true;
}



/********************************** START REBOOT *****************************************/
void SOFTWARE_REBOOT() {
  Serial.print("Rebooting now.");
  ESP.restart();      //ESP.reset(); ***ESP8266 > ESP32 Conversion
}



/********************************** START SEND STATE*****************************************/
void SEND_STATE() {
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["state"] = (stateOn) ? on_cmd : off_cmd;
  JsonObject& color = root.createNestedObject("color");
  color["r"] = red;
  color["g"] = green;
  color["b"] = blue;
  root["brightness"] = brightness;
  root["color_temp"] = color_temp;
  root["effect"] = effectString.c_str();
  char buffer[root.measureLength() + 1];
  root.printTo(buffer, sizeof(buffer));
  client.publish(STATE_TOPIC_LIGHT, buffer, STATE_TOPIC_RETAIN);
  Serial.print(String("Published MQTT message: ") + STATE_TOPIC_LIGHT);
  Serial.println(String(", ") + buffer + String(", ") + STATE_TOPIC_RETAIN);
}



/********************************** START RECONNECT*****************************************/
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection... ");
    // Attempt to connect
    if(client.connect(HOSTNAME, MQTT_USERNAME, MQTT_PASSWORD, LWT_TOPIC_LIGHT, LWT_QOS, LWT_RETAIN, LWT_OFFLINE, MQTT_CLEANSESSION)) {
      Serial.println("MQTT Broker connection established");
      client.subscribe(LWT_TOPIC_HASSIO);
      client.subscribe(CMND_TOPIC_LIGHT);
      client.publish(LWT_TOPIC_LIGHT, LWT_ONLINE, LWT_RETAIN);
      setColor(0, 0, 0);
      SEND_STATE();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



/********************************** START Set Color*****************************************/
void setColor(int inR, int inG, int inB) {
  for(int i = 0; i < NUM_PIXELS; i++){
    leds[i].red   = inR;
    leds[i].green = inG;
    leds[i].blue  = inB;
  }
  FastLED.show();

  Serial.print("Setting LEDs: ");
  Serial.print("r: ");
  Serial.print(inR);
  Serial.print(", g: ");
  Serial.print(inG);
  Serial.print(", b: ");
  Serial.println(inB);
}



/********************************** START MAIN LOOP*****************************************/
void loop() {

  if(!client.connected()){
    reconnect();
  }

  if(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("");
    Serial.println("WIFI Disconnected. Attempting reconnection.");
    setup_wifi();
    return;
  }

  client.loop();

  ArduinoOTA.handle();


  //EFFECT BPM
  if (effectString == "BPM") {
    uint8_t BeatsPerMinute = 62;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
    for ( int i = 0; i < NUM_PIXELS; i++) { //9948
      leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
    }
    if (transitionTime == 0 or transitionTime == NULL) {
      transitionTime = 30;
    }
    showleds();
  }


  //EFFECT Candy Cane
  if (effectString == "Candy Cane") {
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* higher = faster motion */
    fill_palette( leds, NUM_PIXELS,
                  startIndex, 16, /* higher = narrower stripes */
                  currentPalettestriped, 255, LINEARBLEND);
    if (transitionTime == 0 or transitionTime == NULL) {
      transitionTime = 0;
    }
    showleds();
  }


  //EFFECT CONFETTI
  if (effectString == "Confetti" ) {
    fadeToBlackBy( leds, NUM_PIXELS, 25);
    int pos = random16(NUM_PIXELS);
    leds[pos] += CRGB(realRed + random8(64), realGreen, realBlue);
    if (transitionTime == 0 or transitionTime == NULL) {
      transitionTime = 30;
    }
    showleds();
  }


  //EFFECT CYLON RAINBOW
  if (effectString == "Cylon Rainbow") {                    //Single Dot Down
    static uint8_t hue = 0;
    // First slide the led in one direction
    for (int i = 0; i < NUM_PIXELS; i++) {
      // Set the i'th led to red
      leds[i] = CHSV(hue++, 255, 255);
      // Show the leds
      showleds();
      // now that we've shown the leds, reset the i'th led to black
      // leds[i] = CRGB::Black;
      fadeall();
      // Wait a little bit before we loop around and do it again
      //delay(5);  // delay(10);  ***ESP8266 > ESP32 Conversion
    }
    for (int i = (NUM_PIXELS) - 1; i >= 0; i--) {
      // Set the i'th led to red
      leds[i] = CHSV(hue++, 255, 255);
      // Show the leds
      showleds();
      // now that we've shown the leds, reset the i'th led to black
      // leds[i] = CRGB::Black;
      fadeall();
      // Wait a little bit before we loop around and do it again
      //delay(5);  // delay(10);  ***ESP8266 > ESP32 Conversion
    }
    // view cycles of animation executed between mqtt resets
    if (stateOn == true){
      Serial.print("----- ");
      Serial.print(effectString);
      Serial.println(" animation cycle complete -----");
    }
  }


  //EFFECT DOTS
  if (effectString == "Dots") {
    uint16_t inner = beatsin16(bpm, NUM_PIXELS / 4, NUM_PIXELS / 4 * 3);
    uint16_t outer = beatsin16(bpm, 0, NUM_PIXELS - 1);
    uint16_t middle = beatsin16(bpm, NUM_PIXELS / 3, NUM_PIXELS / 3 * 2);
    leds[middle] = CRGB::Purple;
    leds[inner] = CRGB::Blue;
    leds[outer] = CRGB::Aqua;
    nscale8(leds, NUM_PIXELS, fadeval);

    if (transitionTime == 0 or transitionTime == NULL) {
      transitionTime = 30;
    }
    showleds();
  }


  //EFFECT FIRE
  if (effectString == "Fire") {
    Fire2012WithPalette();
    if (transitionTime == 0 or transitionTime == NULL) {
      transitionTime = 150;
    }
    showleds();
  }

  random16_add_entropy( random8());


  //EFFECT Glitter
  if (effectString == "Glitter") {
    fadeToBlackBy( leds, NUM_PIXELS, 20);
//    addGlitterColor(80, random(0, 255), random(0, 255), random(0, 255));   // random color glitter
    addGlitterColor(80, realRed, realGreen, realBlue);                   // selectable color glitter
    if (transitionTime == 0 or transitionTime == NULL) {
      transitionTime = 30;
    }
    showleds();
  }


  //EFFECT JUGGLE
  if (effectString == "Juggle" ) {                           // eight colored dots, weaving in and out of sync with each other
    fadeToBlackBy(leds, NUM_PIXELS, 20);
    for (int i = 0; i < 8; i++) {
      leds[beatsin16(i + 7, 0, NUM_PIXELS - 1  )] |= CRGB(realRed, realGreen, realBlue);
    }
    if (transitionTime == 0 or transitionTime == NULL) {
      transitionTime = 130;
    }
    showleds();
  }


  //EFFECT LIGHTNING
  if (effectString == "Lightning") {
    twinklecounter = twinklecounter + 1;                     //Resets strip if previous animation was running
    if (twinklecounter < 2) {
      FastLED.clear();
      FastLED.show();
    }
    ledstart = random8(NUM_PIXELS);           // Determine starting location of flash
    ledlen = random8(NUM_PIXELS - ledstart);  // Determine length of flash (not to go beyond NUM_PIXELS-1)
    for (int flashCounter = 0; flashCounter < random8(3, flashes); flashCounter++) {
      if (flashCounter == 0) dimmer = 5;    // the brightness of the leader is scaled down by a factor of 5
      else dimmer = random8(1, 3);          // return strokes are brighter than the leader
      fill_solid(leds + ledstart, ledlen, CHSV(255, 0, 255 / dimmer));
      showleds();    // Show a section of LED's
      delay(random8(4, 10));                // each flash only lasts 4-10 milliseconds
      fill_solid(leds + ledstart, ledlen, CHSV(255, 0, 0)); // Clear the section of LED's
      showleds();
      if (flashCounter == 0) delay (130);   // longer delay until next flash after the leader
      delay(50 + random8(100));             // shorter delay between strokes
    }
    delay(random8(frequency) * 100);        // delay between strikes
    if (transitionTime == 0 or transitionTime == NULL) {
      transitionTime = 0;
    }
    showleds();
    // view cycles of animation executed between mqtt resets
    if (stateOn == true){
      Serial.print("----- ");
      Serial.print(effectString);
      Serial.println(" animation cycle complete -----");
    }
  }


  //EFFECT POLICE ALL
  if (effectString == "Police All") {                 //POLICE LIGHTS (TWO COLOR SOLID)
    idex++;
    if (idex >= NUM_PIXELS) {
      idex = 0;
    }
    int idexR = idex;
    int idexB = antipodal_index(idexR);
    int thathue = (thishuepolice + 160) % 255;
    leds[idexR] = CHSV(thishuepolice, thissat, 255);
    leds[idexB] = CHSV(thathue, thissat, 255);
    if (transitionTime == 0 or transitionTime == NULL) {
      transitionTime = 30;
    }
    showleds();
  }

  //EFFECT POLICE ONE
  if (effectString == "Police One") {
    idex++;
    if (idex >= NUM_PIXELS) {
      idex = 0;
    }
    int idexR = idex;
    int idexB = antipodal_index(idexR);
    int thathue = (thishuepolice + 160) % 255;
    for (int i = 0; i < NUM_PIXELS; i++ ) {
      if (i == idexR) {
        leds[i] = CHSV(thishuepolice, thissat, 255);
      }
      else if (i == idexB) {
        leds[i] = CHSV(thathue, thissat, 255);
      }
      else {
        leds[i] = CHSV(0, 0, 0);
      }
    }
    if (transitionTime == 0 or transitionTime == NULL) {
      transitionTime = 30;
    }
    showleds();
  }


  //EFFECT RAINBOW
  if (effectString == "Rainbow") {
    // FastLED's built-in rainbow generator
    static uint8_t starthue = 0;    thishue++;
    fill_rainbow(leds, NUM_PIXELS, thishue, deltahue);
    if (transitionTime == 0 or transitionTime == NULL) {
      transitionTime = 130;
    }
    showleds();
  }


  //EFFECT RAINBOW WITH GLITTER
  if (effectString == "Rainbow With Glitter") {               // FastLED's built-in rainbow generator with Glitter
    static uint8_t starthue = 0;
    thishue++;
    fill_rainbow(leds, NUM_PIXELS, thishue, deltahue);
    addGlitter(80);
    if (transitionTime == 0 or transitionTime == NULL) {
      transitionTime = 130;
    }
    showleds();
  }


  //EFFECT SINELON
  if (effectString == "Sinelon") {
    fadeToBlackBy( leds, NUM_PIXELS, 20);
    int pos = beatsin16(13, 0, NUM_PIXELS - 1);
    leds[pos] += CRGB(realRed, realGreen, realBlue);
    if (transitionTime == 0 or transitionTime == NULL) {
      transitionTime = 150;
    }
    showleds();
  }


  //EFFECT TWINKLE
  if (effectString == "Twinkle") {
    twinklecounter = twinklecounter + 1;
    if (twinklecounter < 2) {                               //Resets strip if previous animation was running
      FastLED.clear();
      FastLED.show();
    }
    const CRGB lightcolor(8, 7, 1);
    for ( int i = 0; i < NUM_PIXELS; i++) {
      if ( !leds[i]) continue; // skip black pixels
      if ( leds[i].r & 1) { // is red odd?
        leds[i] -= lightcolor; // darken if red is odd
      } else {
        leds[i] += lightcolor; // brighten if red is even
      }
    }
    if ( random8() < DENSITY) {
      int j = random16(NUM_PIXELS);
      if ( !leds[j] ) leds[j] = lightcolor;
    }

    if (transitionTime == 0 or transitionTime == NULL) {
      transitionTime = 0;
    }
    showleds();
  }

/* Begin From Dr. Zzs */
    // colored stripes pulsing in Shades of GREEN and RED
    if(effectString == "Christmas"){
      uint8_t BeatsPerMinute = 62;
      CRGBPalette16 palette = bhw2_xmas_gp;
      uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
      for( int i = 0; i < NUM_PIXELS; i++){                 //9948
        leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
      }
      showleds();
    }

 
    // colored stripes pulsing in Shades of Red and ORANGE and Green
    if(effectString == "Thanksgiving"){     
      static uint8_t startIndex = 0;
      startIndex = startIndex + 1;                        /* higher = faster motion */
      fill_palette(leds, NUM_PIXELS,
        startIndex, 16,                                   /* higher = narrower stripes */
        ThxPalettestriped, 255, LINEARBLEND);
      if(transitionTime == 0 or transitionTime == NULL){
        transitionTime = 0;
      }                 
      showleds();
    }
  
   
    // colored stripes pulsing in Shades of Brown and ORANGE
    if(effectString == "Turkey Day"){
      uint8_t BeatsPerMinute = 62;
      CRGBPalette16 palette = bhw2_thanks_gp;
      uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
      for(int i = 0; i < NUM_PIXELS; i++){                    //9948
        leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
      }
      showleds();
    }
  
   
    // colored stripes pulsing in Shades of Purple and Orange
    if(effectString == "Halloween"){
      uint8_t BeatsPerMinute = 62;
      CRGBPalette16 palette = Orange_to_Purple_gp;
      uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
      for(int i = 0; i < NUM_PIXELS; i++){                   //9948
        leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
      }
      showleds();
    }
  
   
  
    if(effectString == "Pumpkin"){
      static uint8_t startIndex = 0;
      startIndex = startIndex + 1;                        /* higher = faster motion */
      fill_palette( leds, NUM_PIXELS,
        startIndex, 16,                                   /* higher = narrower stripes */
        HalloweenPalettestriped, 255, LINEARBLEND);
      if(transitionTime == 0 or transitionTime == NULL){
        transitionTime = 0;
      }                 
      showleds();
    }
  
   
  
    if(effectString == "Holly Jolly"){
      static uint8_t startIndex = 0;
      startIndex = startIndex + 1;                        /* higher = faster motion */
      fill_palette( leds, NUM_PIXELS,
        startIndex, 16,                                   /* higher = narrower stripes */
        HJPalettestriped, 255, LINEARBLEND);
      if (transitionTime == 0 or transitionTime == NULL) {
        transitionTime = 0;
      }
      showleds();
    }
/* End From Dr. Zzs */



/* Begin From Tweaking4All */
    /* Halloween Eyes */
    /* From https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#LEDStripEffectBlinkingHalloweenEyes */
    /* Parameter  Purpose                              Examples
     * red        Red Color                            0xFF
     * green      Green Color                          0x00
     * blue       Blue Color                           0x00
     * EyeWidth   How many LEDs per eye                1
     * EyeSpace   Number of LEDs between the eyes      2
     * Fade       Fade out or not                      true/false
     * Steps      Number of steps on fade out          10
     * FadeDelay  Delay between each fade out level    100
     * EndPause   Delay after everything is completed  1000
     */    
    if(effectString == "Halloween Eyes"){
      if(!stateOn){
        // do nothing (this is only needed for these animations from Tweaking4All)
      }else{

        // scale transitionTime to a number usable by this effect
        /* Scaling equation from https://stats.stackexchange.com/a/281164 */    
        double value = transitionTime;  // input to be scaled
        double rMin = 1;                // minimum of input scale
        double rMax = 150;              // maximum of input scale
        double sMin = 5;                // minimum of target scale
        double sMax = 50;               // maximum of target scale
        double scaledSteps = ((value - rMin) / (rMax - rMin)) * (sMax - sMin) + sMin;
        // convert to integer
        int intSteps = (int)scaledSteps;
        // invert number scale
        int correctedSteps = sMax - intSteps + sMin;
        
        double dMin = 50;                // minimum of target scale
        double dMax = 150;               // maximum of target scale
        double scaledDelay = ((value - rMin) / (rMax - rMin)) * (dMax - dMin) + dMin;
        // convert to integer
        int intDelay = (int)scaledDelay;
        // invert number scale
        int correctedDelay = dMax - intDelay + dMin;
        
        double pMin = 1000;                // minimum of target scale
        double pMax = 10000;               // maximum of target scale
        double scaledPause = ((value - rMin) / (rMax - rMin)) * (pMax - pMin) + pMin;
        // convert to integer
        int intPause = (int)scaledPause;
        // invert number scale
        int correctedPause = pMax - intPause + pMin;
      
      // Fixed:
      //    HalloweenEyes(0xff, 0x00, 0x00, 1,4, true, 10, 80, 3000);
      // Dynamic:
      HalloweenEyes(0xff, 0x00, 0x00, 1, 4, true, 
                    random(sMin, correctedSteps), 
                    random(dMin, correctedDelay), 
                    random(pMin, correctedPause));
      }
    }


    /* Rainbow Cycle */
    /* From https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#LEDStripEffectRainbowCycle */
    if(effectString == "Rainbow Cycle"){
      if(!stateOn){
        // do nothing (this is only needed for these animations from Tweaking4All)
      }else{
        /* rainbowCycle(20); */             // higher number = slower effect
        
        // scale transitionTime to a number usable by this effect
        /* Scaling equation from https://stats.stackexchange.com/a/281164 */
        double rMin = 1;                // minimum of input scale
        double rMax = 150;              // maximum of input scale
        double tMin = 1;                // minimum of target scale
        double tMax = 35;               // maximum of target scale (max time for animation delay.. works fine for 150 leds but may have to be reduced if pixel count increases or if MQTT disconnects occur)
        double value = transitionTime;  // input to be scaled
        double scaledTime = ((value - rMin)/(rMax - rMin))*(tMax - tMin)+tMin;
  
        // convert to integer for use by rainbowCycle()
        int inverseTime = (int)scaledTime;
  
        // invert number scale
        int correctedTime = tMax - inverseTime + tMin;
        
//        Serial.println(correctedTime);  // debug
        rainbowCycle(correctedTime);      
      }
    }
/* End From Tweaking4All */


/* Begin From The Hookup */
    /* LED Locator */
    /* From https://github.com/thehookup/Holiday_LEDS/blob/master/LightsMCU_CONFIGURE.ino */
    if(effectString == "LED Locator"){
      if(newLocator < NUM_PIXELS && newLocator != oldLocator){
        oldLocator = newLocator;
        setColor(0, 0, 0);          // from this sketch
        delay(10);  
        leds[newLocator]=CRGB(red, green, blue);
        FastLED.show();
      }
    }
/* End From The Hookup (modified) */

    
  EVERY_N_MILLISECONDS(10) {

    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);  // FOR NOISE ANIMATIon
    {
      gHue++;
    }

    //EFFECT NOISE
    if (effectString == "Noise") {
      for (int i = 0; i < NUM_PIXELS; i++) {                                     // Just onE loop to fill up the LED array as all of the pixels change.
        uint8_t index = inoise8(i * scale, dist + i * scale) % 255;            // Get a value from the noise function. I'm using both x and y axis.
        leds[i] = ColorFromPalette(currentPalette, index, 255, LINEARBLEND);   // With that value, look up the 8 bit colour palette value and assign it to the current LED.
      }
      dist += beatsin8(10, 1, 4);                                              // Moving along the distance (that random number we started out with). Vary it a bit with a sine wave.
      // In some sketches, I've used millis() instead of an incremented counter. Works a treat.
      if (transitionTime == 0 or transitionTime == NULL) {
        transitionTime = 0;
      }
      showleds();
    }

    //EFFECT RIPPLE
    if (effectString == "Ripple") {
      for (int i = 0; i < NUM_PIXELS; i++) leds[i] = CHSV(bgcol++, 255, 15);  // Rotate background colour.
      switch (step) {
        case -1:                                                          // Initialize ripple variables.
          center = random(NUM_PIXELS);
          colour = random8();
          step = 0;
          break;
        case 0:
          leds[center] = CHSV(colour, 255, 255);                          // Display the first pixel of the ripple.
          step ++;
          break;
        case maxsteps:                                                    // At the end of the ripples.
          step = -1;
          break;
        default:                                                             // Middle of the ripples.
          leds[(center + step + NUM_PIXELS) % NUM_PIXELS] += CHSV(colour, 255, myfade / step * 2);   // Simple wrap from Marc Miller
          leds[(center - step + NUM_PIXELS) % NUM_PIXELS] += CHSV(colour, 255, myfade / step * 2);
          step ++;                                                         // Next step.
          break;
      }
      if (transitionTime == 0 or transitionTime == NULL) {
        transitionTime = 30;
      }
      showleds();
    }

  }


  EVERY_N_SECONDS(5) {
    targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
  }

  //FLASH AND FADE SUPPORT
  if (flash) {
    if (startFlash) {
      startFlash = false;
      flashStartTime = millis();
    }

    if ((millis() - flashStartTime) <= flashLength) {
      if ((millis() - flashStartTime) % 1000 <= 500) {
        setColor(flashRed, flashGreen, flashBlue);
      }
      else {
        setColor(0, 0, 0);
        // If you'd prefer the flashing to happen "on top of"
        // the current color, uncomment the next line.
        // setColor(realRed, realGreen, realBlue);
      }
    }
    else {
      flash = false;
      effectString = oldeffectString;
      if (onbeforeflash) { //keeps light off after flash if light was originally off
        setColor(realRed, realGreen, realBlue);
      }
      else {
        stateOn = false;
        setColor(0, 0, 0);
        SEND_STATE();
      }
    }
  }

  if (startFade && effectString == "Solid") {
    // If we don't want to fade, skip it.
    if (transitionTime == 0) {
      setColor(realRed, realGreen, realBlue);

      redVal = realRed;
      grnVal = realGreen;
      bluVal = realBlue;

      startFade = false;
    }
    else {
      loopCount = 0;
      stepR = calculateStep(redVal, realRed);
      stepG = calculateStep(grnVal, realGreen);
      stepB = calculateStep(bluVal, realBlue);

      inFade = true;
    }
  }

  if (inFade) {
    startFade = false;
    unsigned long now = millis();
    if (now - lastLoop > transitionTime) {
      if (loopCount <= 1020) {
        lastLoop = now;

        redVal = calculateVal(stepR, redVal, loopCount);
        grnVal = calculateVal(stepG, grnVal, loopCount);
        bluVal = calculateVal(stepB, bluVal, loopCount);

        if (effectString == "Solid") {
          setColor(redVal, grnVal, bluVal); // Write current values to LED pins
        }
        loopCount++;
      }
      else {
        inFade = false;
      }
    }
  }
}


/**************************** START TRANSITION FADER *****************************************/
// From https://www.arduino.cc/en/Tutorial/ColorCrossfader
/* BELOW THIS LINE IS THE MATH -- YOU SHOULDN'T NEED TO CHANGE THIS FOR THE BASICS
  The program works like this:
  Imagine a crossfade that moves the red LED from 0-10,
    the green from 0-5, and the blue from 10 to 7, in
    ten steps.
    We'd want to count the 10 steps and increase or
    decrease color values in evenly stepped increments.
    Imagine a + indicates raising a value by 1, and a -
    equals lowering it. Our 10 step fade would look like:
    1 2 3 4 5 6 7 8 9 10
  R + + + + + + + + + +
  G   +   +   +   +   +
  B     -     -     -
  The red rises from 0 to 10 in ten steps, the green from
  0-5 in 5 steps, and the blue falls from 10 to 7 in three steps.
  In the real program, the color percentages are converted to
  0-255 values, and there are 1020 steps (255*4).
  To figure out how big a step there should be between one up- or
  down-tick of one of the LED values, we call calculateStep(),
  which calculates the absolute gap between the start and end values,
  and then divides that gap by 1020 to determine the size of the step
  between adjustments in the value.
*/
int calculateStep(int prevValue, int endValue) {
  int step = endValue - prevValue; // What's the overall gap?
  if (step) {                      // If its non-zero,
    step = 1020 / step;          //   divide by 1020
  }

  return step;
}
/* The next function is calculateVal. When the loop value, i,
   reaches the step size appropriate for one of the
   colors, it increases or decreases the value of that color by 1.
   (R, G, and B are each calculated separately.)
*/
int calculateVal(int step, int val, int i) {
  if ((step) && i % step == 0) { // If step is non-zero and its time to change a value,
    if (step > 0) {              //   increment the value if step is positive...
      val += 1;
    }
    else if (step < 0) {         //   ...or decrement it if step is negative
      val -= 1;
    }
  }

  // Defensive driving: make sure val stays in the range 0-255
  if (val > 255) {
    val = 255;
  }
  else if (val < 0) {
    val = 0;
  }

  return val;
}

/* Begin From Dr. Zzs */
////////////////////////place setup__Palette and __Palettestriped custom functions here - for Candy Cane effects ///////////////// 
///////You can use up to 4 colors and change the pattern of A's AB's B's and BA's as you like//////////////
void setupHalloweenPalette( CRGB A, CRGB AB, CRGB B, CRGB BA){
  HalloweenPalettestriped = CRGBPalette16(
    A, A, A, A, A, A, A, A, B, B, B, B, B, B, B, B
  );
}

void setupThxPalette( CRGB A, CRGB AB, CRGB B, CRGB BA){
  ThxPalettestriped = CRGBPalette16(
    A, A, A, A, A, A, A, AB, AB, AB, B, B, B, B, B, B
  );
}

 

void setupHJPalette( CRGB A, CRGB AB, CRGB B, CRGB BA){
  HJPalettestriped = CRGBPalette16(
    A, A, A, A, A, A, A, A, B, B, B, B, B, B, B, B
  );
}
/* End From Dr. Zzs */


/* Begin From Halloween Eyes */
void HalloweenEyes(byte red, byte green, byte blue, 
                   int EyeWidth, int EyeSpace, 
                   boolean Fade, int Steps, int FadeDelay,
                   int EndPause){
  //randomSeed(analogRead(0));  //esp8266 ***ESP8266 > ESP32 Conversion
  randomSeed(esp_random());     //esp32
  
  int i;
  int StartPoint  = random( 0, NUM_PIXELS - (2*EyeWidth) - EyeSpace );
  int Start2ndEye = StartPoint + EyeWidth + EyeSpace;
  
  for(i = 0; i < EyeWidth; i++) {
    setPixel(StartPoint + i, red, green, blue);
    setPixel(Start2ndEye + i, red, green, blue);
  }
  
  FastLED.show();
  
  if(Fade==true) {
    float r, g, b;
  
    for(int j = Steps; j >= 0; j--) {
      r = j*(red/Steps);
      g = j*(green/Steps);
      b = j*(blue/Steps);
      
      for(i = 0; i < EyeWidth; i++) {
        setPixel(StartPoint + i, r, g, b);
        setPixel(Start2ndEye + i, r, g, b);
      }
      
      FastLED.show();
      delay(FadeDelay);
    }
  }
  
  setColor(0, 0, 0); // Set all black
  
  delay(EndPause);
}
/* End From Halloween Eyes */

/**************************** START STRIPLED PALETTE *****************************************/
void setupStripedPalette( CRGB A, CRGB AB, CRGB B, CRGB BA) {
  currentPalettestriped = CRGBPalette16(
    A, A, A, A, A, A, A, A, B, B, B, B, B, B, B, B
  );
}



/********************************** START FADE************************************************/
void fadeall() {
  for (int i = 0; i < NUM_PIXELS; i++) {
    leds[i].nscale8(250);  //for Cylon
  }
}



/********************************** START FIRE **********************************************/
void Fire2012WithPalette()
{
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_PIXELS];

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUM_PIXELS; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_PIXELS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = NUM_PIXELS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for ( int j = 0; j < NUM_PIXELS; j++) {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    byte colorindex = scale8( heat[j], 240);
    CRGB color = ColorFromPalette( gPal, colorindex);
    int pixelnumber;
    if ( gReverseDirection ) {
      pixelnumber = (NUM_PIXELS - 1) - j;
    } else {
      pixelnumber = j;
    }
    leds[pixelnumber] = color;
  }
}



/********************************** START ADD GLITTER *********************************************/
void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_PIXELS) ] += CRGB::White;
  }
}



/********************************** START ADD GLITTER COLOR ****************************************/
void addGlitterColor( fract8 chanceOfGlitter, int red, int green, int blue)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_PIXELS) ] += CRGB(red, green, blue);
  }
}



/********************************** START SHOW LEDS ***********************************************/
void showleds() {

  delay(1);

  if (stateOn) {
    FastLED.setBrightness(brightness);  //EXECUTE EFFECT COLOR
    FastLED.show();
    if (transitionTime > 0 && transitionTime < 130) {  //Sets animation speed based on receieved value
      FastLED.delay(1000 / transitionTime);
      //delay(10*transitionTime);
    }
  }
  else if (startFade) {
    setColor(0, 0, 0);
    startFade = false;
  }
}
void temp2rgb(unsigned int kelvin) {
    int tmp_internal = kelvin / 100.0;
    
    // red 
    if (tmp_internal <= 66) {
        red = 255;
    } else {
        float tmp_red = 329.698727446 * pow(tmp_internal - 60, -0.1332047592);
        if (tmp_red < 0) {
            red = 0;
        } else if (tmp_red > 255) {
            red = 255;
        } else {
            red = tmp_red;
        }
    }
    
    // green
    if (tmp_internal <=66){
        float tmp_green = 99.4708025861 * log(tmp_internal) - 161.1195681661;
        if (tmp_green < 0) {
            green = 0;
        } else if (tmp_green > 255) {
            green = 255;
        } else {
            green = tmp_green;
        }
    } else {
        float tmp_green = 288.1221695283 * pow(tmp_internal - 60, -0.0755148492);
        if (tmp_green < 0) {
            green = 0;
        } else if (tmp_green > 255) {
            green = 255;
        } else {
            green = tmp_green;
        }
    }
    
    // blue
    if (tmp_internal >=66) {
        blue = 255;
    } else if (tmp_internal <= 19) {
        blue = 0;
    } else {
        float tmp_blue = 138.5177312231 * log(tmp_internal - 10) - 305.0447927307;
        if (tmp_blue < 0) {
            blue = 0;
        } else if (tmp_blue > 255) {
            blue = 255;
        } else {
            blue = tmp_blue;
        }
    }
}

/* Begin From Tweaking4All */
/* Halloween Eyes */
void setPixel(int Pixel, byte red, byte green, byte blue){
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
}
/* End Halloween Eyes */

/* Rainbow Cycle */
void rainbowCycle(int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*1; j++) { // 1 cycle of all colors on wheel
    for(i=0; i< NUM_PIXELS; i++) {
      c=Wheel(((i * 256 / NUM_PIXELS) + j) & 255);
      setPixel(i, *c, *(c+1), *(c+2));
    }
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(SpeedDelay);
  }
}

byte * Wheel(byte WheelPos) {
  static byte c[3];
  
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}
/* End Rainbow Cycle */
/* End From Tweaking4All */