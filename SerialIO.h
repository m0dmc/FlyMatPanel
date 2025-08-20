// This code was used by Sequence control program, separated into
// separate file to simplify editing and reading code.
// This class only deals with IO to serial port.  
//
// 2020 October 14 - dmc
//
// serial port io functions, buffered to avoid blocking conflicts with IRQ timing


#ifndef SerialIO_h
#define SerialIO_h

#define BUFF_LENGTH 20
#define TERM_CHAR 13

class SerialIO{
private:
  char cbuf[BUFF_LENGTH];
  char NewString[BUFF_LENGTH];
  int NewStringLength;
  int cptr=0;
  char zz[50];
  void ClearBuffer();
  void ClearNewString();
  byte AppendBuffer();
  // check command string match
  bool CheckQueryString(char * QueryString,bool exactmatch);
  // check command string with parameters and comma delimiting
  int CheckQueryComma(char * QueryString);      // first comma
  int CheckQueryComma(char * QueryString,int n);// next comma?
public:
  void Init();
  bool CheckSerialIn(); // return length of new string after CR received
  void GetSerialInString(char *x);
  bool CheckMatch(char * QueryString); // check new serial input matches query string
  bool CheckMatch(char * QueryString,double *mmm);
  bool CheckMatch(char * QueryString,long *nnn);
  bool CheckMatch(char * QueryString,double *mmm,long *nnn);
  bool CheckMatch(char * QueryString,double *mmm,double *nnn);
  bool CheckMatch(char * QueryString,long *mmm,long *nnn);
  bool CheckMatch(char * QueryString,long *mmm,long *nnn,long *ppp);
  bool CheckMatch(char * QueryString,double *mmm,long *nnn,long *ppp);
  void SerialOut(char * x);
  void EchoNewString();
  void Version();
  SerialIO(){}
  ~SerialIO(){}
};

void SerialIO::ClearBuffer(){
  byte i;
  for(i=0;i<BUFF_LENGTH;i++)cbuf[i]=0;
  cptr=0; // next vacant char array location
}

void SerialIO::ClearNewString(){
  byte i;
  for(i=0;i<BUFF_LENGTH;i++)NewString[i]=0;
}

byte SerialIO::AppendBuffer(){
  byte c;
  while(SerialPort.available()){
    c=SerialPort.read();
    if(cptr<BUFF_LENGTH)cbuf[cptr++]=c;
    if(cptr>=BUFF_LENGTH){ // response to buffer overflow
      if(c==TERM_CHAR){
	cptr=0;
	return(0);
      } else {
	return(-1);
      }
    } else {
      if(c==TERM_CHAR){
	cbuf[cptr-1]=0;
	strcpy(NewString,cbuf);
	NewStringLength=cptr-1;
	cptr=0;
	return(NewStringLength);
      }
    }
  }
  return(0);
}

bool SerialIO::CheckQueryString(char * QueryString,bool exactmatch){
  int lenQuery=strlen(QueryString);
  bool match=(0==strncasecmp(NewString,QueryString,lenQuery));// ignore case
  if(exactmatch){ // Exact match without trailing characters
    if(lenQuery==NewStringLength)return(match);
  } else {        // Match with trailing characters
    if(lenQuery<NewStringLength)return(match);
  }
  return(false);
}

// check command string with parameters and comma delimiting
int SerialIO::CheckQueryComma(char * QueryString,int n){
  int lenQuery;
  int i,j,comma=0;
  //  char zz[50];  // for debugging
  j=n;
  lenQuery=strlen(QueryString);
  //  sprintf(zz,"len=%d newstrlen=%d [%s,%s]\n",lenQuery,NewStringLength,QueryString,NewString);
  //  SerialPort.println(zz);
  if(lenQuery<(NewStringLength-2)){
    if(0==strncasecmp(NewString,QueryString,lenQuery)){
      i=lenQuery;
      do{
	++i;
	if(NewString[i]==',') j--; // countdown commas
	//	sprintf(zz,"i=%d j=%d [%c]\n",i,j,NewString[i]);
	//	SerialPort.println(zz);
      } while(((NewString[i]!=',')||(j!=0))&&(i<NewStringLength));
      if(NewString[i]==','){
	comma=i;
      }
    }
  }
  return(comma);
}
int SerialIO::CheckQueryComma(char * QueryString){
  return(CheckQueryComma(QueryString,1));
}

void SerialIO::Init(){
  long TimePlus5s=millis()+5000;
  SerialPort.begin(BAUDRATE);
  while ((!SerialPort)&(TimePlus5s>millis()))delay(200);
  delay(200);
}

bool SerialIO::CheckSerialIn(){
  return(AppendBuffer()>0);
}

void SerialIO::GetSerialInString(char *x){
  strcpy(x,NewString);
}

bool SerialIO::CheckMatch(char * QueryString){
  bool match;
  match=(CheckQueryString(QueryString,true));
  if(match)EchoNewString();
  return(match);
}

bool SerialIO::CheckMatch(char * QueryString,double *mmm){
  char * ptrEnd;
  int lenQuery;
  bool match;
  lenQuery=strlen(QueryString);
  if(CheckQueryString(QueryString,false)){
    *mmm=strtod(NewString+lenQuery,&ptrEnd);
    match=(ptrEnd==(NewString+NewStringLength));
    if(match)EchoNewString();
    return(match);
  }
  return(false);
}

