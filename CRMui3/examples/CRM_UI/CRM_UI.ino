//Example use CRMui3  /  Пример использования CRMui3

#include "CRMui3.h"
#include <Ticker.h> // Входит в состав ядра

// Объявление объектов
CRMui3 crm;     // CRMui
Ticker myLoop;  // Ticker

// Переменные в примере
bool st3, st4, st5;

int select4 = 0;
void btnResponce(const char *name);


String lng() {
  // Вариант реализации многоязычности
  // Получить индекс языка
  // crm.getLang();
  String L = crm.getLang();
  uint8_t l = 0;
  if (L == "de") l = 1;
  else if (L == "ru") l = 2;
  switch (l) {
    case 0: return "English";
    case 1: return "Deutsch";
    case 2: return "Russian";
    default: return "n/a";
  }
}

void myLoopRun() {
 
  static int a[3] = {0};
  static int i = 0;
  if (i > 2) i = 0;
  a[i] = WiFi.RSSI();

  // Обновление значений элементов веб интерфейса
  // crm.webUpdate("[ID элемента]", "[Значение]");
  // Интервал отправки 1 раз в сек.
  int b = (a[0] + a[1] + a[2]) / 3;
  crm.webUpdate("rssi", String(b));
  crm.webUpdate("rssiraw", String(a[i]));
  crm.webUpdate("G_0", String(b));
  crm.webUpdate("G_1", String(random(-20, 60)));
  crm.webUpdate("G_2", String(random(0, 100)));
  i++;
}


void update() {
  // Метод вызывается при каждом изменении значения элементов через веб интерфейса
  Serial.println("Method update() run");
  Serial.println("Language: " + lng());

  // Получить(Записать) значение переменной из(в) конфига
  // crm.var("ID")
  // crm.var("ID", "Значение")
  bool a = crm.var("card1") == "true" ? true : false;

  static bool b = false;
  if (b != a) {
    digitalWrite(2, a ? HIGH : LOW);

    // Отправить уведомление на страницу веб интерфейса
    // crm.webNotif("[Цвет]", "[Сообщение]", [время показа, с], [крестик закрыть (1, true)]);
    // Цвет: green, red, orange, blue, "" - без цвета
    crm.webNotif(a ? "Red" : "Green", a ? "Мотор запущен" : "Мотор остановлен", 5);

    b = a;
  }
}


void api(String p) {
  // Метод вызывается при API запросах
  // Обязательно с параметром, тип String
  //
  // Ответ на запрос, тип JSON
  // crm.apiResponse("[ID]", "[Значение]");

  Serial.print("API: ");
  Serial.println(p);

  DynamicJsonDocument doc(200);
  deserializeJson(doc, p);

  //Запрос http://IP/api?hum
  const char* hum = doc["hum"];
  if (hum != NULL) {
    crm.apiResponse("Humidity", "51%");
  }

  //Использование параметров
  //Запрос http://IP/api?random=500
  const char* rnd = doc["random"];
  if (rnd != NULL) {
    crm.apiResponse("Random", String(random(atoi(rnd))));
  }

  //Запрос http://IP/api?print=[any_text]
  const char* prt = doc["print"];
  if (prt != NULL) {
    Serial.println(prt);
  }
}


//функции кнопок
void hw_butt() {
  Serial.println("HW BUTTON PRESS!");
}

// ------- Through a callBackButton Event -------------
void btnResponce(const char *name) {
  if (strcmp(name, "card3") == 0) {
    Serial.println("Card 3 Button press.");
    st3 = !st3;
    crm.webUpdate("card3", st3 ? "Открыта" : "Закрыта");
  }
}
// ----------------------------------------------------

void card_sw4() {
  Serial.println("Card 4 Button press.");
  st4 = !st4;
  crm.webUpdate("card4", st4 ? "Открыта" : "Закрыта");
}

void card_sw5() {
  Serial.println("Card 5 Button press.");
  st5 = !st5;
  crm.webUpdate("card5", st5 ? "Открыта" : "Закрыта");
}

void tablt2() {
  Serial.println("Button STOP press.");
  crm.webUpdate("t2", String(millis()));
}


void reboot() {
  crm.webNotif("info", "Reboot ESP", 5, 1);

  // Отправляет модуль на перезагрузку, с сохранением настроек, если требуется.
  crm.espReboot();
}



