// Matrix operations
// dmc - Aug 2025
#ifndef MyMatrix_h
#define MyMatrix_h
/*
   -------------------------------------------------------------------------
    Steps to create a virtual display made up of a chain of panels in a grid
    -------------------------------------------------------------------------

    Read the documentation!
    https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-DMA/tree/master/examples/ChainedPanels

    tl/dr:
    
    - Set values for NUM_ROWS, NUM_COLS, PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN_TYPE. 

    - Other than where the matrix is defined and matrix.begin in the setup, you 
      should now be using the virtual display for everything (drawing pixels, writing text etc).
       You can do a find and replace of all calls if it's an existing sketch
      (just make sure you don't replace the definition and the matrix.begin)

    - If the sketch makes use of MATRIX_HEIGHT or MATRIX_WIDTH, these will need to be 
      replaced with the width and height of your virtual screen. 
      Either make new defines and use that, or you can use virtualDisp.width() or .height()
*/
// 1) Include key virtual display library
#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>

// 2) Set configuration
#define PANEL_RES_X 64 // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 32 // Number of pixels tall of each INDIVIDUAL panel module.

//#define NUM_ROWS 3 // Number of rows of chained INDIVIDUAL PANELS (ROW >= 1 for virtual array)
//#define NUM_COLS 1 // Number of INDIVIDUAL PANELS

#define PANEL_CHAIN NUM_ROWS*NUM_COLS    // total number of panels chained one to another

#define VIRTUAL_MATRIX_CHAIN_TYPE CHAIN_BOTTOM_LEFT_UP 

// placeholder for the matrix object
MatrixPanel_I2S_DMA *dma_display = nullptr;

// placeholder for the virtual display object
VirtualMatrixPanel  *virtualDisp = nullptr;

class MyMatrix{
 private:
  uint16_t myBLACK = dma_display->color565(0, 0, 0);
  uint16_t myWHITE = dma_display->color565(255, 255, 255);
  uint16_t myRED   = dma_display->color565(255, 0, 0);
  uint16_t myGREEN = dma_display->color565(0, 255, 0);
  uint16_t myBLUE  = dma_display->color565(0, 0, 255);
  uint16_t bg      = dma_display->color565(0, 0, 0);
  uint16_t fg      = dma_display->color565(255, 255, 255);
  int Brightness =192;
  int VirtWidth;
  int VirtHeight;
 public:
  void Init(){
    // Init matrix boards
    HUB75_I2S_CFG mxconfig(
			   PANEL_RES_X,   // module width
			   PANEL_RES_Y,   // module height
			   PANEL_CHAIN    // chain length
			   );
    
    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    SetBrightness();
    
    // Allocate memory and start DMA display
    if( not dma_display->begin() )SerialIO.SerialOut("I2S memory allocation failed");
    
    // create VirtualDisplay object based on our newly created dma_display object
    virtualDisp = new VirtualMatrixPanel((*dma_display),
					 NUM_ROWS,
					 NUM_COLS,
					 PANEL_RES_X,
					 PANEL_RES_Y,
					 VIRTUAL_MATRIX_CHAIN_TYPE);
    // Set global width and height
    VirtWidth=virtualDisp->width();
    VirtHeight=virtualDisp->height();
    
    delay(500);
    ClearScreen();// Uses default bg colour
    delay(500);
    DrawTestBorder();
    delay(2000);
    ClearScreen();
  }
  void SetBrightness(int b){
    Brightness=(b & 0xff);
    dma_display->setBrightness(Brightness);  // range is 0-255, 0 - 0%, 255 - 100%
  }
  void SetBrightness(){
    SetBrightness(Brightness);
  }
  void ClearScreen(){
    //virtualDisp->fillScreen(bg);
    virtualDisp->clearScreen();
  }
  void DrawTestBorder(){
    virtualDisp->drawRect(1,1, VirtWidth-2, VirtHeight-2, fg);
  }

  int VirtualWidth(){
    return(VirtWidth);
  }
  int VirtualHeight(){
    return(VirtHeight);
  }
  void SetFGcol(int r,int g,int b){
    fg=dma_display->color565(r&0xff,g&0xff,b&0xff);
  }
  void SetFGcol(int c){
    fg=c;
  }
  uint16_t GetFGcol(){
    return(fg);
  }
  void SetBGcol(int r,int g,int b){
    bg=dma_display->color565(r&0xff,g&0xff,b&0xff);
  }
  void SetBGcol(int c){
    bg=c;
  }
  uint16_t GetBGcol(){
    return(bg);
  }

  void DrawTheLine(int x,uint16_t col){
    virtualDisp->drawLine(0,x,VirtWidth-1,x, col);
  }
  void TestBarLR(){
    for (int i=0;i<VirtHeight;i++){
      DrawTheLine(i,fg);
      delay(100);
      DrawTheLine(i,bg);
    }
  }
  void TestBarRL(){
    for (int i=VirtHeight-1;i>=0;i--){
      DrawTheLine(i,fg);
      delay(100);
      DrawTheLine(i,bg);
    }
  }
  
  MyMatrix(){}
  ~MyMatrix(){}
};

#endif
