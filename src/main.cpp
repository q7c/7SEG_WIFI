#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include <time.h>
#include "effects.h"
#include <FS.h>
#include <FastLED.h>
#include "FastLED_RGBW.h"
#include <ESP8266HTTPUpdateServer.h>
#include "index.h"

#define LED_PIN 2 // GPIO2 (D4)
#define NUM_LEDS 87
#define INDICATOR_LEDS 3 // Используем только первые 3 светодиода

CRGBW leds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0]; // FastLED with RGBW

uint8_t hue = 0;
uint8_t saturation = 255;
uint8_t value = 255;    
bool blinkState = false;

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

// Настройки WiFi
const char* ssid = "q7c";
const char* password = "11111111";

// const uint8_t kMatrixY = 9;
// const uint8_t kMatrixX = 21;

int matrix[kMatrixY][kMatrixX] = {-1};

/*
Виртуальная матрица 9x21 с наложенными дисплеями:

    0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20
  ┌───────────────────────────────────────────────────────────────────────────────────┐
0 │  -1   8   7   6  -1  -1  29  28  27  -1  -1  -1  53  52  51  -1  -1  74  73  72  -1 │
1 │   9  -1  -1  -1   5  30  -1  -1  -1  26  -1  54  -1  -1  -1  50  75  -1  -1  -1  71 │
2 │  10  -1  -1  -1   4  31  -1  -1  -1  25  42  55  -1  -1  -1  49  76  -1  -1  -1  70 │
3 │  11  -1  -1  -1   3  32  -1  -1  -1  24  -1  56  -1  -1  -1  48  77  -1  -1  -1  69 │
4 │  -1   0   1   2  -1  -1  21  22  23  -1  -1  -1  45  46  47  -1  -1  66  67  68  -1 │
5 │  12  -1  -1  -1  20  33  -1  -1  -1  41  -1  57  -1  -1  -1  65  78  -1  -1  -1  86 │
6 │  13  -1  -1  -1  19  34  -1  -1  -1  40  43  58  -1  -1  -1  64  79  -1  -1  -1  85 │
7 │  14  -1  -1  -1  18  35  -1  -1  -1  39  -1  59  -1  -1  -1  63  80  -1  -1  -1  84 │
8 │  -1  15  16  17  -1  -1  36  37  38  -1  44  -1  60  61  62  -1  -1  81  82  83  -1 │
  └───────────────────────────────────────────────────────────────────────────────────┘

Сегменты дисплеев:

Дисплей 1 (индексы 0-20):
 ┌───┐
 │ A │
F│   │B
 ├─G─┤
E│   │C
 └───┘
   D

Дисплей 2 (индексы 21-41):
 ┌───┐
 │ A │
F│   │B
 ├─G─┤
E│   │C
 └───┘
   D

Дисплей 3 (индексы 45-65):
 ┌───┐
 │ A │
F│   │B
 ├─G─┤
E│   │C
 └───┘
   D

Дисплей 4 (индексы 66-86):
 ┌───┐
 │ A │
F│   │B
 ├─G─┤
E│   │C
 └───┘
   D

Дополнительные диоды:
- Индекс 42: Дополнительный диод 1
- Индекс 43: Дополнительный диод 2
- Индекс 44: Дополнительный диод 3
*/

const int ledMatrix[kMatrixY][kMatrixX] = {
    { -1,   8,   7,   6,  -1,    -1,  29,  28,  27,  -1,    -1,    -1,  53,  52,  51,  -1,    -1,  74,  73,  72,  -1},
    {  9,  -1,  -1,  -1,   5,    30,  -1,  -1,  -1,  26,    -1,    54,  -1,  -1,  -1,  50,    75,  -1,  -1,  -1,  71},
    { 10,  -1,  -1,  -1,   4,    31,  -1,  -1,  -1,  25,    42,    55,  -1,  -1,  -1,  49,    76,  -1,  -1,  -1,  70},
    { 11,  -1,  -1,  -1,   3,    32,  -1,  -1,  -1,  24,    -1,    56,  -1,  -1,  -1,  48,    77,  -1,  -1,  -1,  69},
    { -1,   0,   1,   2,  -1,    -1,  21,  22,  23,  -1,    -1,    -1,  45,  46,  47,  -1,    -1,  66,  67,  68,  -1},
    { 12,  -1,  -1,  -1,  20,    33,  -1,  -1,  -1,  41,    -1,    57,  -1,  -1,  -1,  65,    78,  -1,  -1,  -1,  86},
    { 13,  -1,  -1,  -1,  19,    34,  -1,  -1,  -1,  40,    43,    58,  -1,  -1,  -1,  64,    79,  -1,  -1,  -1,  85},
    { 14,  -1,  -1,  -1,  18,    35,  -1,  -1,  -1,  39,    -1,    59,  -1,  -1,  -1,  63,    80,  -1,  -1,  -1,  84},
    { -1,  15,  16,  17,  -1,    -1,  36,  37,  38,  -1,    44,    -1,  60,  61,  62,  -1,    -1,  81,  82,  83,  -1}
};