// Метод, вызывается при открытии веб интерфейса.
void interface() {
  // Заголовок новой страницы
  crm.page("&#xe802; Главная");

  //Разделитель
  //crm.output({[Тип], ["Размер в px"], ["Отступы, смотри свойство: margin html"]});
  crm.output({OUTPUT_HR, "1px", "20px 10% -31px"});

  // Вывод значений в виде таблицы
  // crm.output({[Тип], ["ID"], ["Заголовок"], ["Значение при загрузке страницы"], ["цвет в HEX формате"]});
  crm.output({OUTPUT_TABL, "t2", "Заголовок 2", "222", "0f0"});
  crm.output({OUTPUT_TABL, "t3", "Заголовок 3", "333"});
  crm.output({OUTPUT_TABL, "t4", "Заголовок 4", "444", "f0f"});
  crm.output({OUTPUT_HR, "1px", "-3px 10% 0"});

  // График
  // Тип: CHART_L - линии, CHART_B - бары (столбики)
  // Данные: [] - сохранять значения при навигации по разделам; "" - не сохранять
  //crm.chart({ [Тип], ["ID"], ["Заголовок"], ["[Массив заголовков]"], ["[Данные]"], ["цвет в HEX формате"], ["высота графика"] });
  crm.chart({CHART_L, "rssi", "WiFi RSSI", "[1,2,3,4,5]",  "[1,2,3,4,5]", "#00dd00", "250"});

  // Дуговые индикаторы
  // Тип: GAUDE_1 - со стрелкой, GAUDE_2 - без стрелки
  // crm.gauge({[Тип], "[ID]", "[Заголовок]", [Min, шкала], [Max шкала], [Значение при загрузке], {[Цветовая палитра]}, ["Единицы измерения"], [Группировка]});
  crm.gauge({GAUDE_1, "G_0", "WiFi RSSI", -100, -40, (float)WiFi.RSSI(),
    {
      {"#FF0000", "0.0"},   // 0.0 = 0%, 1.0 = 100%
      {"#FFFF00", "0.5"},   // Цвет, расположение на шкале, в формате HEX
      {"#00FF00", "1.0"}    // Количество не больше 6
    }, "dBi"
  });
  crm.gauge({GAUDE_1, "G_1", "Температура", -20, 80, 0,
    {
      {"#FF0000", "-20", "5"},   // Указываются конкретные значения
      {"#FFFF00", "6", "12"},    // Цвет, начало заны, конец зоны, в формате HEX
      {"#00FF00", "13", "24"},   // Количество не больше 6
      {"#FFFF00", "25", "30"},   //
      {"#FF0000", "31", "60"},   //
      {"#FF0000", "62", "80"}    //
    }, "°C",                     // Единицы измерения
    true                         // Группировать с предыдущим, def = false
  });
  crm.gauge({GAUDE_2, "G_2", "Влажность", 0, 100, 35,
    {
      {"#FF0000", "0.0"},        // 0.0 = 0%, 1.0 = 100%
      {"#FF0000", "0.3"},        // Цвет, расположение на шкале, в формате HEX
      {"#FFFF00", "0.7"},        // Количество не больше 6
      {"#00FF00", "1.0"}         //
    }, "%",                      // Единицы измерения
    true                         // Группировать с предыдущим, def = false
  });

  // Плитки / Карточки
  //
  // Переключатель
  // crm.card({[Тип], ["ID"], ["Заголовок"], ["Значение по умолчанию"], ["Значок"], ["цвет в HEX формате"], [Новая группа]});
  // Значок указывается из списка icon.pdf, в формате [&#[CODE]];  без 0
  // Состояние автоматический не сохраняется в память
  crm.card({CARD_CHECKBOX, "card1", "Мотор", "false", "&#xf2c5;", "aaa"});

  // Плитки с графиком
  // Тип: CARD_CHART_L - линии, CARD_CHART_B - бары (столбики)
  // Данные: [] - сохранять значения при навигации по разделам; "" - не сохранять
  // crm.card({[Тип], ["ID"], ["Заголовок"], ["[Массив заголовков]"], ["[Данные]"], ["цвет в HEX формате"], [Новая группа]]});
  crm.card({CARD_CHART_B, "rssiraw", "WiFi RSSI RAW", "[]",  "[]", "#dddd00"});

  // Кнопка
  // crm.card({[Тип], ["ID"], ["Заголовок"], ["Значение по умолчанию"], ["Значок"], ["Цвет"], [Новая группа]});
  crm.card({CARD_BUTTON, "card3", "Дверь 3", (st3 ? "Открыта" : "Закрыта"), "&#xe802;", "0ab", true});
  crm.card({CARD_BUTTON, "card4", "Дверь 4", (st4 ? "Открыта" : "Закрыта"), "&#xe802;", "#a0b"});
  crm.card({CARD_BUTTON, "card5", "Дверь 5", (st5 ? "Открыта" : "Закрыта"), "&#xe805;", "#0ab"});

  // Использование значков: &#[код];
  // Пример:  &#xf1c9;
  String txt = F("&#xf1c9; In computer science, an array data structure, or simply an array, is a data structure consisting of a collection of elements (values or variables), each identified by at least one array index or key. &#xf1c9; An array is stored such that the position of each element can be computed from its index tuple by a mathematical formula.[1][2][3] The simplest type of data structure is a linear array, also called one-dimensional array.");

  // Текстовое поле справка
  //crm.output({[Тип], ["ID"], "["Заголовок"]", ["Текст"], ["цвет в HEX формате"]});
  crm.output({OUTPUT_TEXT, "t1", "Любой текст", txt, "#ff5"});

  // Кнопки
  // crm.input({[Тип], ["ID"], ["Заголовок / значок"], ["Внутренние отступы, смотри: padding html"], ["r - вряд"], ["Размер"]});
  crm.input({INPUT_BUTTON, "b1", "&#xe816;", "10px 11px 10px 15px", "r", "55"});
  crm.input({INPUT_BUTTON, "b2", "&#xe811;", "10px", "r", ""});
  crm.input({INPUT_BUTTON, "b3", "&#xe812;", "10px 30px 10px 15px", "r", "45"});
  crm.input({INPUT_BUTTON, "b4", "&#xe815;", "30px 11px 10px 15px", "r", "35"});


  crm.page("&#xf1de; Настройки");
  // Поле выбора (селект)
   // crm.select({[Тип], ["ID"], ["Заголовок / значок"], ["Значение по умолчанию"], ["Значения {{A:1},{B:2},{N:n}}] });
  crm.select({INPUT_SELECT_PIECE_UP,"select4", "Обновление без конфига", String(select4), {{"Отправить 0", "0"}, {"Отправить 1", "1"}}});
  // crm.select({["ID"], ["Заголовок / значок"], ["Значение по умолчанию"], ["Значения {{A:1},{B:2},{N:n}}] });
  crm.select({"select1", "Доп. опции", "0", {{"Скрыть", "0"}, {"Показать", "1"}}});
  // Получить значение из конфига
  // crm.var(["ID переменной"])
  if (crm.var("select1") == "1") {
    // Поля ввода даты времени
    // crm.input({[Тип], ["ID"], ["Заголовок"]});
    crm.input({INPUT_DATE, "date1", "Дата"});
    crm.input({INPUT_TIME, "time1", "Время"});
    crm.input({INPUT_DATETIME, "datetime", "Дата и Время"});

    // Поле ввода текста и цифр, поддерживает паттерн, смотри в интернете.
    // crm.input({[Тип], ["ID"], ["Заголовок / значок"], ["Значение по умолчанию"], ["паттерн, смотри в инете"]});
    crm.input({INPUT_TEXT, "input1", "Текст (паттерн)", "145", "[0-9]{1,8}"});
    crm.input({INPUT_TEXT, "input2", "Шаблон температурыe", "Температура %T1"});
    crm.input({INPUT_NUMBER, "num1", "Только цыфры", "123"});
    crm.input({INPUT_PASSWORD, "pass1", "Пароль", "123456"});
  }

  // Поле ввода адреса электронной почты, цвета, переключателя (чекбокс).
  // crm.input({[Тип], ["ID"], ["Заголовок"], ["Значение по умолчанию"] });
  crm.input({INPUT_EMAIL, "email", "Мыло", "test@mail.ru"});
  crm.input({INPUT_COLOR, "input3", "Цвет", "#FF22FF"});

  // crm.input({[Тип], ["ID"], ["Заголовок"], ["Значение по умолчанию"], ["Обновление интерфейса: 1"] });
  crm.input({INPUT_CHECKBOX, "chk1", "Кнопка перезагрузки", "false", "1"});
  crm.input({INPUT_CHECKBOX, "chk2", "Подсветка", "true"});

  // Смотри выше ^
  crm.output({OUTPUT_TEXT, "t11", "", txt, "#5f5"});

  // Разделитель заголовок
  // crm.output({ [Тип], ["ID"], ["Заголовок"], ["Выравнивание"], ["#Цвет RGB"], ["Размер px"] });
  // Выравнивание: left, center, right;
  // Если не заменять заголовок через webUpdate, поле ID можно оставить пустым.
  crm.output({OUTPUT_LABEL, "lb1", "Ползунки", "left", "#0f0", "20"});

  // Ползунок
  // crm.range({["ID"], ["Заголовок"], ["Значение по умолчанию"], ["MIN"], ["MAX"], [Шаг], ["Единицы измерения"]});
  crm.range({"range1", "Громкость", 12, 0, 84, 1});           //Без единиц измерения
  crm.range({"range2", "Яркость", 52, 0, 84, 1, " попугаея"});  //С единицами измерения
  if (crm.var("chk1") == "1") crm.input({INPUT_BUTTON, "reboot", "&#xe810;", "8px 9px 8px 14px", "row", "50"});

  crm.page("<z class='zanim'>&#xe82b;</z> Wi-Fi");
  // форма с полями для WiFi
  // crm.wifiForm([Режим работы], ["Название ТД"], ["Пароль ТД"], ["WiFi сеть для подключения", ["Пароль сети"], ["Время ожидания подключения"]]);
  // Режим работы: WIFI_AP - точка доступа, WIFI_STA - клиент, WIFI_AP_STA - ТД + Клиент
  crm.wifiForm(WIFI_AP, "MY-AP");
  crm.input({INPUT_BUTTON, "reboot", "Перезагрузить"});
}

