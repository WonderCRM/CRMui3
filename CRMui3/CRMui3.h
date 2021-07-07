#include "defines.h"
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>

#ifdef ESP32
#include "SPIFFS.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#else
#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif


typedef struct {
  const String &id;
  const String &label;
  String defaultValue;
  String options[50][2];
} Select;

typedef struct {
  uint8_t type;
  const String &id;
  const String &label;
  String defaultValue;
  String d;
  String s;
} Input;

typedef struct {
  uint8_t type;
  const String &id;
  const String &label;
  String def;
  String c;
} Output;

typedef struct {
  const String &id;
  const String &label;
  float defaultValue;
  float min;
  float max;
  float step;
  String unit;
} Range;

typedef struct {
  uint8_t type;
  const String &id;
  const String &label;
  String defaultValue;
  String icon;
  String color;
  bool newGroup;
} Card;

typedef struct {
  uint8_t type;
  const String &id;
  const String &label;
  String labelLine;
  String valueLine;
  String color;
  String height;
} Chart;




class CRMui3 {
    StaticJsonDocument<4096> cfg;

    typedef void (*buttonCallback) ();
    typedef void (*uiCallback) ();
    typedef void (*updateCallback) ();
    typedef void (*apiCallback) (String);

  public:
    void test(uint32_t s);
    void begin(const String &app_name, void (*uiFunction)(), void (*updateFunction)() = NULL,
               void (*apiFunction)(String) = NULL, uint32_t baud = 0);
    void run();
    String upTime();
    void cfgDelete();
    void espReboot();
    String getLang();
    uint8_t webConnCountStatus();
    void espSleep(uint32_t sec = 0, bool m = false);
    void license(const String &lic, const String &e = "", const String &t = "", const String &h = "");

    void setWebAuth(const String &login, const String &pass = "");
    void setApiKey(const String &key);
    void page(const String &pageName);
    void select(Select);
    void input(Input);
    void output(Output);
    void range(Range);
    void card(Card);
    void chart(Chart);
    bool btnSwStatus();
    void btnCallback(const String &name, buttonCallback response);
    void btnCallback(int, buttonCallback response, uint8_t lvl = LOW);
    void wifiForm(uint8_t, const String &ap_ssid = "", const String &ap_pass = "",
                  const String &ssid = "", const String &pass = "", long wtc = 120);
    void webUpdate(const String &name, const String &value, bool n = false);
    void webNotif(const String &type, const String &text, long tout = 5, bool x = true);
    void apiResponse(const String &p, const String &v);

    String var(const String &name);
    void var(const String &name, const String &value, bool save = true);
    void var(const String &name, float, bool save = true);
    void var(const String &name, double, bool save = true);
    void var(const String &name, int, bool save = true);
    void var(const String &name, long, bool save = true);
    void var(const String &name, uint32_t, bool save = true);
    void var(const String &name, char, bool save = true);

  private:
    String test_s = "";
    uiCallback ui;
    updateCallback upd;
    apiCallback api;
    bool _apiStatus = true;
    bool _updateStatus = true;

    void wifiEvent();
    void wifiSTA();
    void wifiAP();
    void wifiStart();
    uint8_t _wifiMode = 2;
    uint32_t _connectingTimer = 0;
    uint32_t _waitTimeForConnection = 0;

    String _app_name = String();
    String _buf = String();
    bool _start = true;
    uint32_t _upTime = 0;
    uint32_t _runTimer = 0;
    bool _sendStatistic = false;

    String _WebAuthLogin = String();
    String _WebAuthPass = String();
    String _apiKey = String();
    bool _AuthenticateStatus = false;
    void http();
    void uiJsonSend();
    String respString(const String &m, int t = -1);
    String _apiResponse;
    String _licKey = String();
    String _homePage = String();
    String _eMail = String();
    String _telega = String();

    bool _debug = false;
    bool _espNeedReboot = false;

    bool _AutoSaveStatus = false;
    uint32_t _AutoSaveTimer = 0;
    void defaultWifi(uint8_t, const String &ap_ssid, const String &ap_pass, const String &ssid, const String &pass, long wtc);
    void cfgLoad();
    void cfgSave();
    void cfgAutoSave();
    void cfgPrint();
    void var_auto_save(bool save);

    String _id = String();
    void getID();

    String _btnui = String();
};
