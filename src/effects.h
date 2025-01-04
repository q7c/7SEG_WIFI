#ifndef EFFECTS_H
#define EFFECTS_H

#include <FastLED.h>
#include "FastLED_RGBW.h"

constexpr uint8_t kMatrixY = 9;
constexpr uint8_t kMatrixX = 21;
extern const int ledMatrix[kMatrixY][kMatrixX];



void rainbowEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState);
void blueRedEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState);
void redEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState);
void fireEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState);
void auroraEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState);
void starryEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState);
void waveEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState);
void confettiEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState);
void breathingEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState);
void sparkleRainbowEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState);
void runningLightsEffect(CRGBW* leds, int matrix[kMatrixY][kMatrixX], int numLeds, int speed, int saturation, int brightness, bool blinkState);
void showSeparators(CRGBW* leds, uint8_t hue, uint8_t saturation, uint8_t brightness, bool blinkState);

#endif // EFFECTS_H