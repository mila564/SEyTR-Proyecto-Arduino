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
  int board[BOARD_SIZE][BOARD_SIZE], int row, int column, int rowLowerIncrement,
  int rowUpperIncrement, int columnLowerIncrement, int columnUpperIncrement) {
  boolean isWaterAround = true;
  for (int rowAux = row + rowLowerIncrement; rowAux <= row + rowUpperIncrement; rowAux++) {
    if (rowAux < 0 || rowAux >= BOARD_SIZE) continue;
    for (int columnAux = column + columnLowerIncrement; columnAux <= column + columnUpperIncrement; columnAux++) {
      if (columnAux < 0 || columnAux >= BOARD_SIZE) continue;
      isWaterAround = board[rowAux][columnAux] == WATER;
      if (!isWaterAround) break;
    }
    if (!isWaterAround) break;
  }
  return isWaterAround;
}

void setBoats(int board[BOARD_SIZE][BOARD_SIZE], int numBoats, int boatLength, int orientation,
  int rowLowerIncrement, int rowUpperIncrement, int columnLowerIncrement, int columnUpperIncrement){
  //colocar barcos
  for (int i = 0; i < numBoats; i++) {
    int row, column, maxValidRow, maxValidColumn;
    if (orientation == HORIZONTAL) {
      maxValidRow = BOARD_SIZE;
      maxValidColumn = BOARD_SIZE - boatLength + 1;
    } else {
      maxValidRow = BOARD_SIZE - boatLength + 1;
      maxValidColumn = BOARD_SIZE;
    }
    boolean isWaterAround = true;
    do {
      row = random(0, maxValidRow);
      column = random(0, maxValidColumn);
      isWaterAround = checkValidCoordinate(board, row, column, rowLowerIncrement, rowUpperIncrement,
        columnLowerIncrement, columnUpperIncrement);
    } while (!isWaterAround);
    if(orientation == HORIZONTAL){
      for(int i = column; i < column + boatLength; i++){
        board[row][i] = BOAT;
      }
    } else { // vertical
      for(int i = row; i < row + boatLength; i++){
        board[i][column] = BOAT;
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
  setBoats(board, NUM_DESTROYER, DESTROYER_LENGTH, HORIZONTAL, -1, 1, -1, 3);
  setBoats(board, 2, BATTLESHIP_LENGTH, VERTICAL, -1, 4, -1, 1);
  showBoard(board);
  //Serial.println();
  //showBoard(board2);
  Serial.println("-------------------");
  delay(5000);
}

/*
 columns ---->
 0   1   2   3   4   5   6   7   8   9
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 0
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 1
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 2
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 3
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 4
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 5
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 6   ^
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 7   |
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 8   |
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 9 rows

board[row][columns]
*/
