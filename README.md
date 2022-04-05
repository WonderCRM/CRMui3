# CRMui3 WebFramework для esp8266 и esp32

## Описание проекта
**CRMui3** библиотека для Arduino IDE и ей подобных. Позволяет быстро и просто создать веб интерфейс для настройки и управления устройством на базе ESP8266 и ESP32.
Данный проект не претендует на какую-либо премию, распространяется как есть. Полностью бесплатен для личного использования.

**P.S**. я не программист, это моё хобби, изящность и рациональность написания того или иного участка кода не гарантирую, но к этому стремлюсь.
Конструктивная критика, а также предложения по улучшению приветствуется.

## По вопросом сотрудничества, контакты ниже.
* [**Канал этого проекта и других моих**](https://t.me/s/CRMdevelop)
* [**Моя Телега**](https://t.me/User624)
* [**Мой Mail**](mailto:crm.dev@bk.ru)
* [**Донат (Благодарность)**](https://donatepay.ru/don/crmdev)

## Железо
Совместим с микроконтроллерами на ESP8266 (**ESP-12F лучше**) и ESP32.

## Папки и файлы
- **libraries** - библиотеки, используемые в данном проекте (распаковать);
- **scr** - сам CRMui3 WebFramework.
- **_icon.pdf** - Набор иконок

## Использование
Описание методов и их использование есть в примере, идущем вместе с библиотекой.
Параметры загрузки в Arduino IDE ниже на картинке. Для 8266 lwIP Variant обязательно.
Как устанавливать библиотеки, работать с Arduino IDE и загружать прошивку в ESP можно посмотреть тут:
* [**Установка ESP8266 в Arduino IDE (руководство для ОС Windows)**](https://wiki.iarduino.ru/page/WEMOS_start/)
* [**Уроки Arduino**](https://alexgyver.ru/lessons/) (раздел "Введение в Arduino")
* Так же есть видео с примерами: [**Заметки Ардуинщика**](https://www.youtube.com/c/ЗаметкиАрдуинщика/videos)

## Скриншоты
![PROJECT_PHOTO](https://github.com/WonderCRM/CRMui3/blob/main/scr1.png)
![PROJECT_PHOTO](https://github.com/WonderCRM/CRMui3/blob/main/scr2.png)
![PROJECT_PHOTO](https://github.com/WonderCRM/CRMui3/blob/main/scr3.png)
![PROJECT_PHOTO](https://github.com/WonderCRM/CRMui3/blob/main/scr4.png)

## Параметры загрузки в Arduino IDE
* Для ESP8266, **если работает не стабильно!**

![PROJECT_PHOTO](https://github.com/WonderCRM/CRMui3/blob/main/IDE_8266.png)

*  Для ESP32
 
![PROJECT_PHOTO](https://github.com/WonderCRM/CRMui3/blob/main/IDE_32.png)

## Отличие от оригинала
Реализован метод который позволяет передавать данные веб интерфейсу не через конфиг и обратно
Реализован подобие pwa приложения