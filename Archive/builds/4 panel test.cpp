#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS    97     
#define POT_PIN     25     // Potentiometer pin
#define MAX_BRIGHTNESS 50 // Maximum brightness
#define MIN_BRIGHTNESS 10  // Minimum brightness to avoid complete darkness
#define DELAY_TIME  5000   // Delay between digit changes (5 seconds)


// Define LED pins for 4 panels
#define LED_PIN_1   21
#define LED_PIN_2   22
#define LED_PIN_3   32
#define LED_PIN_4   33

CRGB ledsPanel1[NUM_LEDS];
CRGB ledsPanel2[NUM_LEDS];
CRGB ledsPanel3[NUM_LEDS];
CRGB ledsPanel4[NUM_LEDS];

const int segmentLengths[7] = {14, 13, 14, 14, 14, 15, 13};

// 7-segment LED mapping (excluding filler LEDs 0 and 13)
const int segmentMap[7][15] = {
  { 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54 },   // Top left 1
  { 36, 37, 38, 39, 40, 55, 56, 57, 58, 59, 60, 61, 62 },   // Top 2
  { 30, 31, 32, 33, 34, 35, 63, 64, 65, 66, 67, 68, 69, 70 },   // Top Right 3
  { 24, 25, 26, 27, 28, 29, 71, 72, 73, 74, 75, 76, 77, 78 },   // Bottom Right 4
  { 19, 20, 21, 22, 23, 79, 80, 81, 82, 83, 84, 85, 86, 87 },   // Bottom 5
  { 13, 14, 15, 16, 17, 18, 88, 89, 90, 91, 92, 93, 94, 95, 96 },      // Bottom Left 6 
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }            // Middle 7
};

// Digit-to-segment mapping (1 = ON, 0 = OFF)
const byte numbers[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 0, 1, 1, 0, 0, 0}, // 1
  {0, 1, 1, 0, 1, 1, 1}, // 2
  {0, 1, 1, 1, 1, 0, 1}, // 3
  {1, 0, 1, 1, 0, 0, 1}, // 4
  {1, 1, 0, 1, 1, 0, 1}, // 5
  {1, 1, 0, 1, 1, 1, 1}, // 6
  {0, 1, 1, 1, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 1, 0, 1}  // 9
};

// Function to display a digit on a single panel
void displayDigitOnPanel(CRGB* leds, int digit) {
  fill_solid(leds, NUM_LEDS, CRGB::Black);

  for (int i = 0; i < 7; i++) {
    if (numbers[digit][i] == 1) {
      for (int j = 0; j < segmentLengths[i]; j++) {
        if (segmentMap[i][j] < NUM_LEDS) {
          leds[segmentMap[i][j]] = CRGB::Red; // Turn on the LED
        }
      }
    }
  }

  FastLED.show();
}

// Function to display a digit on all panels
void displayDigit(int digit) {
  displayDigitOnPanel(ledsPanel1, digit);
  displayDigitOnPanel(ledsPanel2, digit);
  displayDigitOnPanel(ledsPanel3, digit);
  displayDigitOnPanel(ledsPanel4, digit);
}

void setup() {
  // Initialize FastLED for all panels
  FastLED.addLeds<WS2812B, LED_PIN_1, RGB>(ledsPanel1, NUM_LEDS);
  FastLED.addLeds<WS2812B, LED_PIN_2, RGB>(ledsPanel2, NUM_LEDS);
  FastLED.addLeds<WS2812B, LED_PIN_3, RGB>(ledsPanel3, NUM_LEDS);
  FastLED.addLeds<WS2812B, LED_PIN_4, RGB>(ledsPanel4, NUM_LEDS);

  //FastLED.setBrightness(255);
  //FastLED.setMaxPowerInMilliWatts(MAX_POWER_MILLIAMPS);

  // Set all LEDs to off initially
  fill_solid(ledsPanel1, NUM_LEDS, CRGB::Black);
  fill_solid(ledsPanel2, NUM_LEDS, CRGB::Black);
  fill_solid(ledsPanel3, NUM_LEDS, CRGB::Black);
  fill_solid(ledsPanel4, NUM_LEDS, CRGB::Black);

  FastLED.show();
}

void loop() {
  for (int digit = 0; digit <= 9; digit++) {
    displayDigit(digit);
    delay(DELAY_TIME);
  }
}
