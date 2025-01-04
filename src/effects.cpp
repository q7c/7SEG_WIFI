#include "effects.h"

// Функция rainbowEffect отвечает за создание радужного эффекта на светодиодах.
// Она принимает массив светодиодов, матрицу, количество светодиодов, скорость изменения цвета, насыщенность и яркость.
void rainbowEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState) {
    static uint8_t hue = 0;
    
    for (uint8_t y = 0; y < kMatrixY; y++) {
        for (uint8_t x = 0; x < kMatrixX; x++) {
            int ledIndex = ledMatrix[y][x];
            if (ledIndex != -1 && matrix[y][x] >= 0) {
                uint8_t pixelHue = hue + (ledIndex * 256 / numLeds);
                leds[ledIndex] = CHSV(pixelHue, saturation, brightness);
            }
        }
    }
    
    FastLED.show();
    
    hue += speed;
    if (hue >= 256) {
        hue = 0;
    }
    
    showSeparators(leds, hue, saturation, brightness, blinkState);
}

void blueRedEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState) {
    static uint8_t hue = 0;
    
    for (uint8_t y = 0; y < kMatrixY; y++) {
        for (uint8_t x = 0; x < kMatrixX; x++) {
            int ledIndex = ledMatrix[y][x];
            if (ledIndex != -1 && matrix[y][x] >= 0) {
                uint8_t pixelHue = hue + (ledIndex * 256 / numLeds);
                if (pixelHue < 128) {
                    leds[ledIndex] = CHSV(160, saturation, brightness); // Синий цвет
                } else {
                    leds[ledIndex] = CHSV(0, saturation, brightness); // Красный цвет
                }
            }
        }
    }
    
    FastLED.show();
    
    hue += speed;
    if (hue >= 256) {
        hue = 0;
    }
    
    showSeparators(leds, hue, saturation, brightness, blinkState);
}

void redEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState) {
    for (uint8_t y = 0; y < kMatrixY; y++) {
        for (uint8_t x = 0; x < kMatrixX; x++) {
            int ledIndex = ledMatrix[y][x];
            if (ledIndex != -1 && matrix[y][x] >= 0) {
                leds[ledIndex] = CHSV(0, saturation, brightness); // Красный цвет
            }
        }
    }
    
    FastLED.show();
    
    showSeparators(leds, 0, saturation, brightness, blinkState);
}

void fireEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState) {
    static uint8_t heat[kMatrixY][kMatrixX] = {0};

    // Шаг 1: Охлаждение
    for (uint8_t y = 0; y < kMatrixY; y++) {
        for (uint8_t x = 0; x < kMatrixX; x++) {
            heat[y][x] = qsub8(heat[y][x], random8(0, 255 / kMatrixY + 2));
        }
    }

    // Шаг 2: Нагрев
    for (uint8_t x = 0; x < kMatrixX; x++) {
        if (random8() < 160) {
            heat[kMatrixY - 1][x] = qadd8(heat[kMatrixY - 1][x], random8(192, 255));
        }
    }

    // Шаг 3: Распространение огня
    for (uint8_t y = 0; y < kMatrixY - 1; y++) {
        for (uint8_t x = 0; x < kMatrixX; x++) {
            heat[y][x] = (heat[y + 1][x] + heat[y][x] + heat[y][x]) / 3;
        }
    }

    // Шаг 4: Преобразование значений нагрева в цвета
    for (uint8_t y = 0; y < kMatrixY; y++) {
        for (uint8_t x = 0; x < kMatrixX; x++) {
            int ledIndex = ledMatrix[y][x];
            if (ledIndex != -1 && matrix[y][x] >= 0) {
                uint8_t colorIndex = heat[y][x];
                uint8_t hue = map(colorIndex, 0, 255, 0, 30);
                uint8_t saturation = 255;
                uint8_t value = map(colorIndex, 0, 255, brightness / 2, brightness);
                leds[ledIndex] = CHSV(hue, saturation, value);
            }
        }
    }

    FastLED.show();
    
    showSeparators(leds, 0, saturation, brightness, blinkState);
}

void auroraEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState) {
    static uint8_t hue = 0;
    static uint8_t wavePos = 0;
    
    for (uint8_t y = 0; y < kMatrixY; y++) {
        for (uint8_t x = 0; x < kMatrixX; x++) {
            int ledIndex = ledMatrix[y][x];
            if (ledIndex != -1 && matrix[y][x] >= 0) {
                uint8_t pixelHue = hue + sin8(wavePos + x * 2) / 2;
                leds[ledIndex] = CHSV(pixelHue, saturation, brightness);
            }
        }
    }
    
    FastLED.show();
    
    hue += speed;
    wavePos += speed / 2;
    
    showSeparators(leds, hue, saturation, brightness, blinkState);
}

void starryEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState) {
    static uint8_t hue = 160; // Темно-синий цвет
    static uint8_t starPositions[kMatrixY][kMatrixX] = {0};
    
    // Шаг 1: Установка фонового цвета
    for (uint8_t y = 0; y < kMatrixY; y++) {
        for (uint8_t x = 0; x < kMatrixX; x++) {
            int ledIndex = ledMatrix[y][x];
            if (ledIndex != -1 && matrix[y][x] >= 0) {
                leds[ledIndex] = CHSV(hue, saturation, 32); // Темно-синий фон с низкой яркостью
            }
        }
    }
    
    // Шаг 2: Обновление позиций звезд
    for (uint8_t y = 0; y < kMatrixY; y++) {
        for (uint8_t x = 0; x < kMatrixX; x++) {
            if (random8() < 4) { // Уменьшаем вероятность появления новых звезд
                starPositions[y][x] = random8(200, 255); // Увеличиваем диапазон яркости звезд
            } else {
                starPositions[y][x] = starPositions[y][x] * 15 / 16; // Замедляем затухание звезд
            }
        }
    }
    
    // Шаг 3: Отображение звезд
    for (uint8_t y = 0; y < kMatrixY; y++) {
        for (uint8_t x = 0; x < kMatrixX; x++) {
            int ledIndex = ledMatrix[y][x];
            if (ledIndex != -1 && matrix[y][x] >= 0) {
                if (starPositions[y][x] > 0) {
                    leds[ledIndex] = CHSV(0, 0, starPositions[y][x]); // Белые звезды
                }
            }
        }
    }
    
    FastLED.show();
    
    showSeparators(leds, 160, saturation, brightness, blinkState);
}

void waveEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState) {
    static uint8_t hue = 0;
    static int wavePos = 0;
    
    for (uint8_t y = 0; y < kMatrixY; y++) {
        for (uint8_t x = 0; x < kMatrixX; x++) {
            int ledIndex = ledMatrix[y][x];
            if (ledIndex != -1 && matrix[y][x] >= 0) {
                uint8_t pixelHue = hue + sin8(wavePos + y * 16) / 2;
                leds[ledIndex] = CHSV(pixelHue, saturation, brightness);
            }
        }
    }
    
    FastLED.show();
    
    hue += speed;
    wavePos += speed;
    
    showSeparators(leds, hue, saturation, brightness, blinkState);
}

void confettiEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState) {
    static uint8_t confettiPositions[kMatrixY][kMatrixX] = {0};
    static uint8_t confettiColors[kMatrixY][kMatrixX] = {0};
    static uint8_t confettiFalling[kMatrixY][kMatrixX] = {0};
    static uint8_t confettiSpeeds[kMatrixY][kMatrixX] = {0};
    static unsigned long lastUpdate = 0;
    
    unsigned long currentTime = millis();
    if (currentTime - lastUpdate >= speed) {
        lastUpdate = currentTime;
        
        // Шаг 1: Обновление позиций конфетти
        for (uint8_t y = kMatrixY - 1; y > 0; y--) {
            for (uint8_t x = 0; x < kMatrixX; x++) {
                if (confettiFalling[y - 1][x]) {
                    confettiPositions[y][x] = confettiPositions[y - 1][x];
                    confettiColors[y][x] = confettiColors[y - 1][x];
                    confettiSpeeds[y][x] = confettiSpeeds[y - 1][x];
                    confettiFalling[y][x] = 1;
                    confettiFalling[y - 1][x] = 0;
                }
            }
        }
        
        // Шаг 2: Генерация нового конфетти
        for (uint8_t x = 0; x < kMatrixX; x++) {
            if (random8() < 8) {
                confettiPositions[0][x] = random8(64, 255);
                confettiColors[0][x] = random8();
                confettiSpeeds[0][x] = random8(1, 4);
                confettiFalling[0][x] = 1;
            }
        }
    }
    
    // Шаг 3: Отображение конфетти
    for (uint8_t y = 0; y < kMatrixY; y++) {
        for (uint8_t x = 0; x < kMatrixX; x++) {
            int ledIndex = ledMatrix[y][x];
            if (ledIndex != -1 && matrix[y][x] >= 0) {
                if (confettiPositions[y][x] > 0) {
                    leds[ledIndex] = CHSV(confettiColors[y][x], saturation, confettiPositions[y][x]);
                    uint8_t fadeAmount = confettiSpeeds[y][x];
                    confettiPositions[y][x] = (confettiPositions[y][x] > fadeAmount) ? confettiPositions[y][x] - fadeAmount : 0;
                } else {
                    leds[ledIndex] = CRGBW(0, 0, 0, 0);
                }
            }
        }
    }
    
    FastLED.show();
    
    showSeparators(leds, random8(), saturation, brightness, blinkState);
}

void breathingEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState) {
    static uint8_t hue = 0;
    static uint8_t breathingBrightness = 0;
    static uint8_t direction = 1;
    
    for (uint8_t y = 0; y < kMatrixY; y++) {
        for (uint8_t x = 0; x < kMatrixX; x++) {
            int ledIndex = ledMatrix[y][x];
            if (ledIndex != -1 && matrix[y][x] >= 0) {
                leds[ledIndex] = CHSV(hue, saturation, breathingBrightness);
            }
        }
    }
    
    FastLED.show();
    
    hue += speed;
    breathingBrightness += direction * speed;
    
    if (breathingBrightness <= 0 || breathingBrightness >= brightness) {
        direction *= -1;
    }
    
    showSeparators(leds, hue, saturation, breathingBrightness, blinkState);
}

void sparkleRainbowEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState) {
    static uint8_t hue = 0;
    
    for (uint8_t y = 0; y < kMatrixY; y++) {
        for (uint8_t x = 0; x < kMatrixX; x++) {
            int ledIndex = ledMatrix[y][x];
            if (ledIndex != -1 && matrix[y][x] >= 0) {
                if (random8() < 16) {
                    leds[ledIndex] = CHSV(hue + random8(), saturation, brightness);
                } else {
                    leds[ledIndex] = CHSV(hue, saturation, brightness);
                }
            }
        }
    }
    
    FastLED.show();
    
    hue += speed;
    
    showSeparators(leds, hue, saturation, brightness, blinkState);
}

void runningLightsEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState) {
    static uint8_t hue = 0;
    static uint8_t position = 0;
    
    for (uint8_t y = 0; y < kMatrixY; y++) {
        for (uint8_t x = 0; x < kMatrixX; x++) {
            int ledIndex = ledMatrix[y][x];
            if (ledIndex != -1 && matrix[y][x] >= 0) {
                if (x == position) {
                    leds[ledIndex] = CHSV(hue, saturation, brightness);
                } else {
                    leds[ledIndex] = CRGBW(0, 0, 0, 0);
                }
            }
        }
    }
    
    FastLED.show();
    
    hue += speed;
    position = (position + 1) % kMatrixX;
    
    showSeparators(leds, hue, saturation, brightness, blinkState);
}

void showSeparators(CRGBW* leds, uint8_t hue, uint8_t saturation, uint8_t brightness, bool blinkState) {
    int separatorIndex1 = ledMatrix[2][10];
    int separatorIndex2 = ledMatrix[6][10];
    
    if (blinkState) {
        leds[separatorIndex1] = CHSV(hue, saturation, brightness);
        leds[separatorIndex2] = CHSV(hue, saturation, brightness);
    } else {
        leds[separatorIndex1] = CRGBW(0, 0, 0, 0);
        leds[separatorIndex2] = CRGBW(0, 0, 0, 0);
    }
}