void pieceUpdate(String paramName, String value) {
  if(paramName == "select4"){
    select4 = !select4;
    Serial.printf("[Update_P] Select обновлен на %s\n без конфига",value.c_str());
    crm.webUpdate();
  }
}


void setup() {
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(4, INPUT_PULLUP);            // Button pin

  // Отключает управление WiFi средствами библиотеки
  //crm.disableWiFiManagement();

  // Включает возможность прошивать модуль по сети через Arduino IDE
  crm.useArduinoOta();

  // callBackButtonEvent. Обработка web кнопок по событию
  // crm.btnCallback([Event function]);
  crm.btnCallback(btnResponce);
  
  // обновление по кусочкам
  crm.updatePiece(pieceUpdate);

  // Инициализация библиотеки, памяти и прочих функций
  // Параметры со * обязательны.
  // crm.begin("[*Название проекта]", [*Ф-я интерфейса], [Ф-я обновления переменных], [Ф-я API], [Скорость серийного порта, доп. отладка]);
  //crm.begin("Project-28", interface, update);
  //crm.begin("Project-28", interface, NULL, NULL, 115200);
  crm.begin("DEMO Project", interface, update, api, 115200);

  // Авторизация в веб интерфейсе.
  // Параметры со * обязательны.
  // crm.setWebAuth("[*Имя пользователя]", "[Пароль]");
  //crm.setWebAuth("admin", "admin");

  // Авторизация на устройстве для API запросов. Без setWebAuth не работает.
  // Пример запроса: http://IP/api?k=d1h6d5&p1=2&...,
  //  где первый параметр должен быть ключ, имя параметра любое
  // crm.setApiKey("[API ключ]");
  //crm.setApiKey("d1h6d5");

  // Задать лицензионный ключ
  // Позволяет отключить уведомление о бесплатной версии, а так же задать свои контакты.
  // Переменные со * обязательны.
  // crm.license([*Лицензионный ключ], [Электронная почта], [Телеграм], [Домашняя страница]);
  //crm.license("5s72to1", "crm.dev@bk.ru", "user624", "https://github.com/WonderCRM/CRMui3");

  // Версия прошивки вашего проекта, если не используется, то отображается версия CRMui
  // crm.version ("[Любая строка]");
  crm.version("1.2.3.4.5");

  // Аналог FreeRTOS
  // NAME.once_ms(ms, Fn); - Выполнить единожды через указанный интервал
  // NAME.attach_ms(ms, Fn); - Цикличное выполнение через указанный интервал
  // NAME.detach(); - Деактивировать
  myLoop.attach_ms(2000, myLoopRun);

  // crm.wifiScan()
  // Возвращает список найденных точек доступа в виде JSON строки.
  // Переменные: s - статус / количество сетей; n - массив сетей [Имя, канал, rssi, шифрование], ...
  // Статус: -2 - сканирование не запущено; -1 - сканирует диапазоны; 0+ - количество найденных сетей
  // Способ опроса асинхронный, задержка минимум, ответ по готовности при следующем запросе

  // Конвертирование uint64_t в String
  // crm.uint64ToStr(uint64_t);
  //
  // Только для ESP32
  //
  // Глубокий / Лёгкий сон
  // Режим: 1 - глубокий, 0 - Лёгкий (сохраняются значения всех переменных)
  // crm.espSleep([Время в секундах], [режим]);
}


void loop() {
  // Обслуживание системных функций библиотеки
  crm.run();

  // Проверка состояния нажатия совтовых кнопок. Проверка не обязательна.
  if (crm.btnSwStatus()) {
    // Проверка конкретных кнопок на нажатие
    // crm.btnCallback("[ID кнопки]", [Функция для выполнения]);
    crm.btnCallback("reboot", reboot);    // Check "reboot" SW button
    crm.btnCallback("card4", card_sw4);   // Check "card4" SW button
    crm.btnCallback("card5", card_sw5);   // Check "card5" SW button
    crm.btnCallback("b3", tablt2);        // Check "b3" SW button
  }
  // Проверка аппаратных кнопок на нажатие
  // crm.btnCallback("[пин подключения кнопки]", [Функция для выполнения], [уровень при нажатии]);
  crm.btnCallback(4, hw_butt, LOW);      // Check pin4 HW button
}