bool SerialIO::CheckMatch(char * QueryString,long *nnn){
  char * ptrEnd;
  int lenQuery;
  bool match;
  lenQuery=strlen(QueryString);
  if(CheckQueryString(QueryString,false)){
    *nnn=strtol(NewString+lenQuery,&ptrEnd,10);
    match=(ptrEnd==(NewString+NewStringLength));
    if(match)EchoNewString();
    return(match);
  }
  return(false);
}

bool SerialIO::CheckMatch(char * QueryString,double *mmm,long *nnn){
  // initial chars followed by (double) mm.mm "," (long) nnn
  char * ptrEnd;
  int lenQuery;
  int i;
  bool match;
  lenQuery=strlen(QueryString);
  i=CheckQueryComma(QueryString);
  if(i!=0){
    *mmm=strtod(NewString+lenQuery,&ptrEnd);
    if(ptrEnd==(NewString+i)){
      *nnn=strtol(NewString+i+1,&ptrEnd,10);
      match=(ptrEnd==(NewString+NewStringLength));
      if(match)EchoNewString();
      return(match);
    }
  }
  return(false);
}

bool SerialIO::CheckMatch(char * QueryString,double *mmm,double *nnn){
  // initial chars followed by (double) mm.mm "," (double) nn.nn
  char * ptrEnd;
  int lenQuery;
  int i;
  bool match;
  lenQuery=strlen(QueryString);
  i=CheckQueryComma(QueryString);
  if(i!=0){
    *mmm=strtod(NewString+lenQuery,&ptrEnd);
    if(ptrEnd==(NewString+i)){
      *nnn=strtod(NewString+i+1,&ptrEnd);
      match=(ptrEnd==(NewString+NewStringLength));
      if(match)EchoNewString();
      return(match);
    }
  }
  return(false);
}

bool SerialIO::CheckMatch(char * QueryString,long *mmm,long *nnn){
  // initial chars followed by (long) mmm "," (long) nnn
  char * ptrEnd;
  int lenQuery;
  int i;
  bool match;
  lenQuery=strlen(QueryString);
  i=CheckQueryComma(QueryString);
  if(i!=0){
    *mmm=strtol(NewString+lenQuery,&ptrEnd,10);
    if(ptrEnd==(NewString+i)){
      *nnn=strtol(NewString+i+1,&ptrEnd,10);
      match=(ptrEnd==(NewString+NewStringLength));
      if(match)EchoNewString();
      return(match);
    }
  }
  return(false);
}

bool SerialIO::CheckMatch(char * QueryString,long *mmm,long *nnn,long *ppp){
  char * ptrEnd;
  int lenQuery;
  int i,j;
  bool match;
  //  char zz[50];
  lenQuery=strlen(QueryString);      // Length of query string (command characters)
  i=CheckQueryComma(QueryString);    // first comma
  j=CheckQueryComma(QueryString,2);  // second comma

  //  sprintf(zz,"%s i=%d j=%d\n",NewString,i,j);
  //  SerialPort.println(zz);
  
  if((i!=0)&(j!=0)){
    *mmm=strtol(NewString+lenQuery,&ptrEnd,10);
    if(ptrEnd==(NewString+i)){
      *nnn=strtol(NewString+i+1,&ptrEnd,10);
      if(ptrEnd==(NewString+j)){
	*ppp=strtol(NewString+j+1,&ptrEnd,10);
	match=(ptrEnd==(NewString+NewStringLength));
	if(match)EchoNewString();

	//float m=(*mmm);
	//int n=(*nnn),p=(*ppp);
	//sprintf(zz,"mmm=%f nnn=%d ppp=%d %d",m,n,p,match);
	//SerialPort.println(zz);
	
	return(match);
      }
    }	    
  }
  
  return(false);
}

bool SerialIO::CheckMatch(char * QueryString,double *mmm,long *nnn,long *ppp){
  // initial chars followed by (double) mmm "," (long) nnn "," (long) ppp
  char * ptrEnd;
  int lenQuery;
  int i,j;
  bool match;
  //  char zz[50];
  lenQuery=strlen(QueryString);      // Length of query string (command characters)
  i=CheckQueryComma(QueryString);    // first comma
  j=CheckQueryComma(QueryString,2);  // second comma

  //  sprintf(zz,"%s i=%d j=%d\n",NewString,i,j);
  //  SerialPort.println(zz);
  
  if((i!=0)&(j!=0)){
    *mmm=strtod(NewString+lenQuery,&ptrEnd);
    if(ptrEnd==(NewString+i)){
      *nnn=strtol(NewString+i+1,&ptrEnd,10);
      if(ptrEnd==(NewString+j)){
	*ppp=strtol(NewString+j+1,&ptrEnd,10);
	match=(ptrEnd==(NewString+NewStringLength));
	if(match)EchoNewString();

	//float m=(*mmm);
	//int n=(*nnn),p=(*ppp);
	//sprintf(zz,"mmm=%f nnn=%d ppp=%d %d",m,n,p,match);
	//SerialPort.println(zz);
	
	return(match);
      }
    }	    
  }
  
  return(false);
}


void SerialIO::SerialOut(char * x){
  SerialPort.print(x);
}

void SerialIO::EchoNewString(){
  SerialPort.println(NewString);
}

void SerialIO::Version(){
  SerialOut(VERSION_LINE1);
  SerialOut(VERSION_LINE2);
}

#endif
