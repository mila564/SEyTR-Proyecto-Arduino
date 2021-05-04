/*
#include <Adafruit_TFTLCD.h>
#include <registers.h>
#include <pin_magic.h>
*/
#define XM A2
#define XP 6
#define YP A1
#define YM 7

#define TS_MINX 905
#define TS_MAXX 130
#define TS_MINY 75
#define TS_MAXY 930

//------------------------

#define WATER 0
#define BOAT 1
#define MISS 2
#define IMPACT 3
#define SUNK 4

#define BOARD_SIZE 10

#define NUM_PATROL_BOATS 4
#define NUM_SUBMARINE 3
#define NUM_DESTROYER 2
#define NUM_BATTLESHIP 1
/*
typedef struct {
  int x;
  int y;
} TCoordinate;

typedef struct {
  int[BOARD_SIZE][BOARD_SIZE] board;
} TBoard;

typedef struct {
  TCoordinate square[4]
} TBattleship;
typedef struct {
  TCoordinate square[3];
} TDestroyer;
typedef struct {
  TCoordinate square[2];
} TSubmarine;
typedef struct {
  TCoordinate square;
} TPatrolBoat;

typedef struct {
  TBattleship battleship;
  TDestroyer destroyer[2];
  TSubmarine submarine[3];
  TPatrolBoat patrolBoat[4];
} TBoatList;

typedef struct {
  int boatsAfloat;
  TBoard board;
  TBoatList boatList;
} TPlayer;

TPlayer player[2];
*/
void setup() {
  int board[BOARD_SIZE][BOARD_SIZE];
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  // iniciar todas las casillas a agua
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      board[i][j] = WATER;
    }
  }
/*
  board[2][3] = WATER;
  board[2][4] = WATER;
  board[2][5] = WATER;
  board[3][3] = WATER;
  board[3][4] = WATER;
  board[3][5] = WATER;
  board[4][3] = WATER;
  board[4][4] = WATER;
  board[4][5] = WATER;
*/
  // colocar lanchas
  for (int i = 0; i < NUM_PATROL_BOATS; i++) {
    int x, y;
    boolean isWaterAround = true;
    do {
      x = random(0, BOARD_SIZE);
      y = random(0, BOARD_SIZE);
      // comprobar que todo lo que rodea a (x,y)
      // es agua, incluyendo la casilla (x,y).
      for (int xAux = x - 1; xAux <= x + 1; xAux++) {
        if (xAux < 0 || xAux >= BOARD_SIZE) continue;
        for (int yAux = y - 1; yAux <= y + 1; yAux++) {
          if (yAux < 0 || yAux >= BOARD_SIZE) continue;
          isWaterAround = board[xAux][yAux] == WATER;
          if (!isWaterAround) break;
        }
        if (!isWaterAround) break;
      }
      Serial.println(isWaterAround);
    } while (!isWaterAround);
    board[x][y] = BOAT;
    // debug
    Serial.print(x);
    Serial.print(y);
    Serial.print(board[x][y]);
    Serial.println();
  }
}

void loop() {
  
}
