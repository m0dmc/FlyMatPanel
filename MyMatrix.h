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
#define OLD_PANEL
#ifdef OLD_PANEL
////////////////////////////////////////////////////////////////////////////
// 2) Set configuration
#define PANEL_RES_X 64 // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 32 // Number of pixels tall of each INDIVIDUAL panel module.

#ifndef NUM_ROWS
#define NUM_ROWS 3 // Number of rows of chained INDIVIDUAL PANELS (ROW >= 1 for virtual array)
#endif

#ifndef NUM_COLS
#define NUM_COLS 1 // Number of INDIVIDUAL PANELS
#endif
////////////////////////////////////////////////////////////////////////////
#else
//New panel

#define PANEL_RES_X 172 // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 86 // Number of pixels tall of each INDIVIDUAL panel module.

#ifndef NUM_ROWS
#define NUM_ROWS 1 // Number of rows of chained INDIVIDUAL PANELS (ROW >= 1 for virtual array)
#endif

#ifndef NUM_COLS
#define NUM_COLS 1 // Number of INDIVIDUAL PANELS
#endif
/////////////////////////////////////////////////////////////////////////////
#endif

#define PANEL_CHAIN NUM_ROWS*NUM_COLS    // total number of panels chained one to another

#define VIRTUAL_MATRIX_CHAIN_TYPE CHAIN_BOTTOM_LEFT_UP 

// placeholder for the matrix object
MatrixPanel_I2S_DMA *dma_display = nullptr;

// placeholder for the virtual display object
VirtualMatrixPanel  *virtualDisp = nullptr;

enum DrawShape{Nul,Line,Rect,FillRect,Circle,FillCircle};

class MyMatrix{
 private:
  uint16_t bg      = dma_display->color565(0, 0, 0);
  uint16_t fg      = dma_display->color565(255, 255, 255);
  int Brightness =192;
  int VirtWidth;     // set during init dependent on number of panels
  int VirtHeight;    // set during init dependent on number of panels
#define DRAW_SIZE 10
  struct LastDraw{
    int Xpos;
    int Ypos;
    int Xarea;
    int Yarea;
    int Colour;
    DrawShape Shape;
  }LastDraw[DRAW_SIZE];
  int NDraws;
  int Mode=0;// 0=off, 1 remember drawn, 2 delete before next draw
  void AppendDraw(DrawShape S,int Xp,int Yp,int Xa,int Ya,int Colour){
    if(NDraws<DRAW_SIZE-1){
      LastDraw[NDraws].Xpos=Xp;
      LastDraw[NDraws].Ypos=Yp;
      LastDraw[NDraws].Xarea=Xa;
      LastDraw[NDraws].Yarea=Ya;
      LastDraw[NDraws].Colour=Colour;
      LastDraw[NDraws].Shape=S;
    }
    NDraws++;
  }
  void DelDraw(){
    if(NDraws!=0){
      for(int i=0;i<NDraws;i++){
	LastDraw[i].Xpos  = LastDraw[i+1].Xpos;
	LastDraw[i].Ypos  = LastDraw[i+1].Ypos;
	LastDraw[i].Xarea = LastDraw[i+1].Xarea;
	LastDraw[i].Yarea = LastDraw[i+1].Yarea;
	LastDraw[i].Colour= LastDraw[i+1].Colour;
	LastDraw[i].Shape = LastDraw[i+1].Shape;
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
    //delay(500);
    
    // Allocate memory and start DMA display
    if( not dma_display->begin() )SerialIO.SerialOut("I2S memory dma_display allocation failed\r\n");
#ifdef OLD_PANEL
    SerialIO.SerialOut("OLD Panel\r\n");
#else
    SerialIO.SerialOut("Not OLD Panel\r\n");
#endif
    SetBrightness();
  
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
      LastDraw[i].Colour=0;
      LastDraw[i].Shape=Nul;
    }
    NDraws=0;
    DrawTestBorder();
    delay(1500);
    ClearScreen();
  }
  void SetMode(int x){
    Mode=x;
  }
  int GetMode(){
    return(Mode);
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
    NDraws=0;
  }
  void ClearScreen(int bg){
    virtualDisp->fillScreen(bg);
    NDraws=0;
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

  void DrawNow(DrawShape s,int Xp,int Yp,int Xa,int Ya,int col){
    SerialIO.SerialOut("DrawShape ");
    ShowXY(Xp,Yp,Xa,Ya);
    switch(s){
    default:
      break;
    case Circle:
      virtualDisp->drawCircle(Xp,Yp,Xa, col);
      break;
    case Line:
      virtualDisp->drawLine(Xp,Yp,Xa,Ya, col);
      break;
    case Rect:
      virtualDisp->drawRect(Xp,Yp,Xa,Ya, col);
      break;
    case FillRect:
      virtualDisp->fillRect(Xp,Yp,Xa,Ya, col);
      break;
    case FillCircle: //Ya ignored (Xa=diameter)
      virtualDisp->fillCircle(Xp,Yp,Xa, col);
      break;
    }
  }

  void Draw(DrawShape s,int Xp,int Yp,int Xa,int Ya,int col){
    if((Mode==2)|(Mode==3)){
      if(NDraws>0){
	SerialIO.SerialOut("ClearShape ");
	ShowXY(LastDraw[0].Xpos,
	       LastDraw[0].Ypos,
	       LastDraw[0].Xarea,
	       LastDraw[0].Yarea);
	DrawNow(LastDraw[0].Shape,
	     LastDraw[0].Xpos,
	     LastDraw[0].Ypos,
	     LastDraw[0].Xarea,
	     LastDraw[0].Yarea,
	     bg);
	DelDraw();
      }
    }
    if((Mode==1)|(Mode==3)){
      AppendDraw(s,Xp,Yp,Xa,Ya,col);
    }
    DrawNow(s,Xp,Yp,Xa,Ya,col);
  }

  void DrawTestBorder(){
    DrawNow(Rect,0,0, VirtWidth, VirtHeight, fg);
  }
  
  MyMatrix(){}
  ~MyMatrix(){}
};

#endif
