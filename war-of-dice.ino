#include <Adafruit_NeoPixel.h>

// Must be at least 8x4
#define PIXEL_W 8
#define PIXEL_H 4
#define PIN     6

int p_rgb[][3] = {
  {32, 0, 0},
  {0, 0, 16}
};
int sep_rgb[3] = {0, 16, 0};
int scr_rgb[3] = {32, 32, 32};

// Each pip is on or off
// Only going to use 6, but give blank plus 1-9 pip die in case it's ever needed
int pips[][9] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 1, 0, 0},
  {0, 0, 1, 0, 1, 0, 1, 0, 0},
  {1, 0, 1, 0, 0, 0, 1, 0, 1},
  {1, 0, 1, 0, 1, 0, 1, 0, 1},
  {1, 0, 1, 1, 0, 1, 1, 0, 1},
  {1, 0, 1, 1, 1, 1, 1, 0, 1},
  {1, 1, 1, 1, 0, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1}
};

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_W * PIXEL_H, PIN, NEO_GRB + NEO_KHZ800);

int pixel(int x, int y) {
  return y * PIXEL_W + x;
}

int pixel_from_player_pip(int p, int pip) {
  int y = pip / 3;
  int x = pip % 3;

  if (2 == p) {
    x = x + PIXEL_W - 3;
  }

  return pixel(x, y);
}

void print_die(int p, int num_pips = 9) {
  switch (num_pips) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 9:
      break;
    default:
      num_pips = 0;
  }

  // Light up the pips
  for (int i = 0; i < 9; i++) {
    int px = pixel_from_player_pip(p, i);

    if (pips[num_pips][i]) {
      pixels.setPixelColor(px, pixels.Color(p_rgb[p-1][0], p_rgb[p-1][1], p_rgb[p-1][2]));
    } else {
      pixels.setPixelColor(px, pixels.Color(0, 0, 0));
    }
    pixels.show();
  }
}

void setup() {
  pixels.begin();
  Serial.begin(9600);

  // display a separator in the middle
  int sep_lines =  (PIXEL_W - 6) % 2 ? 1 : 2;
  for (int i = 0; i < sep_lines; i++) {
    int x = PIXEL_W / 2 - i;
    for (int y = 0; y < PIXEL_H - 1; y++) {
          pixels.setPixelColor(pixel(x, y), pixels.Color(sep_rgb[0], sep_rgb[1], sep_rgb[2]));
          pixels.show();          
    }
  }

  print_die(1, 9);
  print_die(2, 9);
}

void loop() {
  for (int i = 0; i < 10; i++) {
    print_die(1, i);
    print_die(2, i);
    delay(250);
  }
}
