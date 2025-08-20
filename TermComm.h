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
    SerialIO.SerialOut("Fly Cinema \r\n");
    sprintf(zz,"Background = [%04x] : Foreground = [%04x]\r\n",MyMatrix.GetBGcol(),MyMatrix.GetFGcol());
    SerialIO.SerialOut(zz);
    SerialIO.SerialOut("b              = draw box\r\n");
    SerialIO.SerialOut("c              = clear screen\r\n");
    SerialIO.SerialOut("h              = show help\r\n");
    SerialIO.SerialOut("v              = show version\r\n");
    SerialIO.SerialOut("bg##           = set bg=##\r\n");
    SerialIO.SerialOut("fg##           = set fg=##\r\n");
    SerialIO.SerialOut("l##            = draw line at ##\r\n");
    //    SerialIO.SerialOut("r##,##         = rectangle draw & fill at XX,YY\r\n");
    SerialIO.SerialOut("Each new draw is preceded by filling previous draw with background.\r\n");
  }
  bool CommandBox(){   // Draw border box
    if(SerialIO.CheckMatch("b")){
      MyMatrix.DrawTestBorder();
      return(true);
    }
    return(false);
  }  
  bool CommandClear(){   // Clear to background colour
    if(SerialIO.CheckMatch("c")){
      MyMatrix.ClearScreen();
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
  bool CommandSetBG(){    // Set background
    long val;
    if(SerialIO.CheckMatch("bg",&val)){
      MyMatrix.SetBGcol((int)(0xffff & val));
      return(true);
    }
    if(SerialIO.CheckMatch("bg",&valR,&valG,&valB)){
      MyMatrix.SetBGcol((int)(0xff & valR),
			(int)(0xff & valG),
			(int)(0xff & valB));
      return(true);
    }
    return(false);
  }
  bool CommandSetFG(){   // Set foreground 
    long val,valR,valG,valB;
    if(SerialIO.CheckMatch("fg",&val)){
      MyMatrix.SetFGcol((int)(0xffff & val));
      return(true);
    }
    if(SerialIO.CheckMatch("fg",&valR,&valG,&valB)){
      MyMatrix.SetFGcol((int)(0xff & valR),
			(int)(0xff & valG),
			(int)(0xff & valB));
      return(true);
    }
    return(false);
  }
  bool CommandLine(){ 
    long val;
    int lx;
    if(SerialIO.CheckMatch("l",&val)){
      if(val<0){
	lx=0;
      } else if(val>=MyMatrix.VirtualHeight()){
	lx=MyMatrix.VirtualHeight()-1;
      } else {
	lx=val;
      }
      MyMatrix.DrawTheLine(lx,MyMatrix.GetFGcol());
      delay(250);
      MyMatrix.DrawTheLine(lx,MyMatrix.GetBGcol());
      return(true);
    }
    return(false);
  }
  bool CommandFillRect(); // Draw rectangle and fill
  bool CheckCommand(){
    if(CommandBox())return(true);
    if(CommandClear())return(true);
    if(CommandHelp())return(true);
    if(CommandVersion())return(true);
    if(CommandSetBG())return(true);
    if(CommandSetFG())return(true);
    if(CommandLine())return(true);
    return(false); // echo bad string ?
  }
  TermComm(){}
  ~TermComm(){} 
};

#endif
