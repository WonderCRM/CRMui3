# CRMui3 WebFramework для esp8266 и esp32

## Описание проекта
**CRMui3** библиотека для Arduino IDE и ей подобных. Позволяет быстро и просто создать веб интерфейс для настройки и управления устройством на базе ESP8266 и ESP32.
Данный проект не претендует на какую-либо премию, распространяется как есть. Полностью бесплатен для личного использования.

**P.S.** Я не программист, это моё хобби, изящность и рациональность написания того или иного участка кода не гарантирую, но к этому стремлюсь.
Конструктивная критика, а также предложения по улучшению приветствуется.

## Если хотите поддержать, контакты ниже.
* [**Канал этого проекта и других моих**](https://t.me/s/CRMdevelop)
* [**Моя Телега**](https://t.me/User624)
* [**Мой Mail**](mailto:crm.dev@bk.ru)

## Железо
Совместим с микроконтроллерами на ESP8266 (**ESP-12F лучше**) и ESP32.

## ПО
Проверена работа на Arduino IDE
* ESP8266 - ядро 3.0.2
* ESP32 - ядро 1.0.6
* ESP32 - ядро выше 2.0, требуется в файле wi-fi.cpp, закомментировать 13 строку и раскомментировать 16 строку.

## Папки и файлы
- **libraries** - библиотеки, используемые в данном проекте (распаковать);
- **scr** - сам CRMui3 WebFramework.
- **_icon.pdf** - Набор иконок

## Использование
Описание методов и их использование есть в примере, идущем вместе с библиотекой.
Параметры загрузки в Arduino IDE ниже на картинке.
Как устанавливать библиотеки, работать с Arduino IDE и загружать прошивку в ESP можно посмотреть тут:
* [**Установка ESP8266 в Arduino IDE (руководство для ОС Windows)**](https://wiki.iarduino.ru/page/WEMOS_start/)
* [**Уроки Arduino**](https://alexgyver.ru/lessons/before-start/) (раздел "НАЧАЛО РАБОТЫ С ARDUINO IDE")
* Так же есть видео с примерами: [**Заметки Ардуинщика**](https://www.youtube.com/c/ЗаметкиАрдуинщика/videos)

## Параметры загрузки в Arduino IDE
* Для ESP8266

![PROJECT_PHOTO](https://github.com/WonderCRM/CRMui3/blob/main/IDE_8266.png)

*  Для ESP32

![PROJECT_PHOTO](https://github.com/WonderCRM/CRMui3/blob/main/IDE_32.png)

## Скриншоты
![PROJECT_PHOTO](https://github.com/WonderCRM/CRMui3/blob/main/scr1.png)

![PROJECT_PHOTO](https://github.com/WonderCRM/CRMui3/blob/main/scr2.png)

![PROJECT_PHOTO](https://github.com/WonderCRM/CRMui3/blob/main/scr3.png)

![PROJECT_PHOTO](https://github.com/WonderCRM/CRMui3/blob/main/scr4.png)

![PROJECT_PHOTO](https://github.com/WonderCRM/CRMui3/blob/main/scr5.png)
