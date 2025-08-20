// Fly panel sprite test routines
// dmc - Aug 2025
#ifndef SpriteTest_h
#define SpriteTest_h

#define SPRITE_SIZE 3
#define SPRITE_XMAX (MyMatrix.VirtualWidth()-SPRITE_SIZE)+1
#define SPRITE_YMAX (MyMatrix.VirtualHeight()-SPRITE_SIZE+1)
#define STEP 4

class SpriteControl{
 private:
  // Start position 
  float Xpos=0;
  float Ypos=0;
  // Position step
  float Xstep=1;
  float Ystep=1;
 public:
  void SetStep(){
    Xstep=random(STEP*0.25,STEP*2);
    Ystep=random(STEP*0.25,STEP*2);
  }
  bool IncCount(float x,float y){
    bool HitWall=false;
    Xpos+=x;
    Ypos+=y;
    while(Xpos>=(2*SPRITE_XMAX)){
      Xpos-=2*SPRITE_XMAX;
      HitWall=true;
    }
    while(Xpos<0){
      Xpos+=2*SPRITE_XMAX;
      HitWall=true;
    }
    while(Ypos>=(2*SPRITE_YMAX)){
      Ypos-=2*SPRITE_YMAX;
      HitWall=true;
    }
    while(Ypos<0){
      Ypos+=2*SPRITE_YMAX;
      HitWall=true;
    }
    return(HitWall);
  }
  int GetXpos(){
    return((Xpos<SPRITE_XMAX?Xpos:2*SPRITE_XMAX-Xpos));
  }
  int GetYpos(){
    return((Ypos<SPRITE_YMAX?Ypos:2*SPRITE_YMAX-Ypos));
  }

  void BounceTest(){
    int xp,yp,i=0;
    bool HitWall=false;
    do{
      HitWall=IncCount(Xstep,Ystep);
      xp=GetXpos();
      yp=GetYpos();
      virtualDisp->fillCircle(xp+1,yp+1,2, MyMatrix.GetFGcol());
      delay(30);
      virtualDisp->fillCircle(xp+1,yp+1,2, MyMatrix.GetBGcol());
      if(HitWall)SetStep();
      LEDext.Toggle();
    }while(i++<100);
    

  }

  
  SpriteControl(){}
  ~SpriteControl(){}
}Sprite;

#endif
