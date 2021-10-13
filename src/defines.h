#ifndef defines_h
#define defines_h

#define CRM_VER F("2021.09.14a")  // r1.2.3

#define AUTO_SAVE_TIME_INTERVAL 30000

#define INPUT_BUTTON 0
#define INPUT_CHECKBOX 1
#define GAUDE_1 2
#define GAUDE_2 3
//#define ___ 4
//#define ___ 5
//#define ___ 6
//#define ___ 7
#define INPUT_PASSWORD 8
#define INPUT_TEXT 9
#define INPUT_COLOR 10
#define INPUT_DATE 11
#define INPUT_DATETIME 12
#define INPUT_TIME 13
#define INPUT_NUMBER 14
#define INPUT_RANGE 15
//#define ___ 16
#define INPUT_SELECT 17
#define INPUT_EMAIL 18
#define INPUT_WIFI 19

#define CARD_CHECKBOX 20
#define CARD_TEXT 21
#define CARD_BUTTON 22
#define CARD_CHART_L 23
#define CARD_CHART_B 24

#define OUTPUT_TEXT 30
#define OUTPUT_TABL 31
#define OUTPUT_HR 32
#define OUTPUT_LABEL 33

#define CHART_L 43
#define CHART_B 44


#define DBG if (_debug) Serial.print
#define DBGLN if (_debug) Serial.println
#define SPLN(x) Serial.println(x)
#define SP(x) Serial.print(x)

#define TRACE() \
  Serial.print(__FILE__); \
  Serial.print(F(" | ")); \
  Serial.print(__LINE__); \
  Serial.print(F(" | M")); \
  Serial.println(ESP.getFreeHeap()); \
  Serial.flush();

#endif
