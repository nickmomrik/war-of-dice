/*******************
 * Hardware Config *
 *******************/

// In theory 7x4 and larger should work
// Only tested on 8x4
#define PIXEL_W    8
#define PIXEL_H    4

// What pin controls your Neopixels?
#define PIXEL_PIN  6

// Where are the player buttons?
#define P1_PIN     18
#define P2_PIN     19

 // Any unused pin - for random seed
#define UNUSED_PIN 20

/******************* 
 * Software Config *
 *******************/

// Colors of the dice
int p_rgb[][3] = {
  // Player 1 (red)
  {3, 0, 0},

  // Player 2 (blue)
  {0, 0, 3}
};

// Color of the line separating the two dice (green)
int sep_rgb[3] = {0, 3, 0};

// Color of the scoreboard (white)
int scr_rgb[3] = {3, 3, 3};

// Default to a typical die, but can be 2-9
// Use 2 to simulate coin flips!
int use_pips = 6;

// If true, access the Neopixels in reverse
// Then 1st becomes last and last becomes 1st
bool reverse_pixels = false;

/********************
 * End of Config    *
 *                  *
 * Don't edit below *
 ********************/

#include <Adafruit_NeoPixel.h>

// Which pips should be on/off for each 0-9 value
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

// Various parts of the game state
bool game_started;
bool round_started;
int current_die[2];
int score[2];
bool rolling[2];

// Prepare the neopixel grid
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_W * PIXEL_H, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Determines the neopixel by grid coordinates
int pixel(int x, int y) {
  int px = y * PIXEL_W + x;

  if (reverse_pixels) {
    px = PIXEL_W * PIXEL_H - px - 1;
  }
  
  return px;
}

// Determines the neopixel by the the player and the corresponding 3x3 (pips 0-8) die coordinates
int pixel_from_player_pip(int p, int pip) {
  int y = pip / 3;
  int x = pip % 3;

  // Player 2 is in the upper right corner, so offset the x
  if (2 == p) {
    x = x + PIXEL_W - 3;
  }

  return pixel(x, y);
}

// Turn the individual pixels of a die on/off according to the pip value
void print_die(int p, int num_pips = 9) {
  if (num_pips > 9) {
    num_pips = 9;
  } else if (num_pips < 0) {
    num_pips = 0;
  }

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

// Gets a random value to use for a die
int random_pips() {
  return random(1, use_pips + 1);
}

// Simple way to for the die to keep rolling after the player lets go
bool stopped_rolling() {
  return (1 == random(0, 5));
}

// Show the die
int display_random_die(int p) {
  int value = random_pips();
  print_die(p, value);

  return value;
}

// Are buttons being pressed?
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

// If a die hasn't stopped rolling either update the value or stop it and save the value
void maybe_roll(int p) {
  if (0 == current_die[p - 1]) {
    if (button_state(p) || !stopped_rolling()) {
      display_random_die(p);
    } else {
      current_die[p - 1] = display_random_die(p);
    }
  }
}

// Have both dice stoped?
bool round_rolls_done() {
  return (current_die[0] > 0 && current_die[1] > 0);
}

// Score needed to win the game
int max_score() {
  return PIXEL_W / 2;
}

// Returns the winning player number or 0 if nobody has won yet
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

// Show how many rounds a player has won
void display_score(int p, int score) {
  int y = PIXEL_H - 1;
  int x;
  int max_sc = max_score();

  for (int i = 0; i < max_sc; i++) {
    // Player 1 score goes from left to right and player 2 from right to left. Race to the middle.
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

// Determine the round winner, increase the score, and update the display
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

// If there is a winner, flash their score
void maybe_winner() {
  int win_p = winner();

  if (win_p) {
    int max_sc = max_score();
    for (int i = 0; i < max_sc; i++) {
      display_score(win_p, i);
      delay(100);
    }
    for (int i = max_sc; i >= 0; i--) {
      display_score(win_p, i);
      delay(100);
    }
  }
}

// Will flash the winning die or both if tied
void highlight_round_winner() {
  if (current_die[0] > current_die[1]) {
    flash_player_die(1);
  } else if (current_die[0] < current_die[1]) {
    flash_player_die(2);
  } else {
    flash_player_die(1);
    flash_player_die(2);
  }
}

// Flash a players current die on/off
void flash_player_die(int p) {
  print_die(p, 0);
  delay(100);
  print_die(p, current_die[p - 1]);
  delay(100);
}

// Prepare everything for a new game
void new_game() {
  game_started = true;
  round_started = false;
  current_die[0] = current_die[1] = score[0] = score[1] = 0;
  rolling[0] = rolling[1] = false;

  // Fill dice
  print_die(1, 9);
  print_die(2, 9);

  // Fill scores
  int max_sc = max_score();
  display_score(1, max_sc);
  display_score(2, max_sc);
}

// Prepare everything for a new round
void new_round() {
  round_started = true;
  rolling[0] = rolling[1] = false;
  current_die[0] = current_die[1] = 0;

  // Clear scores if it's the first round
  if (0 == score[0] && 0 == score[1]) {
    display_score(1, 0);
    display_score(2, 0);
  }
}

void setup() {
  // Make sure everything is really random
  randomSeed(analogRead(UNUSED_PIN));

  pinMode(P1_PIN, INPUT_PULLUP);
  pinMode(P2_PIN, INPUT_PULLUP);
  
  pixels.begin();

  // display a separator in the middle of the scoreboard
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
        // Continue updating the dice or determine if there is a round winner
        if (!round_rolls_done()) {
          maybe_roll(1);
          maybe_roll(2);
          delay(200);
        } else {
          update_score();
        }
      } else {
        // Ready to start a new round
        // Both players must press their button to start the rolls
        // Flash between solid and empty dice
        print_die(1, 0);
        print_die(2, 0);
        delay(100);
        print_die(1, 9);
        print_die(2, 9);
        delay(100);

        if (button_state(1) && button_state(2)) {
          rolling[0] = true;
          rolling[1] = true;
        }
      }
    } else if (button_state()) {
      new_round();
    } else if (current_die[0]) {
      highlight_round_winner();
    }
  } else {
    maybe_winner();

    // Hold any button while the winning score is flashing until a new game starts
    if (button_state()) {
      new_game();
    }
  }
}

