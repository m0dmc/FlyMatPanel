// This code is used by FlyCinema system to display shapes...
//
// 2025 Aug - dmc
//


#ifndef TermComm_h
#define TermComm_h
class TermComm{
private:
  
public:
  void ShowHelp(){
    char zz[80];
    int bg=MyMatrix.GetBGcol();
    int fg=MyMatrix.GetFGcol();
    int Mode=MyMatrix.GetMode();
    sprintf(zz,"Fly Cinema, brightness = [%02d]\r\n",MyMatrix.GetBrightness());
    SerialIO.SerialOut(zz);
    sprintf(zz,"Background     = [%02d,%02d,%02d](%04d)\r\n",
	    (bg&0xf800)>>10,   (bg&0x7e0)>>5,   (bg&0x1f)<<1,bg);
    SerialIO.SerialOut(zz);
    sprintf(zz,"Foreground     = [%02d,%02d,%02d](%04d)\r\n",
	    (fg&0xf800)>>10,   (fg&0x7e0)>>5,   (fg&0x1f)<<1,fg);
    SerialIO.SerialOut(zz);
    sprintf(zz,"Memory mode    = %s\r\n",(Mode==0?"OFF":(Mode==1?"Remember draw":
							 (Mode==2?"Clear before draw":
							  (Mode==3?"Remember and clear before draw":"###")))));
    SerialIO.SerialOut(zz);
    SerialIO.SerialOut("b##            = set brightness 0-255\r\n");
    SerialIO.SerialOut("bg##,##,##     = set bg[RED,GREEN,BLUE] = 0-63\r\n");
    SerialIO.SerialOut("c              = clear screen\r\n");
    SerialIO.SerialOut("cb             = clear screen to bg colour\r\n");
    SerialIO.SerialOut("db##           = draw bar at ##\r\n");
    SerialIO.SerialOut("dbc##,##       = draw bar at ## col=##\r\n");
    SerialIO.SerialOut("dc##,##,##     = draw circle at x=##,y=## radius=##\r\n");
    SerialIO.SerialOut("dl##,##,##,##  = draw line at ##,##,##,## in fg colour\r\n");
    SerialIO.SerialOut("dr##,##,##,##  = draw rectangle at ##,##,##,##\r\n");
    SerialIO.SerialOut("fr##,##,##,##  = fill rectangle at ##,##,##,##\r\n");
    SerialIO.SerialOut("fg##,##,##     = set fg[RED,GREEN,BLUE] = 0-63\r\n");
    SerialIO.SerialOut("h              = show help\r\n");
    SerialIO.SerialOut("m#             = set mode 0=OFF,1=Remember draw,2=Clear before draw\r\n");
    SerialIO.SerialOut("                          3=Remember and clear before draw\r\n");
    SerialIO.SerialOut("t              = test draw perimeter box\r\n");
    SerialIO.SerialOut("v              = show version\r\n");
    //    SerialIO.SerialOut("Each new draw should be preceded by filling previous draw with background.\r\n");
  }
  bool CommandSetBright(){    // Set brightness
    long val;
    if(SerialIO.CheckMatch("b",&val)){
      MyMatrix.SetBrightness((int)((0xff & val)));
      return(true);
    }
    return(false);
  }
  bool CommandSetBG(){    // Set background
    long valR,valG,valB;  // 0-63,0-63,0-63 - higher bits masked
    if(SerialIO.CheckMatch("bg",&valR,&valG,&valB)){
      MyMatrix.SetBGcol((int)(4*(0x3f & valR)),
			(int)(4*(0x3f & valG)),
			(int)(4*(0x3f & valB)));
      return(true);
    }
    return(false);
  }
  bool CommandClear(){   // Clear to background colour
    int bg=MyMatrix.GetBGcol();
    if(SerialIO.CheckMatch("c")){
      MyMatrix.ClearScreen();
      return(true);
    }
    if(SerialIO.CheckMatch("cb")){
      MyMatrix.ClearScreen(bg);
      return(true);
    }
    return(false);
  }  
  bool CommandDraw(){ 
    long valA,valB,valC,valD;
    int bg=MyMatrix.GetBGcol();
    int fg=MyMatrix.GetFGcol();
    if(SerialIO.CheckMatch("db",&valA)){
      if((valA<0)|(valA>=MyMatrix.VirtualHeight())){
	// value out of range, colour masked
      } else {
	MyMatrix.Draw(Line,0,valA,MyMatrix.VirtualWidth()-1,valA,fg);
      }
      return(true);
    }
    if(SerialIO.CheckMatch("dbc",&valA,&valB)){
      if((valA<0)|(valA>=MyMatrix.VirtualHeight())){
	// value out of range, colour masked
      } else {
	MyMatrix.Draw(Line,0,valA,MyMatrix.VirtualWidth()-1,valA,(valB&0xffff));
      }
      return(true);
    }
    if(SerialIO.CheckMatch("dl",&valA,&valB,&valC,&valD)){
      if( (valA<0)        | (valA>MyMatrix.VirtualWidth())  |
	  (valB<0)        | (valB>MyMatrix.VirtualHeight()) |
	  ((valA+valC)<1) | ((valA+valC)>MyMatrix.VirtualWidth()) |
	  ((valB+valD)<1) | ((valB+valD)>MyMatrix.VirtualHeight()) ){
	// value out of range
      } else {
	MyMatrix.Draw(Line,valA,valB,valA+valC-1,valB+valD-1,fg);
      }
      return(true);
    }
    if(SerialIO.CheckMatch("dc",&valA,&valB,&valC)){
      if( ((valA-valC)<0)  | ((valA+valC)>=MyMatrix.VirtualWidth())  |
	  ((valB-valC)<0)  | ((valB+valC)>=MyMatrix.VirtualHeight()) ){
	// value out of range
      } else {
	MyMatrix.Draw(Circle,valA,valB,valC,valC,fg);
      }
      return(true);
    }
    if(SerialIO.CheckMatch("fc",&valA,&valB,&valC)){
      if( ((valA-valC)<0)  | ((valA+valC)>=MyMatrix.VirtualWidth())  |
	  ((valB-valC)<0)  | ((valB+valC)>=MyMatrix.VirtualHeight()) ){
	// value out of range
      } else {
	MyMatrix.Draw(FillCircle,valA,valB,valC,valC,fg);
      }
      return(true);
    }
    if(SerialIO.CheckMatch("dr",&valA,&valB,&valC,&valD)){
      if( (valA<0)        | (valA>MyMatrix.VirtualWidth())  |
	  (valB<0)        | (valB>MyMatrix.VirtualHeight()) |
	  ((valA+valC)<1) | ((valA+valC)>MyMatrix.VirtualWidth()) |
	  ((valB+valD)<1) | ((valB+valD)>MyMatrix.VirtualHeight()) ){
	// value out of range
      } else {
	MyMatrix.Draw(Rect,valA,valB,valC,valD,fg);
      }
      return(true);
    }
    if(SerialIO.CheckMatch("fr",&valA,&valB,&valC,&valD)){
      if( (valA<0)        | (valA>MyMatrix.VirtualWidth())  |
	  (valB<0)        | (valB>MyMatrix.VirtualHeight()) |
	  ((valA+valC)<1) | ((valA+valC)>MyMatrix.VirtualWidth()) |
	  ((valB+valD)<1) | ((valB+valD)>MyMatrix.VirtualHeight()) ){
	// value out of range
      } else {
	MyMatrix.Draw(FillRect,valA,valB,valC,valD,fg);
      }
      return(true);
    }
    return(false);
  }  
  bool CommandSetFG(){   // Set foreground 
    long valR,valG,valB; // 0-63,0-63,0-63
    if(SerialIO.CheckMatch("fg",&valR,&valG,&valB)){
      MyMatrix.SetFGcol((int)(4*(0x3f & valR)),
			(int)(4*(0x3f & valG)),
			(int)(4*(0x3f & valB)));
      return(true);
    }
    return(false);
  }
  bool CommandTest(){
    if(SerialIO.CheckMatch("t")){      
      MyMatrix.DrawTestBorder();
      Sprite.BounceTest(0);
      return(true);
    }
    return(false);
  }
  bool CommandHelp(){     // HELP!
    if(SerialIO.CheckMatch("h")){
      ShowHelp();
      return(true);
    }
    return(false);
  }
  bool CommandMode(){    // Set mode
    long val;
    if(SerialIO.CheckMatch("m",&val)){
      if((val>=0)&(val<=3)){
	MyMatrix.SetMode((int)(val));
	return(true);
      }
    }
    return(false);
  }
  
  bool CommandVersion(){  // show version
    if(SerialIO.CheckMatch("v")){
      SerialIO.Version();
      return(true);
    }
    return(false);
  }
  bool CheckCommand(){
    if(CommandSetBright())return(true);
    if(CommandSetBG())return(true);
    if(CommandClear())return(true);
    if(CommandDraw())return(true);
    if(CommandSetFG())return(true);
    if(CommandHelp())return(true);
    if(CommandMode())return(true);
    if(CommandTest())return(true);
    if(CommandVersion())return(true);
    return(false); // echo bad string ?
  }
  TermComm(){}
  ~TermComm(){} 
};

#endif
