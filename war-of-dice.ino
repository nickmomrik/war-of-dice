#include <Adafruit_NeoPixel.h>

// Must be at least 8x4
#define PIXEL_W    8
#define PIXEL_H    4

#define PIXEL_PIN  6
#define UNUSED_PIN 8  // For random seed
#define P1_PIN     18
#define P2_PIN     19

int p_rgb[][3] = {
  {32, 0, 0},
  {0, 0, 16}
};
int sep_rgb[3] = {0, 16, 0};
int scr_rgb[3] = {32, 32, 32};
int use_pips = 6; // Can use 1-9

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

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_W * PIXEL_H, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

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
  if (num_pips > 9) {
    num_pips = 9;
  } else if (num_pips < 0) {
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
  randomSeed(analogRead(UNUSED_PIN));

  pinMode(P1_PIN, INPUT_PULLUP);
  pinMode(P2_PIN, INPUT_PULLUP);
  
  pixels.begin();
  //Serial.begin(9600);

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
  if (LOW == digitalRead(P1_PIN)) {
    print_die(1, random_pips());
  }
  if (LOW == digitalRead(P2_PIN)) {
    print_die(2, random_pips());
  }

  delay(300);
}

int random_pips() {
  return random(1, use_pips + 1);
}

