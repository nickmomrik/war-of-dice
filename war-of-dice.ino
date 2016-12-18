// Hardware Config
#define PIXEL_W    8  // Grid must be at least 8x4 to work properly
#define PIXEL_H    4
#define PIXEL_PIN  6
#define UNUSED_PIN 20 // For random seed
#define P1_PIN     18
#define P2_PIN     19

// Software Config
int p_rgb[][3] = {
  {32, 0, 0},
  {0, 0, 16}
};
int sep_rgb[3] = {0, 16, 0};
int scr_rgb[3] = {32, 32, 32};
int use_pips = 6; // Can use 1-9

/********************
 * End of Config    *
 * Don't edit below *
 ********************/

#include <Adafruit_NeoPixel.h>

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

// Always start with a new game
bool game_started;
bool round_started;
int current_die[2];
int score[2];
bool rolling[2];

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

int random_pips() {
  return random(1, use_pips + 1);
}

bool stopped_rolling() {
  return (1 == random(0, 5));
}

int random_die(int p) {
  int value = random_pips();
  print_die(p, value);

  return value;
}

bool button_state(int p = 0) {
  bool p1_state = (LOW == digitalRead(P1_PIN));
  bool p2_state = (LOW == digitalRead(P2_PIN));

  if (1 == p) {
    return p1_state;
  } else if (2 == p) {
    return p2_state;
  } else {
    return p1_state || p2_state;
  }
}

void maybe_roll(int p) {
  if (0 == current_die[p - 1]) {
    if (button_state(p) || !stopped_rolling()) {
      random_die(p);
    } else {
      current_die[p - 1] = random_die(p);
    }
  }
}

bool round_rolls_done() {
  return (current_die[0] > 0 && current_die[1] > 0);
}

int max_score() {
  return PIXEL_W / 2;
}

int winner() {
  int max_sc = max_score();
  
  if (score[0] == max_sc) {
    return 1;
  } else if (score[1] == max_sc) {
    return 2;
  } else {
    return 0;
  }
}

void display_score(int p, int score) {
  int y = PIXEL_H - 1;
  int x;
  int max_sc = max_score();

  for (int i = 0; i < max_sc; i++) {
    if (1 == p) {
      x = i;      
    } else if (2 == p) {
      x = PIXEL_W - i - 1;
    }

    if (i < score) {
      pixels.setPixelColor(pixel(x, y), pixels.Color(scr_rgb[0], scr_rgb[1], scr_rgb[2]));
    } else {
      pixels.setPixelColor(pixel(x, y), pixels.Color(0, 0, 0));
    }
    pixels.show();
  } 
}

void update_score() {
  if (current_die[0] > current_die[1]) {
    score[0]++;
  } else if (current_die[0] < current_die[1]) {
    score[1]++;
  }

  display_score(1, score[0]);
  display_score(2, score[1]);

  if (winner()) {
    game_started = false;
  }

  round_started = false;
}

void maybe_winner() {
  int win_p = winner();

  if (win_p) {
    int max_sc = max_score();
    for (int i = 0; i < max_sc; i++) {
      display_score(win_p, i);
      delay(200);
    }
    for (int i = max_sc; i >= 0; i--) {
      display_score(win_p, i);
      delay(200);
    }
  }
}

void new_game() {
  // Reset stat variables
  game_started = true;
  round_started = false;
  current_die[0] = current_die[1] = score[0] = score[1] = 0;
  rolling[0] = rolling[1] = false;

  // Fill dice
  print_die(1, 9);
  print_die(2, 9);

  // Clear scores
  display_score(1, 0);
  display_score(2, 0);
}

void new_round() {
  round_started = true;
  rolling[0] = rolling[1] = false;
  current_die[0] = current_die[1] = 0;
}

void setup() {
  randomSeed(analogRead(UNUSED_PIN));

  pinMode(P1_PIN, INPUT_PULLUP);
  pinMode(P2_PIN, INPUT_PULLUP);
  
  pixels.begin();

  // display a separator in the middle
  int sep_lines =  (PIXEL_W - 6) % 2 ? 1 : 2;
  for (int i = 0; i < sep_lines; i++) {
    int x = PIXEL_W / 2 - i;
    for (int y = 0; y < PIXEL_H - 1; y++) {
          pixels.setPixelColor(pixel(x, y), pixels.Color(sep_rgb[0], sep_rgb[1], sep_rgb[2]));
          pixels.show();          
    }
  }

  new_game();
}

void loop() {
  if (game_started) {
    if (round_started) {
      if (rolling[0] && rolling[1]) {
        if (!round_rolls_done()) {
          maybe_roll(1);
          maybe_roll(2);
        } else {
          update_score();
        }
      
        delay(300);
      } else {
        print_die(1, 0);
        print_die(2, 0);
        delay(100);
        print_die(1, 9);
        print_die(2, 9);
        delay(100);

        if (button_state(1)) {
          rolling[0] = true;
        }
        if (button_state(2)) {
          rolling[1] = true;
        }
      }
    } else if (button_state()) {
      new_round();
    }
  } else {
    maybe_winner();

    if (button_state()) {
      new_game();
    }
  }
}

