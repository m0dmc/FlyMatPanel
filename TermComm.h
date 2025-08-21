// This code is used by FlyCinema system to display shapes...
//
// 2025 Aug - dmc
//


#ifndef TermComm_h
#define TermComm_h

class TermComm{
private:
  int BackgroundCol=0;
  int ForegroundCol=0xff;
public:
  void ShowHelp(){
    char zz[80];
    int bg=MyMatrix.GetBGcol();
    int fg=MyMatrix.GetFGcol();
    sprintf(zz,"Fly Cinema, brightness = [%02x]\r\n",MyMatrix.GetBrightness());
    SerialIO.SerialOut(zz);
    sprintf(zz,"Background = [%02d,%02d,%02d] : Foreground = [%02d,%02d,%02d]\r\n",
	    (bg&0xf800)>>10,   (bg&0x7e0)>>5,   (bg&0x1f)<<1,
	    (fg&0xf800)>>10,   (fg&0x7e0)>>5,   (fg&0x1f)<<1);
    SerialIO.SerialOut(zz);
    SerialIO.SerialOut("b##            = set brightness 0-255\r\n");
    SerialIO.SerialOut("bg##,##,##     = set bg[RED,GREEN,BLUE] = 0-63\r\n");
    SerialIO.SerialOut("c              = clear screen\r\n");
    SerialIO.SerialOut("cb             = clear screen to bg colour\r\n");
    SerialIO.SerialOut("dc             = clear last drawn\r\n");
    SerialIO.SerialOut("dl##           = draw line at ##\r\n");
    SerialIO.SerialOut("dlm##          = draw line at ## and remember\r\n");
    SerialIO.SerialOut("dpb            = draw perimeter box\r\n");
    SerialIO.SerialOut("fg##,##,##     = set fg[RED,GREEN,BLUE] = 0-63\r\n");
    SerialIO.SerialOut("h              = show help\r\n");
    SerialIO.SerialOut("t              = test function\r\n");
    SerialIO.SerialOut("v              = show version\r\n");
    SerialIO.SerialOut("Each new draw is preceded by filling previous draw with background.\r\n");
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
    if(SerialIO.CheckMatch("c")){
      MyMatrix.ClearScreen();
      return(true);
    }
    if(SerialIO.CheckMatch("cb")){
      int bg=MyMatrix.GetBGcol();
      MyMatrix.ClearScreen(bg);
      return(true);
    }
    return(false);
  }  
  bool CommandDraw(){ 
    long valA,valB;
    if(SerialIO.CheckMatch("dc")){
      MyMatrix.Draw(Del,0,0,0,0);
      return(true);
    }
    if(SerialIO.CheckMatch("dl",&valA)){
      if((valA<0)|(valA>=MyMatrix.VirtualHeight())){
	// value out of range
      } else {
	MyMatrix.Draw(Line,0,valA,MyMatrix.VirtualWidth()-1,valA);
      }
      return(true);
    }
    if(SerialIO.CheckMatch("dlm",&valB)){
      if((valB<0)|(valB>=MyMatrix.VirtualHeight())){
	// value out of range
      } else {
	MyMatrix.Draw(LineM,0,valB,MyMatrix.VirtualWidth()-1,valB);
      }
      return(true);
    }
    if(SerialIO.CheckMatch("dpb")){
      MyMatrix.DrawTestBorder();
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
    long a,b,c,d;
    if(SerialIO.CheckMatch("t",&a,&b,&c,&d)){
      
      char zz[80];
      sprintf(zz,"A=%03d B=%03d C=%03d D=%03d\r\n",a,b,c,d);
      SerialIO.SerialOut(zz);

      MyMatrix.Draw(LineM,a,b,c,d);
      
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
    if(CommandTest())return(true);
    if(CommandVersion())return(true);
    return(false); // echo bad string ?
  }
  TermComm(){}
  ~TermComm(){} 
};

#endif