// Определение сегментов для цифр 0-9
const uint8_t digitSegments[10][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}  // 9
};

// Определение типа указателя на функцию эффекта
typedef void (*EffectFunction)(CRGBW*, int[kMatrixY][kMatrixX], int, int, int, int, bool);

// Массив указателей на функции эффектов
EffectFunction effects[] = {
    redEffect,
    blueRedEffect,
    rainbowEffect,
    fireEffect,
    auroraEffect,
    starryEffect,
    waveEffect,
    confettiEffect,
    breathingEffect,
    sparkleRainbowEffect,
    runningLightsEffect
};

const int numEffects = sizeof(effects) / sizeof(effects[0]);

// Функция для индикации подключения к Wi-Fi
void indicateConnecting() {
    for (int i = 0; i < INDICATOR_LEDS; i++) {
        leds[i] = CRGBW(0, 0, 255, 0); // Синий
        FastLED.show();
        delay(200);
        leds[i] = CRGBW(0, 0, 0, 0); // Выключить
        FastLED.show();
    }
}

// Функция для индикации успешного подключения
void indicateConnected() {
    for (int j = 0; j < 3; j++) { // Тройное мигание
        for (int i = 0; i < INDICATOR_LEDS; i++) {
            leds[i] = CRGBW(0, 255, 0, 0); // Синий
        }
        FastLED.show();
        delay(300); // Задержка 500 мс
        for (int i = 0; i < INDICATOR_LEDS; i++) {
            leds[i] = CRGBW(0, 0, 0, 0); // Выключить
        }
        FastLED.show();
        delay(300); // Задержка 500 мс
    }
}

// Функция для индикации создания точки доступа
void indicateAPMode() {
    for (int i = 0; i < INDICATOR_LEDS; i++) {
        leds[i] = CRGBW(255, 0, 0, 0); // Красный
    } 
    FastLED.show();
}

void drawDigit(uint8_t digit, uint8_t position, uint8_t huematrix) {
    Serial.printf("Отображение цифры %d на позиции %d\n", digit, position);

    uint8_t offsetX = (position * 5) + (position > 1 ? 1 : 0); 
    uint8_t offsetY = 0;

    Serial.printf("Смещение по X: %d, Смещение по Y: %d\n", offsetX, offsetY);
    
    for (uint8_t i = 0; i < 7; i++) {
        if (digitSegments[digit][i] == 1) {
            //Serial.printf("Включение сегмента %d для цифры %d на позиции %d\n", i, digit, position);
            switch (i) {
                case 0: // Сегмент A
                    matrix[offsetY + 0][offsetX + 1] = huematrix;
                    matrix[offsetY + 0][offsetX + 2] = huematrix;
                    matrix[offsetY + 0][offsetX + 3] = huematrix; 
                    break;
                case 1: // Сегмент B
                    matrix[offsetY + 1][offsetX + 4] = huematrix;
                    matrix[offsetY + 2][offsetX + 4] = huematrix;
                    matrix[offsetY + 3][offsetX + 4] = huematrix;
                    break;
                case 2:// Сегмент C
                    matrix[offsetY + 5][offsetX + 4] = huematrix;
                    matrix[offsetY + 6][offsetX + 4] = huematrix;
                    matrix[offsetY + 7][offsetX + 4] = huematrix;
                    break;
                case 3: // Сегмент D
                    matrix[offsetY + 8][offsetX + 1] = huematrix;
                    matrix[offsetY + 8][offsetX + 2] = huematrix;
                    matrix[offsetY + 8][offsetX + 3] = huematrix;
                    break;
                case 4: // Сегмент E
                    matrix[offsetY + 5][offsetX + 0] = huematrix;
                    matrix[offsetY + 6][offsetX + 0] = huematrix;
                    matrix[offsetY + 7][offsetX + 0] = huematrix;
                    break;
                case 5: // Сегмент F
                    matrix[offsetY + 1][offsetX + 0] = huematrix;
                    matrix[offsetY + 2][offsetX + 0] = huematrix;
                    matrix[offsetY + 3][offsetX + 0] = huematrix;
                    break;
                case 6: // Сегмент G
                    matrix[offsetY + 4][offsetX + 1] = huematrix;
                    matrix[offsetY + 4][offsetX + 2] = huematrix;
                    matrix[offsetY + 4][offsetX + 3] = huematrix;
                    break;
            }
        }
    }
    
    Serial.printf("Цифра %d успешно отображена на позиции %d\n", digit, position);
}

