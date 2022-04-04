#include "CRMui3.h"


/*
  typedef struct {
  uint8_t type;
  const String &id;
  const String &label;
  String defaultValue;
    {
      {"label", "value"},
      ...
      до 50-ти
    }
  } Select_С;
*/
void CRMui3::select(Select_C item) {
  if (var(item.id) == F("null") and item.type != INPUT_SELECT_PIECE_UP) var(item.id, item.defaultValue);
  if (_start) return;
  if (_buf.endsWith("]")) _buf[_buf.length() - 1] = ',';
  _buf += F("{\"o\":[");
  for (uint8_t i = 0; i < 50; i++) {
    if (item.options[i][0] != "") {
      if (!_buf.endsWith(",") && !_buf.endsWith("[")) _buf += ",";
      _buf += String() + F("[\"") + item.options[i][0] + F("\",\"") + item.options[i][1] + F("\"]");
    } else break;
  }
  _buf += String() + F("],\"id\":\"") + item.id + F("\",");
  _buf += String() + F("\"t\":") + String(item.type) + ",";
  if(item.type == INPUT_SELECT_PIECE_UP) 
    _buf += String() + F("\"w\":") + item.defaultValue + ",";
  _buf += String() + F("\"l\":\"") + item.label + F("\"}]");
}

/*
  typedef struct {
  const String &id;
  const String &label;
  String defaultValue;
    {
      {"label", "value"},
      ...
      до 50-ти
    }
  } Select;
*/
void CRMui3::select(Select item) {
  if (var(item.id) == F("null")) var(item.id, item.defaultValue);
  if (_start) return;
  if (_buf.endsWith("]")) _buf[_buf.length() - 1] = ',';
  _buf += F("{\"o\":[");
  for (uint8_t i = 0; i < 50; i++) {
    if (item.options[i][0] != "") {
      if (!_buf.endsWith(",") && !_buf.endsWith("[")) _buf += ",";
      _buf += String() + F("[\"") + item.options[i][0] + F("\",\"") + item.options[i][1] + F("\"]");
    } else break;
  }
  _buf += String() + F("],\"id\":\"") + item.id + F("\",");
  _buf += String() + F("\"t\":") + String(INPUT_SELECT) + ",";
  _buf += String() + F("\"l\":\"") + item.label + F("\"}]");
}


/*
  typedef struct {
  uint8_t type;
  const String &id;
  const String &label;
  String defaultValue;
  String d;
  String s;
  } Input;
*/
void CRMui3::input(Input item) {
  if (item.type != INPUT_BUTTON && item.type != INPUT_WIFI && var(item.id) == F("null"))
    var(item.id, item.defaultValue);
  if (_start) return;
  if (_buf.endsWith("]")) _buf[_buf.length() - 1] = ',';
  _buf += String() + F("{\"t\":") + String(item.type);
  _buf += String() + F(",\"id\":\"") + item.id + "\"";
  if (item.d != "") _buf += String() + F(",\"d\":\"") + item.d + "\"";
  if (item.s != "") _buf += String() + F(",\"s\":") + item.s;
  if (item.type == INPUT_BUTTON && item.defaultValue != "")
    _buf += String() + F(",\"p\":\"") + item.defaultValue + "\"";
  if(item.type == INPUT_SELECT_PIECE_UP and item.type != INPUT_BUTTON) 
    _buf += String() + F(",\"w\":") + item.defaultValue;
  _buf += String() + F(",\"l\":\"") + item.label + F("\"}]");
}


/*
  typedef struct {
  uint8_t type;
  const String &id;
  const String &label;
  String def;
  String c;
  String d;
  } Output;
*/
void CRMui3::output(Output item) {
  if (_start) return;
  if (_buf.endsWith("]")) _buf[_buf.length() - 1] = ',';
  _buf += String() + F("{\"t\":") + String(item.type);
  _buf += String() + F(",\"id\":\"") + item.id + "\"";
  _buf += String() + F(",\"l\":\"") + item.label + "\"";
  _buf += String() + F(",\"d\":\"") + item.def + "\"";
  if (item.c != "") _buf += String() + F(",\"c\":\"") + item.c + "\"";
  if (item.s != "") _buf += String() + F(",\"s\":\"") + item.s + "\"";
  _buf += F("}]");
}


/*
  typedef struct {
  const String &id;
  const String &label;
  float defaultValue;
  float min;
  float max;
  float step;
  String unit;
  } Range;
*/
void CRMui3::range(Range item) {
  if (var(item.id) == F("null")) var(item.id, item.defaultValue);
  if (_start) return;
  if (_buf.endsWith("]")) _buf[_buf.length() - 1] = ',';
  _buf += String() + F("{\"t\":") + String(INPUT_RANGE) + ",";
  _buf += String() + F("\"id\":\"") + item.id + F("\",");
  _buf += String() + F("\"mn\":") + String(item.min) + ",";
  _buf += String() + F("\"mx\":") + String(item.max) + ",";
  _buf += String() + F("\"s\":") + String(item.step) + ",";
  _buf += String() + F("\"w\":") + String(item.defaultValue) + ",";
  _buf += String() + F("\"u\":\"") + item.unit + F("\",");
  _buf += String() + F("\"l\":\"") + item.label + F("\"}]");
}


