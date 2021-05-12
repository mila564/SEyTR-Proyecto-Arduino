// Libraries

#include <Adafruit_GFX.h>   // Graphic functions 
#include <MCUFRIEND_kbv.h>  // Display
#include <TouchScreen.h>    // Touch screen  

//------------------------

// Display 

// Controller's model

#define CONTROLLER 0x9486

// Pins

#define LCD_CS  A3
#define LCD_CD  A2
#define LCD_WR  A1
#define LCD_RD  A0
#define LCD_RESET  A4

//------------------------

// Touch screen

// Pins

#define XM A2
#define XP 6
#define YP A1
#define YM 7

// Maximum and minimum values read in touch screen

#define TS_MINX 127
#define TS_MAXX 961
#define TS_MINY 117
#define TS_MAXY 921

// Pressure interval detected by touch screen

#define MINPRESSURE 10
#define MAXPRESSURE 1000

//------------------------

// Game

// States

#define WATER 0
#define BOAT 1
#define MISS 2
#define IMPACT 3
#define SUNK 4

// Colors

#define BLUE 0x186EDE   // Miss square
#define RED 0xF800      // Impact square
#define BLACK 0x000000  // Sunk square
#define WHITE 0xFFFFFF  // Grid background
#define LIME 0x9CDB27   // Player 1 background color
#define PURPLE 0x8D27C4 // Player 2 background color
#define ORANGE 0xD67F0D // Win background

// Board dimensions

#define BOARD_WIDTH 12
#define BOARD_HEIGHT 8

// Boat's number

#define NUM_TOTAL_BOATS 10

#define NUM_PATROL_BOATS 4
#define NUM_CRUISE 3
#define NUM_DESTROYER 2
#define NUM_BATTLESHIP 1

// Boat's length

#define PATROL_BOAT_LENGTH 1
#define CRUISE_LENGTH 2
#define DESTROYER_LENGTH 3
#define BATTLESHIP_LENGTH 4

// Boat's orientation

#define HORIZONTAL 0
#define VERTICAL 1

int BOXSIZE = 40; // Size of board squares in pixels

int board [BOARD_HEIGHT][BOARD_WIDTH];
int board2 [BOARD_HEIGHT][BOARD_WIDTH];

int numBoatsPlayer1;
int numBoatsPlayer2;

MCUFRIEND_kbv screenDisplay;  
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
      screenDisplay.drawRect(j*BOXSIZE, i*BOXSIZE, BOXSIZE, BOXSIZE, BLACK);
    }
  }
  delay(500);
}

void drawNextTurnSlide(MCUFRIEND_kbv screenDisplay, char turn, int color, TouchScreen touchPanel){
  int char_size = 10;
  screenDisplay.setRotation(1);
  screenDisplay.fillScreen(color);
  screenDisplay.setCursor(220,130);
  screenDisplay.setTextSize(10);
  screenDisplay.print(turn);
  pinMode(13, OUTPUT);
  while(1){
    digitalWrite(13, HIGH);
    TSPoint p = touchPanel.getPoint();
    delay(100);
    p = touchPanel.getPoint();
    delay(100);
    digitalWrite(13, LOW);
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    p.x = p.x + p.y;       
    p.y = p.x - p.y;            
    p.x = p.x - p.y;
    if(p.z >= MINPRESSURE){
      break;
    }
  }
}

int shoot(int board[BOARD_HEIGHT][BOARD_WIDTH], TouchScreen touchPanel, MCUFRIEND_kbv screenDisplay, int numBoatsPlayer){
  int row, column;
  boolean isSunk;
  pinMode(13, OUTPUT);
  while(1){
    digitalWrite(13, HIGH);
    TSPoint p = touchPanel.getPoint();
    delay(100);
    p = touchPanel.getPoint();
    delay(100);
    digitalWrite(13, LOW);
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    p.x = p.x + p.y;
    p.y = p.x - p.y;
    p.x = p.x - p.y;
    p.x = map(p.x, TS_MINX, TS_MAXX, screenDisplay.width(), 0);
    p.y = (screenDisplay.height() - map(p.y, TS_MINY, TS_MAXY, screenDisplay.height(), 0));
    if(p.z >= MINPRESSURE && p.z <= MAXPRESSURE){//Update
      row = p.y/BOXSIZE;
      column= p.x/BOXSIZE;
      switch(board[row][column]){
        case WATER:
           board[row][column] = MISS;
           break;
        case BOAT:
          board[row][column] = IMPACT;
          isSunk = checkIfBoatIsSunk(row, column, board);
          if(isSunk){
            setBoatSunk(row, column, board);
            --numBoatsPlayer;
            if (numBoatsPlayer == 0) {
              return numBoatsPlayer;
            }
          }
          drawGrid(screenDisplay, BOXSIZE, board);
          continue;
        default:
          continue;
      }
      return numBoatsPlayer;
    }
  }
}

