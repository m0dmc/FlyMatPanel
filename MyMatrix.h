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

// Define outside class file ///////////////////////////////////
#ifndef NUM_ROWS
#define NUM_ROWS 3 // Number of rows of chained INDIVIDUAL PANELS (ROW >= 1 for virtual array)
#endif

#ifndef NUM_COLS
#define NUM_COLS 1 // Number of INDIVIDUAL PANELS
#endif

#define PANEL_CHAIN NUM_ROWS*NUM_COLS    // total number of panels chained one to another

#define VIRTUAL_MATRIX_CHAIN_TYPE CHAIN_BOTTOM_LEFT_UP 

// placeholder for the matrix object
MatrixPanel_I2S_DMA *dma_display = nullptr;

// placeholder for the virtual display object
VirtualMatrixPanel  *virtualDisp = nullptr;

enum DrawShape{Del,Line,LineM};

class MyMatrix{
 private:
  uint16_t bg      = dma_display->color565(0, 0, 0);
  uint16_t fg      = dma_display->color565(255, 255, 255);
  int Brightness =192;
  int VirtWidth;     // set during init dependent on number of panels
  int VirtHeight;    // set during init dependent on number of panels
#define DRAW_SIZE 20
  // Del   - deletes oldest draw
  // LineM - draws line with stacked memory 
  // Line  - draws line without memory
  // enum DrawShape{Del,Line,LineM};
  struct LastDraw{
    int Xpos;
    int Ypos;
    int Xarea;
    int Yarea;
    DrawShape Shape;
  }LastDraw[DRAW_SIZE];
  int NDraws;
  void AppendDraw(DrawShape S,int Xp,int Yp,int Xa,int Ya){
    if(NDraws<DRAW_SIZE-1){
      LastDraw[NDraws].Xpos=Xp;
      LastDraw[NDraws].Ypos=Yp;
      LastDraw[NDraws].Xarea=Xa;
      LastDraw[NDraws].Yarea=Ya;
      LastDraw[NDraws].Shape=S;
    }
    NDraws++;
  }
  void DelDraw(){
    if(NDraws!=0){
      for(int i=0;i<NDraws;i++){
	LastDraw[i].Xpos= LastDraw[i+1].Xpos;
	LastDraw[i].Ypos= LastDraw[i+1].Ypos;
	LastDraw[i].Xarea= LastDraw[i+1].Xarea;
	LastDraw[i].Yarea= LastDraw[i+1].Yarea;
	LastDraw[i].Shape= LastDraw[i+1].Shape;
      }
      NDraws--;
    }
  }
  void ShowXY(int Xp,int Yp,int Xa,int Ya){
    char zz[80];
    sprintf(zz,"Xp=%03d Yp=%03d Xa=%03d Ya=%03d\r\n",Xp,Yp,Xa,Ya);
    SerialIO.SerialOut(zz);
  }
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
    for(int i=0;i<DRAW_SIZE;i++){
      LastDraw[i].Xpos=0;
      LastDraw[i].Ypos=0;
      LastDraw[i].Xarea=0;
      LastDraw[i].Yarea=0;
      LastDraw[i].Shape=Del;
    }
    NDraws=0;
    DrawTestBorder();
    delay(1500);
    ClearScreen();
  }
  void SetBrightness(int b){
    Brightness=(b & 0xff);
    dma_display->setBrightness(Brightness);  // range is 0-255, 0 - 0%, 255 - 100%
  }
  void SetBrightness(){
    SetBrightness(Brightness);
  }
  int GetBrightness(){
    return(Brightness);
  }
  void ClearScreen(){
    virtualDisp->clearScreen();
  }
  void ClearScreen(int bg){
    virtualDisp->fillScreen(bg);
  }
  int VirtualWidth(){
    return(VirtWidth);
  }
  int VirtualHeight(){
    return(VirtHeight);
  }
  void SetFGcol(int r,int g,int b){ // 0-255,0-255,0-255
    fg=dma_display->color565(r&0xff,g&0xff,b&0xff);
  }
  void SetFGcol(int c){
    fg=c;
  }
  uint16_t GetFGcol(){
    return(fg);
  }
  void SetBGcol(int r,int g,int b){ // 0-255,0-255,0-255
    bg=dma_display->color565(r&0xff,g&0xff,b&0xff);
  }
  void SetBGcol(int c){
    bg=c;
  }
  uint16_t GetBGcol(){
    return(bg);
  }
  
  void DrawBG(DrawShape s,int Xp,int Yp,int Xa,int Ya){
     SerialIO.SerialOut("DrawBG ");
     ShowXY(Xp,Yp, Xa,Ya);
    switch(s){
    default:
    case Del:
      break;
    case LineM:
    case Line:
      virtualDisp->drawLine(Xp,Yp,Xa,Ya, bg);
      break;
    }
  }
  void DrawFG(DrawShape s,int Xp,int Yp,int Xa,int Ya){
     SerialIO.SerialOut("DrawFG ");
      ShowXY(Xp,Yp, Xa,Ya);
    switch(s){
    default:
    case Del:
      break;
    case LineM:
    case Line:
      virtualDisp->drawLine(Xp,Yp,Xa,Ya, fg);
      break;
    }
  }
  void Draw(DrawShape s,int Xp,int Yp,int Xa,int Ya){
    switch(s){
    default:
    case Del:
      if(NDraws>0){
	DrawBG(LastDraw[0].Shape,
	       LastDraw[0].Xpos,
	       LastDraw[0].Ypos,
	       LastDraw[0].Xarea,
	       LastDraw[0].Yarea);
	DelDraw();
      }
      break;
    case LineM:
      DrawFG(s,Xp,Yp,Xa,Ya);
      AppendDraw(s,Xp,Yp,Xa,Ya);
      break;
    case Line:
      DrawFG(s,Xp,Yp,Xa,Ya);
      break;
    }
  }

  void DrawTestBorder(){
    virtualDisp->drawRect(1,1, VirtWidth-2, VirtHeight-2, fg);
  }
  void DrawTheLine(int x,uint16_t col){
    virtualDisp->drawLine(0,x,VirtWidth-1,x, col);
  }
  void DrawTheLine(int x){
    DrawTheLine(x,fg);
  }
  
  MyMatrix(){}
  ~MyMatrix(){}
};

#endif
