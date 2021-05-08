#include <MCUFRIEND_kbv.h>
#include <Adafruit_TFTLCD.h>  //Librería para visualización
#include <TouchScreen.h> // Librería del panel táctil

#define XM A2
#define XP 6
#define YP A1
#define YM 7

#define TS_MINX 905
#define TS_MAXX 130
#define TS_MINY 75
#define TS_MAXY 930

// Touch screen pins
#define LCD_CS  A3
#define LCD_CD  A2
#define LCD_WR  A1
#define LCD_RD  A0
#define LCD_RESET  A4

#define TS_MIN_X_Y 0
#define TS_MAX_X_Y 1023

//------------------------

#define CONTROLLER 0x9486

// States

#define WATER 0
#define BOAT 1
#define MISS 2
#define IMPACT 3
#define SUNK 4

//Colors

#define BLUE 0x186EDE //MISS
#define RED 0xF800 //IMPACT 0xED2415
#define BLACK 0x000000 //SUNK
#define WHITE 0xFFFFFF// BACKGROUND

//#define BOARD_SIZE 10

#define BOARD_WIDTH 12
#define BOARD_HEIGHT 8

//Pressure

#define MINPRESSURE 120
#define MAXPRESSURE 220

int BOXSIZE = 40;

#define NUM_PATROL_BOATS 3
#define NUM_CRUISE 2
#define NUM_DESTROYER 1
#define NUM_BATTLESHIP 1

#define PATROL_BOAT_LENGTH 1
#define CRUISE_LENGTH 2
#define DESTROYER_LENGTH 3
#define BATTLESHIP_LENGTH 4

#define HORIZONTAL 0
#define VERTICAL 1

//int board [BOARD_SIZE][BOARD_SIZE];
//int board2 [BOARD_SIZE][BOARD_SIZE];

int board [BOARD_HEIGHT][BOARD_WIDTH];
int board2 [BOARD_HEIGHT][BOARD_WIDTH];

MCUFRIEND_kbv screenDisplay(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET); 
TouchScreen touchPanel = TouchScreen(XP, YP, XM, YM, 300); 

void drawGrid(MCUFRIEND_kbv screenDisplay, int BOXSIZE, int board [BOARD_HEIGHT][BOARD_WIDTH]){
  screenDisplay.fillScreen(WHITE);
  for (int i = 0; i < BOARD_HEIGHT; i++){
    for (int j = 0; j < BOARD_WIDTH; j++){
      screenDisplay.drawRect(j*BOXSIZE, i*BOXSIZE, BOXSIZE, BOXSIZE, BLACK);
      switch(board[i][j]){
        case MISS:
          screenDisplay.fillRect(j*BOXSIZE, i*BOXSIZE, BOXSIZE, BOXSIZE, BLUE);
          break;
        case IMPACT:
          screenDisplay.fillRect(j*BOXSIZE, i*BOXSIZE, BOXSIZE, BOXSIZE, RED);
          break;
        case SUNK:
          screenDisplay.fillRect(j*BOXSIZE, i*BOXSIZE, BOXSIZE, BOXSIZE, BLACK);
        default: ;  
      }
    }
  }
}

void shoot(int board [BOARD_HEIGHT][BOARD_WIDTH], TouchScreen touchPanel, MCUFRIEND_kbv screenDisplay  ){
  int row, column;
  while(1){
    //Enable touch panel
    digitalWrite(13, HIGH);
    TSPoint p = touchPanel.getPoint();
    //Disable tocuh panel
    digitalWrite(13, LOW);
    //mirárselo
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    p.x = map(TS_MAX_X_Y - p.x, TS_MAX_X_Y, TS_MIN_X_Y, screenDisplay.width(), 0);
    p.y = map(TS_MAX_X_Y - p.y, TS_MAX_X_Y, TS_MIN_X_Y, screenDisplay.height(), 0);
    if(p.z >= MINPRESSURE && p.z <= MAXPRESSURE){//Update
      row = p.y/BOXSIZE;
      column= p.x/BOXSIZE;
      
      switch(board[row][column]){
        case WATER:
           board[row][column] = MISS;
           break;
        case BOAT:
          ///METODO PARA COMPROBAR SI SE HA HUNDIDO
           break;
        default:
           continue;
      }
      break;
    }
  }
}
boolean checkIfBoatIsSunk(int row, int column, int board[B_H][B_W]){
  int rowAux = row;
  int columnAux = column;
  //bucle hacia arriba
  while(rowAux >= 0 && board[rowAux][columnAux] != WATER){
    if(board[rowAux][columnAux] == BOAT){
      return false;
    }
    rowAux--;
  }
  //restart
  rowAux = row;
  columnAux = column;
  //bucle hacia la izquierda
  while(columnAux >= 0 && board[rowAux][columnAux] != WATER){
    if(board[rowAux][columnAux] == BOAT){
      return false;
    }
    columnAux--;
  }
  //restart
  rowAux = row;
  columnAux = column;
  //bucle hacia abajo
  while(rowAux < BOARD_HEIGHT && board[rowAux][columnAux] != WATER){
    if(board[rowAux][columnAux] == BOAT){
      return false;
    }
    rowAux++;
  }
  //restart
  rowAux = row;
  columnAux = column;
  //bucle hacia la derecha
  while(columnAux < BOARD_WIDTH && board[rowAux][columnAux] != WATER){
    if(board[rowAux][columnAux] == BOAT){
      return false;
    }
    columnAux++;
  }
  return true;
}

