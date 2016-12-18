#include <Adafruit_NeoPixel.h>

// Must be at least 8x4
#define PIXEL_W 8
#define PIXEL_H 4
#define PIN     6

int p1_rgb[3] = {16, 0, 0};
int p2_rgb[3] = {0, 0, 16};
int sep_rgb[3] = {0, 16, 0};
int scr_rgb[3] = {16, 16, 16};

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_W * PIXEL_H, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin();

  // display a separator in the middle
  int sep_lines =  (PIXEL_W - 6) % 2 ? 1 : 2;
  for (int i = 0; i < sep_lines; i++) {
    int x = PIXEL_W / 2 - i;
    for (int y = 0; y < PIXEL_H - 1; y++) {
          pixels.setPixelColor(pixel_number(x, y), pixels.Color(sep_rgb[0], sep_rgb[1], sep_rgb[2]));
          pixels.show();          
    }
  }
}

void loop() {

}

int pixel_number(int x, int y) {
  return y * PIXEL_W + x;
}

