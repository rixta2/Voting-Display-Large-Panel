#include <FastLED.h>

// Constants
#define NUM_LEDS 30
#define BRIGHTNESS 128
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define PANEL_PIN1 4
#define PANEL_PIN2 25
#define PANEL_PIN3 26
#define PANEL_PIN4 27

// LED arrays for each panel
CRGB panel1[NUM_LEDS];
CRGB panel2[NUM_LEDS];
CRGB panel3[NUM_LEDS];
CRGB panel4[NUM_LEDS];

// 7-segment LED segment mapping
const int segmentMap[7][4] = {
    {1, 2, 3, 4},   // Top left
    {5, 6, 7, 8},   // Top
    {9, 10, 11, 12}, // Top Right
    {14, 15, 16, 17}, // Bottom Right
    {18, 19, 20, 21}, // Bottom
    {22, 23, 24, 25}, // Bottom Left
    {26, 27, 28, 29}  // Middle
};

// 7-segment number mapping
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

// Function to display a number on a 7-segment LED panel
void displayDigit(int digit, CRGB* strip) {
    fill_solid(strip, NUM_LEDS, CRGB::Black);  // Clear the panel

    if (digit >= 0 && digit <= 9) {
        for (int segment = 0; segment < 7; segment++) {
            if (numbers[digit][segment]) {
                for (int i = 0; i < 4; i++) {
                    int ledIndex = segmentMap[segment][i]; // Use the segmentMap index
                    if (ledIndex >= 1 && ledIndex <= NUM_LEDS) { // Adjust for 1-based indexing
                        strip[ledIndex] = CRGB::White;  // Set LED color
                    }
                }
            }
        }
    }
    FastLED.show();  // Update LEDs
}

void setup() {
    Serial.begin(115200);

    // Initialize LEDs
    FastLED.addLeds<LED_TYPE, PANEL_PIN1, COLOR_ORDER>(panel1, NUM_LEDS);
    FastLED.addLeds<LED_TYPE, PANEL_PIN2, COLOR_ORDER>(panel2, NUM_LEDS);
    FastLED.addLeds<LED_TYPE, PANEL_PIN3, COLOR_ORDER>(panel3, NUM_LEDS);
    FastLED.addLeds<LED_TYPE, PANEL_PIN4, COLOR_ORDER>(panel4, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
    for (int i = 0; i < 10; i++) {
        displayDigit(i, panel1);
        displayDigit(i, panel2);
        displayDigit(i, panel3);
        displayDigit(i, panel4);
        delay(3000);  // Wait 3 seconds before cycling to the next number
    }
}
