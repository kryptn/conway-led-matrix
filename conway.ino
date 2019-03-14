#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>

#define PIN 5

#define ROWS 16
#define COLS 16
#define PIXELS ROWS*COLS

// a new game spawns this many alive cells
#define NEW_GAME_POPULATION 20

// allow this many cycles before reset
#define REPEATS 8
#define MAX_REPEAT_CYCLE 4
#define CYCLES REPEATS * MAX_REPEAT_CYCLE

// higher allows infinite states to continue longer before intervention
#define ENTROPY_MAX 4000
#define MIN_LIFESPAN 500

// delay time between ticks in ms -- 1s = 1000ms
#define TICK_DELAY 100

// the lower limit for the random color, raise for more generally brighter colors
#define MIN_COLOR_VAL 40

// brightness for the led matrix
#define BRIGHTNESS 8

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, PIN, NEO_GRB + NEO_KHZ800);

bool cells[COLS][ROWS];
bool next[COLS][ROWS];

unsigned long int past[CYCLES];
byte past_ptr = 0;
int age = 0;


void randomState(int k = 40)
{
  for (int a = 0; a < k; a++) {
    int x = random(0, 255);
    int y = random(0, 255);
    cells[x % COLS][y % ROWS] = true;
  }
};


void tryIntervention() {
  int cmp = (age - MIN_LIFESPAN);
  if (cmp > ENTROPY_MAX) {
    cmp = ENTROPY_MAX;
  }

  if (cmp > random(0, ENTROPY_MAX*10)) {
    randomState(NEW_GAME_POPULATION);
    age = 0;
  }
}


bool detectStale(unsigned long int hash) {
  // add this hash to the previous states
  // if there's more than REPEATS occurances, return true
  past[past_ptr] = hash;
  past_ptr = (past_ptr + 1) % (CYCLES);
  int matches = 0;
  for (int i = 0; i < CYCLES; i++) {
    if (hash == past[i]) {
      matches++;
    }
  }

  return matches >= REPEATS;
}


unsigned long int hashCell(int x, int y) {
  return cells[x][y] ? (XYtoIndex(x, y) + 1) * x + 1 * y + 1 : 0;
}


unsigned long int hashState() {
  // returns a deterministic and unique enough result to identify a game state
  unsigned long int hash = 0;
  for (char x = 0; x < COLS; x++) {
    for (char y = 0; y < ROWS; y++) {
      hash = hash + hashCell(x, y);
    }
  }
  return hash;
}


class Color {
  public:
    byte red;
    byte green;
    byte blue;

    uint32_t Pack() {
      return ((uint32_t)red << 16) | ((uint32_t)green << 8) | blue;
    };
    Color(uint8_t r, uint8_t g, uint8_t b) : red(r), blue(b), green(g) {};
    Color(bool x) : red(random(MIN_COLOR_VAL, 255)),
                    blue(random(MIN_COLOR_VAL, 255)), 
                    green(random(MIN_COLOR_VAL, 255)) {};
    Color() : red(0), blue(0), green(0) {};
    void ShowOnStrip(int x, int y) {
      strip.setPixelColor(XYtoIndex(x, y), Pack());
    };
};

Color OFF = Color(0, 0, 0);
Color RED = Color(255, 0, 0);
Color GREEN = Color(0, 255, 0);
Color BLUE = Color(0, 0, 255);
Color DYING = Color(0, 0, 0);


int XYtoIndex(int x, int y)
{
  // convert an x/y coord to an index on a zigzag matrix
  x = x % COLS;
  y = y % ROWS;

  int modifier = x;
  if (y % 2 != 0)
  {
    modifier = 15 - x;
  }

  return y * 16 + modifier;
};


void checkPixels() {
  // iterates through all pixels to verify XYtoIndex and size config
  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLS; x++) {
      strip.setPixelColor(XYtoIndex(x, y), Color(true).Pack());
      strip.show();
      delay(1);
    }
  }

  strip.fill(OFF.Pack());
  strip.show();
}


void initGame(int x, int y) {
  cells[x][y] = false;
  next[x][y] = false;
  age = 0;

};


int aliveAround(int x, int y) {
  x = x + COLS;
  y = y + ROWS;
  int alive = 0;
  for (int xp = x - 1; xp <= x + 1; xp++) {
    for (int yp = y - 1; yp <= y + 1; yp++) {
      if (x != xp || y != yp) {
        int xm = xp % COLS;
        int ym = yp % ROWS;
        if (cells[xm][ym]) {
          alive += 1;
        }
      }
    }
  }
  return alive;
};


void iterState(void (*f)(int x, int y)) {
  for (int x = 0; x < COLS; x++) {
    for (int y = 0; y < ROWS; y++) {
      (*f)(x, y);
    }
  }
};


void resolveNextState(int x, int y) {
  // determines next state for a given cell
  int neighbors = aliveAround(x, y);
  next[x][y] = (neighbors == 3 || (cells[x][y] && neighbors == 2));
};


void emitLED(bool prev, bool current, int x, int y) {
  // given the previous and next states, emit the light
  if (!prev && !current) {
    OFF.ShowOnStrip(x, y);
  } else if (!prev && current) {
    Color(true).ShowOnStrip(x, y);
  } else if (prev && !current) {
    DYING.ShowOnStrip(x, y);
  };
};


void commitNextState(int x, int y) {
  // Moves this cell forward to the next state

  //convenience
  bool prev = cells[x][y];
  bool current = next[x][y];
  
  // actually change the state
  cells[x][y] = current;
  next[x][y] = false;

  emitLED(prev, current, x, y);
};

void checkInterventions() {
  // terminal state intervention
  unsigned long int hash = hashState();
  if (detectStale(hash)) {
    age = 0;
    iterState(initGame);
    randomState();
  } 

  // infinite state intervention
  tryIntervention();
}


void Tick() {
  // operation
  iterState(resolveNextState);
  iterState(commitNextState);

  checkInterventions();

  age = age + 1;

  // display
  strip.show();
}


void seed() {
  int r = 0;
  for ( int i = 0; i <= 10; i++)
    r += analogRead(A0);
  randomSeed(r);
}


void setup()
{
  seed();
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();
  
  checkPixels();

  iterState(initGame);
};


void loop()
{
  Tick();
  delay(TICK_DELAY);
}