void win(MCUFRIEND_kbv screenDisplay, char player){
  screenDisplay.setRotation(1);
  screenDisplay.fillScreen(ORANGE);
  screenDisplay.setCursor(40,130);
  screenDisplay.setTextSize(5);
  screenDisplay.print("Player ");
  screenDisplay.print(player);
  screenDisplay.print(" wins!");
}

boolean checkIfBoatIsSunk(int row, int column, int board[BOARD_HEIGHT][BOARD_WIDTH]){
  int rowAux = row;
  int columnAux = column;
  // Up
  while(rowAux >= 0 && (board[rowAux][columnAux] != WATER && board[rowAux][columnAux] != MISS)){
    if(board[rowAux][columnAux] == BOAT){
      return false;
    }
    rowAux--;
  }
  // Left
  rowAux = row;
  columnAux = column;
  while(columnAux >= 0 && (board[rowAux][columnAux] != WATER && board[rowAux][columnAux] != MISS)){
    if(board[rowAux][columnAux] == BOAT){
      return false;
    }
    columnAux--;
  }
  // Down
  rowAux = row;
  columnAux = column;
  while(rowAux < BOARD_HEIGHT && (board[rowAux][columnAux] != WATER && board[rowAux][columnAux] != MISS)){
    if(board[rowAux][columnAux] == BOAT){
      return false;
    }
    rowAux++;
  }
  // Right
  rowAux = row;
  columnAux = column;
  while(columnAux < BOARD_WIDTH && (board[rowAux][columnAux] != WATER && board[rowAux][columnAux] != MISS)){
    if(board[rowAux][columnAux] == BOAT){
      return false;
    }
    columnAux++;
  }
  return true;
}

void setBoatSunk(int row, int column, int board[BOARD_HEIGHT][BOARD_WIDTH]){
  int rowAux = row;
  int columnAux = column;
  // Up
  while(rowAux >= 0 && (board[rowAux][columnAux] != WATER && board[rowAux][columnAux] != MISS)){
    board[rowAux][columnAux] = SUNK;
    rowAux--;
  }
  rowAux = row;
  columnAux = column;
  // Left
  while(columnAux >= 0 && (board[rowAux][columnAux] != WATER && board[rowAux][columnAux] != MISS)){
    board[rowAux][columnAux] = SUNK;
    columnAux--;
  }
  rowAux = row;
  columnAux = column;
  // Down
  while(rowAux < BOARD_HEIGHT && (board[rowAux][columnAux] != WATER && board[rowAux][columnAux] != MISS)){
    board[rowAux][columnAux] = SUNK;
    rowAux++;
  }
  rowAux = row;
  columnAux = column;
  // Right
  while(columnAux < BOARD_WIDTH && (board[rowAux][columnAux] != WATER && board[rowAux][columnAux] != MISS)){
    board[rowAux][columnAux] = SUNK;
    columnAux++;
  }
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
    } else { // Vertical
      for(int i = row; i < row + boatLength; i++){
        board[i][column] = BOAT;
      }
    }
  }
}

void setup() {
  screenDisplay.reset();
  randomSeed(analogRead(A0));
  screenDisplay.begin(CONTROLLER);
  screenDisplay.setRotation(1);
  for (int i = 0; i < BOARD_HEIGHT; i++) {
    for (int j = 0; j < BOARD_WIDTH; j++) {
      board[i][j] = WATER;
      board2[i][j] = WATER;
    }
  }
  // Player 1
  setBoats(board, NUM_PATROL_BOATS, PATROL_BOAT_LENGTH);
  setBoats(board, NUM_CRUISE, CRUISE_LENGTH);
  setBoats(board, NUM_DESTROYER, DESTROYER_LENGTH);
  setBoats(board, NUM_BATTLESHIP, BATTLESHIP_LENGTH);
  // Player 2
  setBoats(board2, NUM_PATROL_BOATS, PATROL_BOAT_LENGTH);
  setBoats(board2, NUM_CRUISE, CRUISE_LENGTH);
  setBoats(board2, NUM_DESTROYER, DESTROYER_LENGTH);
  setBoats(board2, NUM_BATTLESHIP, BATTLESHIP_LENGTH);
  numBoatsPlayer1 = NUM_TOTAL_BOATS;
  numBoatsPlayer2 = NUM_TOTAL_BOATS;
}

void loop() {
  drawNextTurnSlide(screenDisplay, '1', PURPLE, touchPanel);
  drawGrid(screenDisplay, BOXSIZE, board);
  numBoatsPlayer2 = shoot(board, touchPanel, screenDisplay, numBoatsPlayer2);
  drawGrid(screenDisplay, BOXSIZE, board);
  if(numBoatsPlayer2 == 0){
    win(screenDisplay, '1');
    while(1) {}
  }
  drawNextTurnSlide(screenDisplay, '2', LIME, touchPanel);
  drawGrid(screenDisplay, BOXSIZE, board2);
  numBoatsPlayer1 = shoot(board2, touchPanel, screenDisplay, numBoatsPlayer1);
  drawGrid(screenDisplay, BOXSIZE, board2);
  if(numBoatsPlayer1 == 0){
    win(screenDisplay, '2');
    while(1) {}
  }
}