void updateLEDs() {
    Serial.println("Обновление светодиодов");
    for (uint8_t y = 0; y < kMatrixY; y++) {
        for (uint8_t x = 0; x < kMatrixX; x++) {
            int ledIndex = ledMatrix[y][x];
            if (ledIndex != -1) {
                // Установка цвета светодиода в зависимости от значения в матрице
                // Если значение в матрице больше или равно 0, устанавливаем цвет светодиода по оттенку, иначе выключаем светодиод
                leds[ledIndex] = (matrix[y][x] >= 0) ? CHSV(matrix[y][x], 255, 128) : CHSV(0, 0, 0); // Уменьшаем яркость до 128
            }
        }
    }
    FastLED.show();
    Serial.println("Светодиоды обновлены");
}

void displayNumber(int number) {
    Serial.printf("Функция displayNumber() вызвана с аргументом %d\n", number);
    
    // Очистка матрицы
    for (uint8_t y = 0; y < kMatrixY; y++) {
        for (uint8_t x = 0; x < kMatrixX; x++) {
            matrix[y][x] = -1;
        }
    }
    
    // Разбиение числа на отдельные цифры
    int digits[4];
    for (int i = 3; i >= 0; i--) {
        digits[i] = number % 10;
        number /= 10;
    }

    // Отображение цифр на дисплеях
    uint8_t hueStep = 256 / 4; // Шаг изменения оттенка для каждой цифры
    for (int i = 0; i < 4; i++) {
        uint8_t hue = i * hueStep; // Вычисление оттенка для текущей цифры
        drawDigit(digits[i], i, hue);
    }
}

void displayTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Не удалось получить время");
        return;
    }
    
    int hours = timeinfo.tm_hour;
    int minutes = timeinfo.tm_min;
    
    int displayTime = hours * 100 + minutes;
    
    displayNumber(displayTime);
}


const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 10800; // Смещение времени в секундах для вашего часового пояса
const int daylightOffset_sec = 0; // Смещение времени в секундах для летнего времени

WiFiUDP ntpUDP;