/*
  typedef struct {
  uint8_t type;
  const String &id;
  const String &label;
  String defaultValue;
  String icon;
  String color;
  bool newGroup;
  } Card;
*/
void CRMui3::card(Card item) {
  if (item.type == CARD_CHECKBOX && var(item.id) == F("null"))
    var(item.id, item.defaultValue);
  if (_start) return;
  if (_buf.endsWith("]")) _buf[_buf.length() - 1] = ',';
  _buf += String() + F("{\"t\":") + String(item.type);
  _buf += String() + F(",\"id\":\"") + item.id + "\"";
  _buf += String() + F(",\"l\":\"") + item.label + "\"";
  if (item.type != CARD_CHECKBOX && item.defaultValue != "") {
    _buf += F(",\"v\":");
    if (item.type < CARD_CHART_L) _buf += "\"";
    _buf += item.defaultValue;
    if (item.type < CARD_CHART_L) _buf += "\"";
  }
  if (item.icon != "") {
    _buf += F(",\"i\":");
    if (item.type < CARD_CHART_L) _buf += "\"";
    _buf += item.icon;
    if (item.type < CARD_CHART_L) _buf += "\"";
  }
  if (item.color != "") _buf += String() + F(",\"c\":\"") + item.color + "\"";
  if (item.newGroup) _buf += String() + F(",\"n\":1");
  _buf += F("}]");
}


/*
  typedef struct {
  uint8_t type;
  const String &id;
  const String &label;
  String height;
  String color;
  String labelLine;
  String valueLine;
  } Chart;
*/
void CRMui3::chart(Chart item) {
  if (_start) {
    _useChart = true;
    return;
  }
  if (_buf.endsWith("]")) _buf[_buf.length() - 1] = ',';
  _buf += String() + F("{\"t\":") + String(item.type);
  _buf += String() + F(",\"id\":\"") + item.id + "\"";
  _buf += String() + F(",\"l\":\"") + item.label + "\"";
  if (item.labelLine != "") _buf += String() + F(",\"a\":") + item.labelLine;
  if (item.valueLine != "") _buf += String() + F(",\"b\":") + item.valueLine;
  if (item.color != "") _buf += String() + F(",\"c\":\"") + item.color + "\"";
  if (item.height != "") _buf += String() + F(",\"h\":\"") + item.height + "\"";
  _buf += F("}]");
}


/*
  typedef struct {
  uint8_t type;
  const String &id;
  const String &label;
  float min;
  float max;
  float def;
  String color[6][3];
  String unit;
  bool group;
  } Gauge;
*/
void CRMui3::gauge(Gauge item) {
  if (_start) {
    _useGauge = true;
    return;
  }
  if (_buf.endsWith("]")) _buf[_buf.length() - 1] = ',';
  _buf += F("{\"c\":[");
  for (uint8_t i = 0; i < 6; i++) {
    if (item.color[i][0] != "") {
      if (!_buf.endsWith(",") && !_buf.endsWith("[")) _buf += ",";
      _buf += String() + F("[\"") + item.color[i][0] + F("\",\"") +
              item.color[i][1] + F("\",\"") +
              item.color[i][2] + F("\"]");
    } else break;
  }
  _buf += String() + F("],\"id\":\"") + item.id;
  _buf += String() + F("\",\"t\":") + String(item.type);
  _buf += String() + F(",\"n\":") + String(item.min);
  _buf += String() + F(",\"x\":") + String(item.max);
  _buf += String() + F(",\"d\":") + String(item.def);
  if (item.unit != "") _buf += String() + F(",\"u\":\"") + String(item.unit) + "\"";
  if (item.group) _buf += String() + F(",\"g\":1");
  _buf += String() + F(",\"l\":\"") + item.label + F("\"}]");
}


void CRMui3::wifiForm(uint8_t mode, const String &ap_ssid, const String &ap_pass, const String &ssid, const String &pass, long wtc) {
  if (_start) {
    defaultWifi(mode, ap_ssid, ap_pass, ssid, pass, wtc);
    return;
  }
  if (_buf.endsWith("]")) _buf[_buf.length() - 1] = ',';
  _buf += String() + F("{\"t\":") + String(INPUT_WIFI) + F("}]");
}


void CRMui3::page(const String &pageName) {
  if (_start) return;
  if (!_buf.endsWith(",") && !_buf.endsWith("[")) _buf += ",";
  _buf += String() + F("[{\"m\":\"") + pageName + F("\"}]");
}