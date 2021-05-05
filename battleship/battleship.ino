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
#define NUM_CRUISE 3
#define NUM_DESTROYER 2
#define NUM_BATTLESHIP 1

#define PATROL_BOAT_LENGTH 1
#define CRUISE_LENGTH 2
#define DESTROYER_LENGTH 3
#define BATTLESHIP_LENGTH 4

#define HORIZONTAL 0
#define VERTICAL 1

int board [BOARD_SIZE][BOARD_SIZE];
int board2 [BOARD_SIZE][BOARD_SIZE];

boolean checkValidCoordinate(
  int board[BOARD_SIZE][BOARD_SIZE],
  int x, int y, int xLowerIncrement,
  int xUpperIncrement, int yLowerIncrement,
  int yUpperIncrement) {
  boolean isWaterAround = true;
  for (int xAux = x + xLowerIncrement; xAux <= x + xUpperIncrement; xAux++) {
    if (xAux < 0 || xAux >= BOARD_SIZE) continue;
    for (int yAux = y + yLowerIncrement; yAux <= y + yUpperIncrement; yAux++) {
      if (yAux < 0 || yAux >= BOARD_SIZE) continue;
      isWaterAround = board[xAux][yAux] == WATER;
      if (!isWaterAround) break;
    }
    if (!isWaterAround) break;
  }
  return isWaterAround;
}

void setBoats(int board[BOARD_SIZE][BOARD_SIZE], int numBoats, int boatLength, int orientation, int xLowerIncrement, int xUpperIncrement, int yLowerIncrement, int yUpperIncrement){
  //colocar barcos
  for (int i = 0; i < numBoats; i++) {
    int x, y, maxValidX, maxValidY;
    if (orientation == HORIZONTAL) {
      maxValidX = BOARD_SIZE - boatLength + 1;
      maxValidY = BOARD_SIZE;
    } else {
      maxValidX = BOARD_SIZE;
      maxValidY = BOARD_SIZE - boatLength + 1;
    }
    boolean isWaterAround = true;
    do {
      x = random(0, maxValidX);
      y = random(0, maxValidY);
      // comprobar que todo lo que rodea a (x,y)
      // es agua, incluyendo la casilla (x,y).
      isWaterAround = checkValidCoordinate(board, x, y, xLowerIncrement, xUpperIncrement, yLowerIncrement, yUpperIncrement);
    } while (!isWaterAround);
    //colocacion del barco
    //horizontal
    if(orientation == HORIZONTAL){
      for(int i = x; i < x + boatLength; i++){
        board[i][y] = BOAT;
      }
    }//vertical
    else{
      for(int i = y; i > y - boatLength; i--){
        board[x][i] = BOAT;
      }
    }
  }
}

void showBoard(int board [BOARD_SIZE][BOARD_SIZE]){
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      Serial.print(board[i][j]);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));
}

void loop() {
  // iniciar todas las casillas a agua
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      board[i][j] = WATER;
      board2[i][j] = WATER;
    }
  }
  setBoats(board, NUM_PATROL_BOATS, PATROL_BOAT_LENGTH, VERTICAL, -1, 1, -1, 1);
  setBoats(board2, NUM_DESTROYER, DESTROYER_LENGTH, HORIZONTAL, -1, 3, -1, 1);
  showBoard(board);
  Serial.println();
  showBoard(board2);
  Serial.println("-------------------");
  delay(5000);
}

/*
 0   1   2   3   4   5   6   7   8   9
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 0
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 1
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 2
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 3
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 4
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 5
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 6
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 7
[0] [0] [0] [0] [0] [1] [0] [0] [0] [0] 8
[0] [0] [0] [0] [0] [1] [0] [0] [0] [0] 9

boardsize=10  validY = BOARD_SIZE - BOAT_LENGTH
*/
