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

#define HORIZONTAL 1
#define VERTICAL 0

int board [BOARD_SIZE][BOARD_SIZE];
int board2 [BOARD_SIZE][BOARD_SIZE];


void setBoats(int board[BOARD_SIZE][BOARD_SIZE], int numBoats, int boatLength, int orientation, int xLowerIncrement, int xUpperIncrement, int yLowerIncrement, int yUpperIncrement){
  //colocar barcos
  for (int i = 0; i < numBoats; i++) {
    int x, y;
    boolean isWaterAround = true;
    do {
      x = random(0, BOARD_SIZE);
      y = random(0, BOARD_SIZE);
      // comprobar que todo lo que rodea a (x,y)
      // es agua, incluyendo la casilla (x,y).
      for (int xAux = x + xLowerIncrement; xAux <= x + xUpperIncrement; xAux++) {
        if (xAux < 0 || xAux >= BOARD_SIZE) continue;
        for (int yAux = y + yLowerIncrement; yAux <= y + yUpperIncrement; yAux++) {
          if (yAux < 0 || yAux >= BOARD_SIZE) continue;
          isWaterAround = board[xAux][yAux] == WATER;
          if (!isWaterAround) break;
        }
        if (!isWaterAround) break;
      }
      Serial.println(isWaterAround);
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
    // debug
    Serial.print(x);
    Serial.print(y);
    Serial.print(board[x][y]);
    Serial.println();
  }
}

void showBoard(int board [BOARD_SIZE][BOARD_SIZE]){
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      Serial.print( board[i][j] + " ");
    }
    Serial.println();
  }
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  // iniciar todas las casillas a agua
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      board[i][j] = WATER;
      board2[i][j] = WATER;
    }
  }
}

void loop() {
  
  showBoard(board);
  delay(1000);
  showBoard(board2);
}