boolean checkValidCoordinate(
  int board [BOARD_HEIGHT][BOARD_WIDTH], int row, int column, int rowLowerIncrement,
  int rowUpperIncrement, int columnLowerIncrement, int columnUpperIncrement) {
  boolean isWaterAround = true;
  for (int rowAux = row + rowLowerIncrement; rowAux <= row + rowUpperIncrement; rowAux++) {
    if (rowAux < 0 || rowAux >= BOARD_HEIGHT) continue;
    for (int columnAux = column + columnLowerIncrement; columnAux <= column + columnUpperIncrement; columnAux++) {
      if (columnAux < 0 || columnAux >= BOARD_WIDTH) continue;
      isWaterAround = board[rowAux][columnAux] == WATER;
      if (!isWaterAround) break;
    }
    if (!isWaterAround) break;
  }
  return isWaterAround;
}

void setBoats(int board [BOARD_HEIGHT][BOARD_WIDTH], int numBoats, int boatLength){
  //colocar barcos
  for (int i = 0; i < numBoats; i++) {
    int row, column, maxValidRow, maxValidColumn, orientation, rowUpperIncrement, columnUpperIncrement;
    int rowLowerIncrement = -1;
    int columnLowerIncrement = -1;
    boolean isWaterAround = true;
    do {
      orientation = random(0,2);
      if (orientation == HORIZONTAL) {
        maxValidRow = BOARD_HEIGHT;
        maxValidColumn = BOARD_WIDTH - boatLength + 1;
        rowUpperIncrement = 1;
        columnUpperIncrement = boatLength;
      } else {
        maxValidRow = BOARD_HEIGHT - boatLength + 1;
        maxValidColumn = BOARD_WIDTH;
        rowUpperIncrement = boatLength;
        columnUpperIncrement = 1;
      }
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

void showBoard(int board [BOARD_HEIGHT][BOARD_WIDTH]){
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      Serial.print(board[i][j]);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void setup() {
  Serial.begin(9600);
  screenDisplay.reset();
  randomSeed(analogRead(A0));
  pinMode(13, OUTPUT);
  //Inicializar pantalla 
  screenDisplay.begin(CONTROLLER);
  screenDisplay.setRotation(1); //Display en LANDSCAPE
  screenDisplay.fillScreen(RED);
}

void loop() {
  // iniciar todas las casillas a agua
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      board[i][j] = WATER;
      board2[i][j] = WATER;
    }
  }
  setBoats(board, NUM_PATROL_BOATS, PATROL_BOAT_LENGTH);
  setBoats(board, NUM_CRUISE, CRUISE_LENGTH);
  setBoats(board, NUM_DESTROYER, DESTROYER_LENGTH);
  setBoats(board, NUM_BATTLESHIP, BATTLESHIP_LENGTH);
  /*setBoats(board2, NUM_PATROL_BOATS, PATROL_BOAT_LENGTH);
  setBoats(board2, NUM_CRUISE, CRUISE_LENGTH);
  setBoats(board2, NUM_DESTROYER, DESTROYER_LENGTH);
  setBoats(board2, NUM_BATTLESHIP, BATTLESHIP_LENGTH);*/
  board[0][0] = IMPACT;
  //showBoard(board);
  drawGrid(screenDisplay, BOXSIZE, board);
  //Serial.println();
  //showBoard(board2);
  //Serial.println("-------------------");
  delay(5000);
}

/*
 j/column ---->
   display.width() = 480
 <-------------------------------------------->
 0   1   2   3   4   5   6   7   8   9   10  11
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 0   ^                        
[0] [0] [1] [3] [3] [3] [0] [0] [0] [0] [0] [0] 1   |                        
[0] [0] [0] [0] [0] [0] [0] [0] [0] [1] [0] [0] 2   |   display.height() = 320                     
[0] [0] [0] [0] [0] [0] [4] [0] [0] [3] [0] [0] 3   |   i/row                    
[0] [0] [0] [0] [0] [0] [0] [0] [0] [1] [0] [0] 4   |   ^                    
[0] [3] [1] [3] [0] [0] [0] [0] [0] [0] [0] [0] 5   |   | 
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 6   |
[0] [0] [0] [0] [0] [0] [0] [0] [0] [0] [0] [0] 7   v                        
board[row][column]
*/