void setup() {
    //Serial.begin(115200);
    Serial.println("Запуск");

    // Настройка синхронизации времени
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    // Ожидание успешной синхронизации времени
    Serial.print("Ожидание синхронизации времени...");
    unsigned long startTime = millis();
    while (!time(nullptr) && millis() - startTime < 10000) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    if (!time(nullptr)) {
        Serial.println("Не удалось синхронизировать время");
    } else {
        Serial.println("Время синхронизировано");
    }

    // Инициализация светодиодов
    FastLED.addLeds<WS2812B, LED_PIN, RGB>(ledsRGB, getRGBWsize(NUM_LEDS));

    FastLED.clear();
    FastLED.show();

    // Настройка Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    // Попытка подключения к Wi-Fi
    for (int i = 0; i < 10; i++) { // Попробуем 10 раз
        indicateConnecting();
        
        if (WiFi.waitForConnectResult() != WL_CONNECTED) {
            Serial.println("Ошибка подключения! Повторная попытка...");
            delay(1000);
        } else {
            Serial.println("Подключение успешно!");
            break;
        }
    }

    // Если не удалось подключиться, создаем точку доступа
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.softAP("ESP8266_AP");
        Serial.println("Точка доступа создана. IP адрес: " + WiFi.softAPIP().toString());
        Serial.println("Подключение успешно! здеся");
        indicateAPMode();

        // Настройка веб-сервера
        server.on("/", HTTP_GET, []() {
            server.send(200, "text/html", html);
        });

        server.on("/connect", HTTP_POST, []() {
            String ssid = server.arg("ssid");
            String password = server.arg("password");

            // Подключение к выбранной сети
            WiFi.begin(ssid.c_str(), password.c_str());
            indicateConnecting();
            server.send(200, "text/html; charset=UTF-8", "<html><body><h1>Подключение...</h1></body></html>");
        });

        // Обработчик для получения списка доступных Wi-Fi сетей
        server.on("/networks", HTTP_GET, []() {
            // Сканируем доступные сети Wi-Fi и сохраняем количество найденных сетей
            int n = WiFi.scanNetworks();
            // Инициализируем строку для хранения имен сетей в формате JSON
            String networks = "[";
            // Проходим по всем найденным сетям
            for (int i = 0; i < n; ++i) {
                // Добавляем имя сети в строку, обрамляя его кавычками
                networks += "\"" + WiFi.SSID(i) + "\"";
                // Если это не последняя сеть, добавляем запятую
                if (i < n - 1) {
                    networks += ",";
                }
            }
            // Завершаем строку JSON
            networks += "]";
            server.send(200, "application/json", "{\"networks\":" + networks + "}");
        });



        httpUpdater.setup(&server);
        server.begin();
    } else {
        // Если подключение успешно, показываем страницу для OTA
        indicateConnected();
        server.on("/", HTTP_GET, []() {
            server.send(200, "text/html", html);
        });

        server.on("/update", HTTP_POST, []() {
            server.sendHeader("Connection", "close");
            server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
            Serial.println("Запрос на обновление получен");
            ESP.restart();
        }, []() {
            HTTPUpload& upload = server.upload();
            if (upload.status == UPLOAD_FILE_START) {
                Serial.printf("Начало загрузки: %s\n", upload.filename.c_str());
                if (!Update.begin(upload.totalSize)) {
                    Update.printError(Serial);
                }
            } else if (upload.status == UPLOAD_FILE_WRITE) {
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                    Update.printError(Serial);
                }
            } else if (upload.status == UPLOAD_FILE_END) {
                if (Update.end(true)) {
                    Serial.printf("Загрузка завершена: %u байт\nПерезагрузка...\n", upload.totalSize);
                } else {
                    Update.printError(Serial);
                }
            }
        });

        httpUpdater.setup(&server);
        server.begin();
    }

    // Настройка OTA
    ArduinoOTA.setHostname("esp8266-ota");

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "флеш-память";
        } else { // U_SPIFFS
            type = "файловая система";
        }
        Serial.println("Начало OTA обновления: " + type);
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nОбновление завершено");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Прогресс: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Ошибка[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Ошибка аутентификации");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Ошибка начала OTA");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Ошибка соединения");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Ошибка получения данных");
        else if (error == OTA_END_ERROR) Serial.println("Ошибка завершения");
    });

    ArduinoOTA.begin();
    Serial.println("OTA готов");

}

void loop() {
    ArduinoOTA.handle(); // Обработка OTA
    server.handleClient(); // Обработка запросов веб-сервера

    static unsigned long previousMillis = 0;
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= 1000) {
        previousMillis = currentMillis;
        displayTime();
    }

    static int currentEffect = 0;
    static unsigned long lastEffectChange = 0;
    
    if (currentMillis - lastEffectChange >= 10000) {
        lastEffectChange = currentMillis;
        currentEffect = (currentEffect + 1) % numEffects;
        FastLED.clear();

        // Очистка матрицы
        for (uint8_t y = 0; y < kMatrixY; y++) {
            for (uint8_t x = 0; x < kMatrixX; x++) {
                matrix[y][x] = -1;
            }
        }
    }
    
    static unsigned long lastBlinkTime = 0;
    static bool blinkState = false;

    if (currentMillis - lastBlinkTime >= 500) {
        lastBlinkTime = currentMillis;
        blinkState = !blinkState;
    }
    
    effects[currentEffect](leds, matrix, NUM_LEDS, 1, 255, 128, blinkState);

    FastLED.show();
}



