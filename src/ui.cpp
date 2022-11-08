#include "CRMui3.h"



void CRMui3::selOpt(SelOpt item) {
  if (_start) return;
  if (!_bufOpt) _bufOpt = new String;
  String b = String();
  if ((*_bufOpt).endsWith("]")) b += F(",");
  b += F("[\"");
  b += item.label;
  b += F("\",\"");
  b += item.value;
  b += F("\"]");
  *_bufOpt += b;
}


/*
  typedef struct {
  const String &id;
  const String &label;
  const String &defaultValue;
  bool reload;
  } Select;
*/
void CRMui3::select(Select item) {
  if (var(item.id) == F("null")) var(item.id, item.defaultValue);
  if (_start) return;
  String b = String();
  if ((*_bufUI) != "") b += F(",");
  b += F("{\"o\":[");
  if (_bufOpt) {
    b += *_bufOpt;
    delete _bufOpt;
    _bufOpt = nullptr;
  }
  b += F("],\"id\":\"");
  b += item.id;
  b += F("\",\"t\":");
  b += INPUT_SELECT;
  b += F(",\"r\":");
  b += item.reload;
  b += F(",\"l\":\"");
  b += item.label;
  b += F("\"}");
  *_bufUI += b;
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
  String b = String();
  if ((*_bufUI) != "") b += F(",");
  b += F("{\"t\":");
  b += item.type;
  b += F(",\"id\":\"");
  b += item.id;
  b += F("\"");
  if (item.d != "") {
    b += F(",\"d\":\"");
    b += item.d;
    b += F("\"");
  }
  if (item.s != "") {
    b += F(",\"s\":");
    b += item.s;
  }
  if (item.type == INPUT_BUTTON && item.defaultValue != "") {
    b += F(",\"p\":\"");
    b += item.defaultValue;
    b += F("\"");
  }
  b += F(",\"l\":\"");
  b += item.label;
  b += F("\"}");
  *_bufUI += b;
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
  String b = String();
  if ((*_bufUI) != "") b += F(",");
  b += F("{\"t\":");
  b += item.type;
  b += F(",\"id\":\"");
  b += item.id;
  b += F("\",\"l\":\"");
  b += item.label;
  b += F("\",\"d\":\"");
  b += item.def;
  b += F("\"");
  if (item.c != "") {
    b += F(",\"c\":\"");
    b += item.c;
    b += F("\"");
  }
  if (item.s != "") {
    b += F(",\"s\":\"");
    b += item.s;
    b += F("\"");
  }
  b += F("}");
  *_bufUI += b;
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
  String b = String();
  if ((*_bufUI) != "") b += F(",");
  b += F("{\"t\":");
  b += INPUT_RANGE;
  b += F(",\"id\":\"");
  b += item.id;
  b += F("\",\"mn\":");
  b += item.min;
  b += F(",\"mx\":");
  b += item.max;
  b += F(",\"s\":");
  b += item.step;
  b += F(",\"u\":\"");
  b += item.unit;
  b += F("\",\"l\":\"");
  b += item.label;
  b += F("\"}");
  *_bufUI += b;
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
  String b = String();
  if ((*_bufUI) != "") b += F(",");
  b += F("{\"t\":");
  b += item.type;
  b += F(",\"id\":\"");
  b += item.id;
  b += F("\",\"l\":\"");
  b += item.label;
  b += F("\"");
  if (item.type != CARD_CHECKBOX && item.defaultValue != "") {
    b += F(",\"v\":");
    if (item.type < CARD_CHART_L) b += F("\"");
    b += item.defaultValue;
    if (item.type < CARD_CHART_L) b += F("\"");
  }
  if (item.icon != "") {
    b += F(",\"i\":");
    if (item.type < CARD_CHART_L) b += F("\"");
    b += item.icon;
    if (item.type < CARD_CHART_L) b += F("\"");
  }
  if (item.color != "") {
    b += F(",\"c\":\"");
    b += item.color;
    b += F("\"");
  }
  if (item.newGroup) b += F(",\"n\":1");
  b += F("}");
  *_bufUI += b;
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
  _useChart = true;
  if (_start) return;
  String b = String();
  if ((*_bufUI) != "") b += F(",");
  b += F("{\"t\":");
  b += item.type;
  b += F(",\"id\":\"");
  b += item.id;
  b += F("\",\"l\":\"");
  b += item.label;
  b += F("\"");
  if (item.labelLine != "") {
    b += F(",\"a\":");
    b += item.labelLine;
  }
  if (item.valueLine != "") {
    b += F(",\"b\":");
    b += item.valueLine;
  }
  if (item.color != "") {
    b += F(",\"c\":\"");
    b += item.color;
    b += "\"";
  }
  if (item.height != "") {
    b += F(",\"h\":\"");
    b += item.height;
    b += "\"";
  }
  b += F("}");
  *_bufUI += b;
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
  _useGauge = true;
  if (_start) return;
  String b = String();
  if ((*_bufUI) != "") b += F(",");
  b += F("{\"c\":[");
  for (uint8_t i = 0; i < 6; i++) {
    if (item.color[i][0] != "") {
      if (!b.endsWith(",") && !b.endsWith("[")) b += ",";
      b += F("[\"");
      b += item.color[i][0];
      b += F("\",\"");
      b += item.color[i][1];
      b += F("\",\"");
      b += item.color[i][2];
      b += F("\"]");
    } else break;
  }
  b += F("],\"id\":\"");
  b += item.id;
  b += F("\",\"t\":");
  b += item.type;
  b += F(",\"n\":");
  b += item.minimum;
  b += F(",\"x\":");
  b += item.maximum;
  b += F(",\"d\":");
  b += item.def;
  if (item.unit != "") {
    b += F(",\"u\":\"");
    b += item.unit;
    b += F("\"");
  }
  if (item.group) {
    b += F(",\"g\":1");
  }
  b += F(",\"l\":\"");
  b += item.label;
  b += F("\"}");
  *_bufUI += b;
}


void CRMui3::wifiForm(uint8_t mode, const String &ap_ssid, const String &ap_pass, const String &ssid, const String &pass, long wtc) {
  if (_start) {
    defaultWifi(mode, ap_ssid, ap_pass, ssid, pass, wtc);
    return;
  }
  String b = String();
  if ((*_bufUI) != "") b += F(",");
  b += F("{\"t\":");
  b += INPUT_WIFI;
  b += F("}");
  *_bufUI += b;
}


void CRMui3::page(const String &pageName) {
  if (_start) return;
  String b = String();
  if ((*_bufUI).endsWith("}")) b += "]";
  if (!(*_bufUI).endsWith("[")) b += F(",");
  b += F("[{\"m\":\"");
  b += pageName;
  b += F("\"}");
  *_bufUI += b;
}


void CRMui3::group(uint8_t type) {
  if (_start) return;
  String b = String();
  if ((*_bufUI) != "") b += F(",");
  b += F("{\"t\":");
  b += type;
  b += F("}");
  *_bufUI += b;
}
