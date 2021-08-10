//Example use CRMui3  /  Пример использования CRMui3

#include "CRMui3.h"
#include <Ticker.h> // Входит в состав ядра

// Объявление объектов
CRMui3 crm;     // CRMui
Ticker myLoop;  // Ticker

// Переменные в примере
bool st3, st4, st5;


void setup() {
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(4, INPUT_PULLUP);            // Button pin

  // Инициализация библиотеки, памяти и прочих функций
  // Параметры со * обязательны.
  // crm.begin("[*Название проекта]", [*Ф-я интерфейса], [Ф-я обновления переменных], [Ф-я API], [Скорость серийного порта, доп. отладка]);
  //crm.begin("Project-28", interface, update);
  //crm.begin("Project-28", interface, NULL, NULL, 115200);
  crm.begin("Project-28", interface, update, api, 115200);

  // Авторизация в веб интерфейсе
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

  // Аналог FreeRTOS
  // NAME.once_ms(ms, Fn); - Выполнить единожды через указанный интервал
  // NAME.attach_ms(ms, Fn); - Цикличное выполнение через указанный интервал
  // NAME.detach(); - Деактивировать
  myLoop.attach_ms(2000, myLoopRun);
}


void loop() {
  // Обслуживание системных функций библиотеки
  crm.run();

  // Проверка состояния нажатия совтовых кнопок. Проверка не обязательна.
  if (crm.btnSwStatus()) {
    // Проверка конкретных кнопок на нажатие
    // crm.btnCallback("[ID кнопки]", [Функция для выполнения]);
    crm.btnCallback("reboot", reboot);    // Check "reboot" SW button
    crm.btnCallback("card3", card_sw3);   // Check "card3" SW button
    crm.btnCallback("card4", card_sw4);   // Check "card4" SW button
    crm.btnCallback("card5", card_sw5);   // Check "card5" SW button
    crm.btnCallback("b3", tablt2);        // Check "b3" SW button
  }
  // Проверка аппаратных кнопок на нажатие
  // crm.btnCallback("[пин подключения кнопки]", [Функция для выполнения], [уровень при нажатии]);
  crm.btnCallback(4, hw_butt, LOW);      // Check pin33 HW button
}


