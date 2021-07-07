#define CRM_VER F("2021.07.06a")

#define CONFIG_BUTTON_PIN 0
#define AUTO_SAVE_TIME_INTERVAL 30000
#define UDP_PORT 4243

#define INPUT_BUTTON 0
#define INPUT_CHECKBOX 1
//#define INPUT_FILE 2
//#define INPUT_IMAGE 4
#define INPUT_PASSWORD 5
//#define INPUT_RADIO 6
//#define INPUT_RESET 7
//#define INPUT_SUBMIT 8
#define INPUT_TEXT 9
#define INPUT_COLOR 10
#define INPUT_DATE 11
#define INPUT_DATETIME 12
#define INPUT_EMAIL 13
#define INPUT_NUMBER 14
#define INPUT_RANGE 15
//#define INPUT_SEARCH 16
#define INPUT_SELECT 17
#define INPUT_TIME 18
#define INPUT_WIFI 19

#define CARD_CHECKBOX 20
#define CARD_TEXT 21
#define CARD_BUTTON 22
#define CARD_CHART_L 23
#define CARD_CHART_B 24

#define OUTPUT_TEXT 30
#define OUTPUT_TABL 31
#define OUTPUT_HR 32
#define CHART_L 33
#define CHART_B 34

#define DBG if (_debug) Serial.print
#define DBGLN if (_debug) Serial.println
#define SPLN Serial.println
#define SP Serial.print

#define TRACE() \
  Serial.print(__FILE__); \
  Serial.print(" | "); \
  Serial.print(__LINE__); \
  Serial.print(" | "); \
  Serial.println(__PRETTY_FUNCTION__); \
  Serial.flush();
  