void myLoopRun() {
  static int a[3] = {};
  static int i = 0;
  if (i > 2) i = 0;
  a[i] = WiFi.RSSI();

  // Обновление значений элементов веб интерфейса
  // crm.webUpdate("[ID элемента]", "[Значение]");
  // Интервал отправки 1 раз в сек.
  crm.webUpdate("rssi", String((a[0] + a[1] + a[2]) / 3));
  crm.webUpdate("rssiraw", String(a[i]));
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
    crm.webNotif(a ? "Red" : "Green", a ? "Motor start" : "Motor stop", 5);

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
    crm.apiResponse("Random", String(random(String(rnd).toInt())));
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

void tablt2() {
  Serial.println("Button STOP press.");
  crm.webUpdate("t2", String(millis()));
}

void card_sw3() {
  Serial.println("Card 3 Button press.");
  st3 = !st3;
  crm.webUpdate("card3", st3 ? "Open" : "Close");
}

void card_sw4() {
  Serial.println("Card 4 Button press.");
  st4 = !st4;
  crm.webUpdate("card4", st4 ? "Open" : "Close");
}

void card_sw5() {
  Serial.println("Card 5 Button press.");
  st5 = !st5;
  crm.webUpdate("card5", st5 ? "Open" : "Close");
}


void reboot() {
  crm.webNotif("info", "Reboot ESP", 5, 1);

  // Отправляет модуль на перезагрузку, с сохранением настроек, если требуется.
  crm.espReboot();
}



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


// Метод, вызывается при открытии веб интерфейса.
void interface() {
  // Заголовок новой страницы
  crm.page("Dash board");

  //Разделитель
  //crm.output({[Тип], ["Размер в px"], ["Отступы, смотри свойство: margin html"]});
  crm.output({OUTPUT_HR, "1px", "20px 10% -31px"});

  // Вывод значений в виде таблицы
  // crm.output({[Тип], ["ID"], ["Заголовок"], ["Значение при загрузке страницы"], ["цвет в HEX формате"]});
  crm.output({OUTPUT_TABL, "t2", "Label 2", "222", "0f0"});
  crm.output({OUTPUT_TABL, "t3", "Label 3", "333"});
  crm.output({OUTPUT_TABL, "t4", "Label 4", "444", "f0f"});
  crm.output({OUTPUT_HR, "1px", "-3px 10% 0"});

  // График
  // Тип: CHART_L - линии, CHART_B - бары (столбики)
  //crm.chart({[Тип], ["ID"], ["Заголовок"], ["[Массив заголовков]"], ["[Данные]"], ["цвет в HEX формате"], ["высота графика"]});
  crm.chart({CHART_L, "rssi", "WiFi RSSI", "[1,2,3,4,5,6]",  "[1,5,3,2,6,3]", "#00dd00", "250"});

  // Плитки / Карточки
  // Переключатель
  // crm.card({[Тип], ["ID"], ["Заголовок"], ["Значение по умолчанию"], ["Значок"], ["цвет в HEX формате"], [Новая группа]});
  // Значок указывается из списка icon.pdf, в формате &#[CODE];  без 0
  crm.card({CARD_CHECKBOX, "card1", "Motor", "false", "&#xf2c5;", "aaa"});
  
  // Плитки с нрафиком
  // Тип: CARD_CHART_L - линии, CARD_CHART_B - бары (столбики)
  // crm.card({[Тип], ["ID"], ["Заголовок"], ["[Массив заголовков]"], ["[Данные]"], ["цвет в HEX формате"], [Новая группа]]});
  crm.card({CARD_CHART_B, "rssiraw", "WiFi RSSI RAW", "",  "", "#dddd00"});
  
  // Кнопка
  // crm.card({[Тип], ["ID"], ["Заголовок"], ["Значение по умолчанию"], ["Значок"], ["Цвет"], [Новая группа]});
  crm.card({CARD_BUTTON, "card3", "Door 3", (st3 ? "Open" : "Close"), "&#xe802;", "0ab", true});
  crm.card({CARD_BUTTON, "card4", "Door 4", (st4 ? "Open" : "Close"), "&#xe802;", "#a0b"});
  crm.card({CARD_BUTTON, "card5", "Door 5", (st5 ? "Open" : "Close"), "&#xe805;", "#0ab"});

  // Для отображения значков в текстовых полях заключаем их в <z></z>
  // Пример <z>&#xf1c9;</z>
  String txt = F("<z>&#xf1c9;</z> In computer science, an array data structure, or simply an array, is a data structure consisting of a collection of elements (values or variables), each identified by at least one array index or key. An array is stored such that the position of each element can be computed from its index tuple by a mathematical formula.[1][2][3] The simplest type of data structure is a linear array, also called one-dimensional array.");

  // Текстовое поле справка
  //crm.output({[Тип], ["ID"], "["Заголовок"]", ["Текст"], ["цвет в HEX формате"]});
  crm.output({OUTPUT_TEXT, "t1", "Any label text", txt, "#ff5"});

  // Кнопки
  // crm.input({[Тип], ["ID"], ["Заголовок / значок"], ["Внутренние отступы, смотри: padding html"], ["r - вряд"], ["Размер"]});
  crm.input({INPUT_BUTTON, "b1", "&#xe816;", "10px 11px 10px 15px", "r", "55"});
  crm.input({INPUT_BUTTON, "b2", "&#xe811;", "10px", "r", ""});
  crm.input({INPUT_BUTTON, "b3", "&#xe812;", "10px 30px 10px 15px", "r", "45"});
  crm.input({INPUT_BUTTON, "b4", "&#xe815;", "30px 11px 10px 15px", "r", "35"});


  crm.page("Settings");
  // Поле выбора (селект)
  // crm.select({["ID"], ["Заголовок / значок"], ["Значение по умолчанию"], ["Значения {{A:1},{B:2},{N:n}}] });
  crm.select({"select1", "Elements", "0", {{"Hide", "0"}, {"Show", "1"}}});
  // Получить значение из конфига
  // crm.var(["ID переменной"])
  if (crm.var("select1").toInt() > 0) {
    // Поля ввода даты времени
    // crm.input({[Тип], ["ID"], ["Заголовок"]});
    crm.input({INPUT_DATE, "date1", "Date"});
    crm.input({INPUT_TIME, "time1", "Time"});
    crm.input({INPUT_DATETIME, "datatime1", "Date & Time"});

    // Поле ввода текста и цифр, поддерживает паттерн, смотри в интернете.
    // crm.input({[Тип], ["ID"], ["Заголовок / значок"], ["Значение по умолчанию"], ["паттерн, смотри в инете"]});
    crm.input({INPUT_TEXT, "input1", "Text (pattern)", "145", "[0-9]{1,8}"});
    crm.input({INPUT_TEXT, "input2", "Output template", "Температура %T1"});
    crm.input({INPUT_NUMBER, "num1", "Only number", "123"});
  }

  // Поле ввода адреса электронной почты, цвета, переключателя (чекбокс).
  // crm.input({[Тип], ["ID"], ["Заголовок"], ["Значение по умолчанию"] });
  crm.input({INPUT_EMAIL, "email", "Your mail", "test@mail.ru"});
  crm.input({INPUT_COLOR, "input3", "Color", "#FF22FF"});
  crm.input({INPUT_CHECKBOX, "chk1", "Button Reboot", "false"});
  
  // Смотри выше ^
  crm.output({OUTPUT_TEXT, "t11", "", txt, "#5f5"});

  // Ползунок
  // crm.range({["ID"], ["Заголовок"], ["Значение по умолчанию"], ["MIN"], ["MAX"], [Шаг], ["Единицы измерения"]});
  crm.range({"range1", "Volume", 12, 0, 84, 1});						//Без единиц измерения
  crm.range({"range2", "Brightness", 52, 0, 84, 1, " lux"});	//С единицами измерения
  if (crm.var("chk1") == "true") crm.input({INPUT_BUTTON, "reboot", "&#xe810;", "8px 9px 8px 14px", "row", "50"});

  crm.page("Wi-Fi");
  // форма с полями для WiFi
  // crm.wifiForm([Режим работы], ["Название ТД"], ["Пароль ТД"], ["WiFi сеть для подключения", ["Пароль сети"], ["Время ожидания подключения"]]);
  // Режим работы: WIFI_AP - точка доступа, WIFI_STA - клиент, WIFI_AP_STA - ТД + Клиент
  crm.wifiForm(WIFI_AP, "MY-AP");
  crm.input({INPUT_BUTTON, "reboot", "REBOOT"});
}
