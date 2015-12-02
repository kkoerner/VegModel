//---------------------------------------------------------------------------
/**\file
   \brief Implement Parameter Exchange
*/
//#include <vcl.h>
//#include <stdlib.h>
#pragma hdrstop
#include <fstream>
#include <iostream>
#include <iomanip>
#include <math.h>
#include "prefs.h"
#include "mresults.h"

#include "random.h"
//-----------------------------
//Preferences::Preferences(){//Konstruktor
  int Preferences::ID=999999;
  int Preferences::MaxX=129;    //--geht nicht hier zu definieren
  int Preferences::MaxY=129;
  int Preferences::maxRuns=0;
  int Preferences::maxYears=0;
  int Preferences::InitTime=200;

  double Preferences::transitions[5][5]={0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0,
                                         0,0,0,0,0, 0,0,0,0,0};
  double Preferences::weather[2][5]= {0,0,0,0,0, 0,0,0,0,0};
  double Preferences::pcPotHab=0;
  double Preferences::pcInitPop=0;
  double Preferences::patchDecreaseRate=1;
  double Preferences::disturbanceRate=0;
  double Preferences::dispersed=0; ///<Anteil verbreiteter Samen
  double Preferences::compAbil=0;  ///<competetive ability high:+x neutral:0 low:-x
  float Preferences::manMode=0;
//  eMode Preferences::manMode=NONE;
  double Preferences::manEffect=1;
  double Preferences::man[5]={1,0,1,0,1};
  double Preferences::dimension=4.0;
  string Preferences::startFile=(string)"toRun.txt";
  string Preferences::path="";
  vector<float> Preferences::MusterCapacity= vector<float>();
  vector<bool> Preferences::MusterResultCells= vector<bool>();
//}


/// Constructor
Preferences::Preferences(){
  o_ID=ID;//++ID;
  for (int i=0;i<5;i++)for (int j=0;j<5;j++)
    this->o_transitions[i][j]=transitions[i][j];
  for (int i=0;i<2;i++)for (int j=0;j<5;j++)
    this->o_weather[i][j]=weather[i][j];
//  o_transitions=transitions;
//  o_weather=weather;
  o_pcPotHab=pcPotHab;//0;
  o_pcInitPop=pcInitPop;//0;
  o_patchDecreaseRate=patchDecreaseRate;//1;
  o_disturbanceRate=disturbanceRate;
  o_dispersed=dispersed; //Anteil verbreiteter Samen
  o_compAbil=compAbil;  //competetive ability high:+x neutral:0 low:-x
  o_manMode=manMode;
  o_manEffect=manEffect;
  for (int i=0;i<5;i++)
    this->o_man[i]=man[i];
//  o_man=man;
}//Construktor

//bool operator==(Preferences& a,Preferences& b){
//  return a.operator ==(b);
//}
/// Überladen des Vergleichsoperatoren ==
bool const Preferences::operator==(const Preferences& p){
  for (int i=0;i<5;i++)for (int j=0;j<5;j++)
    if (this->o_transitions[i][j]!=p.o_transitions[i][j]) return false;
  for (int i=0;i<2;i++)for (int j=0;j<5;j++)
    if (this->o_weather[i][j]!=p.o_weather[i][j]) return false;
  if (o_pcPotHab!=p.o_pcPotHab) return false;//0;
  if (o_pcInitPop!=p.o_pcInitPop) return false;//0;
  if (o_patchDecreaseRate!=patchDecreaseRate) return false;//1;
  if (o_disturbanceRate!=p.o_disturbanceRate) return false;
  if (o_dispersed!=p.o_dispersed) return false; //Anteil verbreiteter Samen
  if (o_compAbil!=p.o_compAbil) return false;  //competetive ability high:+x neutral:0 low:-x
  if (o_manMode!=p.o_manMode) return false;
  if (o_manEffect!=p.o_manEffect) return false;
  for (int i=0;i<5;i++)
    if (this->o_man[i]!=p.o_man[i]) return false;
  return true;
}


///Copy-Constructor
Preferences::Preferences(const Preferences& p){
  o_ID=p.o_ID;
  for (int i=0;i<5;i++)for (int j=0;j<5;j++)
    this->o_transitions[i][j]
         =p.o_transitions[i][j];
  for (int i=0;i<2;i++)for (int j=0;j<5;j++)
    this->o_weather[i][j]
          =p.o_weather[i][j];
//  o_transitions=p->o_transitions;
//  o_weather=p->o_weather;
  o_pcPotHab=p.o_pcPotHab;//0;
  o_pcInitPop=p.o_pcInitPop;//0;
  o_patchDecreaseRate=p.o_patchDecreaseRate;//1;
  o_disturbanceRate=p.o_disturbanceRate;
  o_dispersed=p.o_dispersed; //Anteil verbreiteter Samen
  o_compAbil=p.o_compAbil;  //competetive ability high:+x neutral:0 low:-x
  o_manMode=p.o_manMode;
  o_manEffect=p.o_manEffect;
  for (int i=0;i<5;i++)
    this->o_man[i]=p.o_man[i];
//  o_man=p->o_man;
}
//--------------------------------------------------
/**
  creates a new set of preferences, depending on values ranges given by
  \param min and \param max.
*/
void Preferences::getNewPrefSet(const Preferences* min, const Preferences* max)
{ this->getNewPrefSet(*min,*max);
}
void Preferences::getNewPrefSet(const Preferences& min, const Preferences& max)
{ //zZt alle Parameter simultan geändert
cout<<"getNewPrefSet...";
  o_ID=std::max(std::max(min.o_ID,max.o_ID),ID)+1;
  for (int i=0;i<5;i++)for (int j=0;j<5;j++)
//  this->setOTransition((state)i,(state)j,
//         MRandom::rnd_range(min.o_transitions[i][j],max.o_transitions[i][j]));
  this->o_transitions[i][j]
        =MRandom::rnd_range(min.o_transitions[i][j],max.o_transitions[i][j]);
  //translate t to p -->estab and growth parameter:
  float p1=o_transitions[1][2], p2=o_transitions[1][3],p3=o_transitions[1][4];
  o_transitions[1][2]=p1*(1-p2);o_transitions[1][3]=p1*p2*(1-p3);o_transitions[1][4]=p1*p2*p3;
  float gr1=o_transitions[2][3],gr2=o_transitions[2][4];
  o_transitions[2][3]=gr1*(1-gr2);o_transitions[2][4]=gr1*gr2;
  //--------------------------------
  for (int i=0;i<2;i++)for (int j=0;j<5;j++)
  this->o_weather[i][j]
       =MRandom::rnd_range(min.o_weather[i][j],max.o_weather[i][j]);
  o_pcPotHab
       =MRandom::rnd_range(min.o_pcPotHab,max.o_pcPotHab);
  o_pcInitPop
       =MRandom::rnd_range(min.o_pcInitPop,max.o_pcInitPop);
  o_patchDecreaseRate
       =MRandom::rnd_range(min.o_patchDecreaseRate,max.o_patchDecreaseRate);
  o_disturbanceRate
       =MRandom::rnd_range(min.o_disturbanceRate,max.o_disturbanceRate);
  o_dispersed
       =MRandom::rnd_log_range(min.o_dispersed,max.o_dispersed); //Anteil verbreiteter Samen
  o_compAbil
       =MRandom::rnd_range(min.o_compAbil,max.o_compAbil);  //competetive ability high:+x neutral:0 low:-x
  o_manMode
       = MRandom::rnd_range(min.o_manMode,max.o_manMode);
  o_manEffect
       =MRandom::rnd_range(min.o_manEffect,max.o_manEffect);
  for (int i=0;i<5;i++)
    this->o_man[i]=MRandom::rnd_range(min.o_man[i],max.o_man[i]);
cout<<"done\n";
}//end getNewPrefSet(min,max)
//--------------------------------------------------
/**
  creates a new set of preferences, depending on values ranges given by
  \param min and \param max.
  there are the number <nchange> parameters rendomly changed
  (depending on \param change)
*/
void Preferences::getGNVarPrefSet(const Preferences* min, const Preferences* max,
     const float change, const int nchange)
{ this->getGNVarPrefSet(*min,*max,change,nchange);
}
void Preferences::getGNVarPrefSet(const Preferences &min, const Preferences &max,
     const float change, const int nchange)
{
cout<<"get new PC:\n";
 // if (min.operator ==(max)) throw Exception("Boundaries equal!");
  o_ID=std::max(std::max(min.o_ID,max.o_ID),ID)+1;
  Preferences* old=new Preferences(*this);
  for (int nb=0;nb<nchange;nb++){
    Preferences* lastV=new Preferences(*this);
    do{
cout<<".";
      int parameter=int(MRandom::rnd01()*48.0);//rand()%48;
      if (parameter<25) { int j=(parameter)/5;int i=(parameter)%5;
         float value=o_transitions[i][j];
         switch (parameter){
           //for the transformed parameters p1-3 and gr1,2 ...
           case 21: case 16: case 11: case 17: case 22:
           changeOTransition((state)i,(state)j,
             min.o_transitions[i][j],max.o_transitions[i][j],
             MRandom::g_rnd_range(-.5,.5,0,change));break;
           //for all other transition params ...
           default:
             this->setOTransition((state)i,(state)j,
             MRandom::g_rnd_range(min.o_transitions[i][j],max.o_transitions[i][j],
               value,change));break;
         }
      }else if(parameter<35){ int i=(parameter-25)/5;int j=(parameter-25)%5;
       this->o_weather[i][j]
        =MRandom::g_rnd_range(min.o_weather[i][j],max.o_weather[i][j],
         o_weather[i][j],change);
      }else if(parameter<40){int i=(parameter-35)%5;
       this->o_man[i]=MRandom::g_rnd_range(min.o_man[i],max.o_man[i],
                      o_man[i],change);
      }else switch (parameter){
       case 40:
         o_pcPotHab=MRandom::g_rnd_range(min.o_pcPotHab,max.o_pcPotHab,
                    o_pcPotHab,change);
         break;
       case 41:
         o_pcInitPop=MRandom::g_rnd_range(min.o_pcInitPop,max.o_pcInitPop,
                     o_pcInitPop,change);
         break;
       case 42:
         o_patchDecreaseRate=MRandom::g_rnd_range(min.o_patchDecreaseRate,
           max.o_patchDecreaseRate,o_patchDecreaseRate,change);break;
       case 43:
         o_disturbanceRate=MRandom::g_rnd_range(min.o_disturbanceRate,
           max.o_disturbanceRate,o_disturbanceRate,change);break;
       case 44:
         o_dispersed=MRandom::g_rnd_log_range(min.o_dispersed,
           max.o_dispersed,o_dispersed,change); break;//Anteil verbreiteter Samen
       case 45:
         o_compAbil=MRandom::g_rnd_range(min.o_compAbil,max.o_compAbil,
                  o_compAbil,change);
         break;  //competetive ability high:+x neutral:0 low:-x
       case 46:
         o_manMode= MRandom::g_rnd_range(min.o_manMode,max.o_manMode,
                 o_manMode,change);
         break;
       case 47:
         o_manEffect=MRandom::g_rnd_range(min.o_manEffect,max.o_manEffect,
                   o_manEffect,change);
         break;
      }//end switch
    }while(*this==*old || *this==*lastV);
    delete lastV; lastV=0;
  }//end nb of changed parameters
  delete old;old=0;
cout<<"found.\n";
}//end getGNVarPrefSet
//--------------------------------------------------
/**
  creates a new set of preferences, depending on values ranges given by
  \param min and \param max.
  there is only one parameter rendomly changed (depending on \param change)
*/
void Preferences::getGVarPrefSet(const Preferences* min, const Preferences* max, const float change)
{ this->getGVarPrefSet(*min,*max,change);
}
void Preferences::getGVarPrefSet(const Preferences &min, const Preferences &max, const float change)
{
  getGNVarPrefSet(min,max,change);
}//end getGVarPrefSet
//--------------------------------------------------

///convert the static values to an Object
void Preferences::static2object(void){
  this->o_ID=ID;
  for (int i=0;i<5;i++)for (int j=0;j<5;j++)
    this->o_transitions[i][j]
         =transitions[i][j];
  for (int i=0;i<2;i++)for (int j=0;j<5;j++)
    this->o_weather[i][j]=weather[i][j];
  this->o_pcPotHab=pcPotHab;
  this->o_pcInitPop=pcInitPop;
  this->o_patchDecreaseRate=patchDecreaseRate;
  this->o_disturbanceRate=disturbanceRate;
  this->o_dispersed=dispersed; //Anteil verbreiteter Samen
  this->o_compAbil=compAbil;  //competetive ability high:+x neutral:0 low:-x
  this->o_manMode=manMode;
  this->o_manEffect=manEffect;
  for (int i=0;i<5;i++)
    this->o_man[i]=man[i];
}

/// convert an object to static values
void Preferences::object2static(void){
  ID=this->o_ID;
  for (int i=0;i<5;i++)for (int j=0;j<5;j++)
    transitions[i][j]=this->o_transitions[i][j];
  for (int i=0;i<2;i++)for (int j=0;j<5;j++)
    weather[i][j]=this->o_weather[i][j];
  pcPotHab=this->o_pcPotHab;
  pcInitPop=this->o_pcInitPop;
  patchDecreaseRate=this->o_patchDecreaseRate;
  disturbanceRate=this->o_disturbanceRate;
  dispersed=this->o_dispersed; //Anteil verbreiteter Samen
  compAbil=this->o_compAbil;  //competetive ability high:+x neutral:0 low:-x
  manMode=this->o_manMode;
  manEffect=this->o_manEffect;
  for (int i=0;i<5;i++)
    man[i]=this->o_man[i];
}

//---File exchange----------------------------
///  help function to read preferences files
short Preferences::versionInit(const char*  ifile)
{
  //static unsigned long filePos=0;
  short stop=1;
  while (stop > 0)
  {
	stop =getPrefs(ifile,"next");
  }
  if (stop==-1) std::cout<<"no more preferences"<<endl;
  //run=0;
  return stop;
}// end versionInit
//------------------------------------------------------------------------
/**
  \brief add a set of preferences to a file
*/
void Preferences::addPrefs(const char* ofile)
{
  //hier Dateiausgabe der Parameterkombinationen
   //const char* ziel= locOFile.c_str();
   ofstream pkfile(ofile,ios::app);
   if(!pkfile){//Öffnen ist gescheitert =>Fehlerbehandlung
     cout<<"Die PK-Ausgabedatei konnte nicht erfolgreich"
                            " geöffnet werden."<<endl;    exit(3);
   }
   pkfile<< Preferences::ID<<'\t'<<this->maxYears<<'\t'<<this->maxRuns<<'\t'
          << 1<<'\t'<<this->o_pcInitPop<<'\t'
          <<this->o_disturbanceRate<<'\t'<<this->o_patchDecreaseRate<<'\t'
          <<this->o_compAbil <<'\t'<<setprecision(4)<<this->o_dispersed<<'\t';
   //---Einfluss von Management einfügen    (ab V2.02)
   pkfile<<this->o_manMode<<'\t';
   for(int i=0;i<5;i++) pkfile<<getOMan((process)i)<<'\t';
   //Umweltparameter
   for (int x=0; x<2; x++) for (int y=0; y<5; y++)
       pkfile<< this->o_weather[x][y]<<'\t';

   //Transitions
   for (int x=0; x<5; x++)  for (int y=0; y<5; y++) {
     pkfile<<setprecision(4)<< this->o_transitions[y][x];
     if (x+y<8) pkfile<<'\t';   //damit am Ende kein tab
   }
   pkfile<<endl;
}

//------------------------------------------------------------------------

///read Preferences from file
short Preferences::getPrefs(const char* iFile, string method){
  //liest Preferences aus Datei
  ifstream preferences(iFile);
  static unsigned long fpos=0;// filePos;
  if (!preferences){
     cout<<"could not open IFile: "<<iFile<<endl;exit(3);}
  if (method=="next"){
    preferences.seekg(fpos);
  }
  //if (method=="last")
  else{
    /// \todo set filepos at beginning of last line
    int counter1=0,counter2=0,counter3=0;
    bool eofile=preferences.eof();
    while (!preferences.eof()){
          counter1=counter2;
          //counter2=counter3;
          counter2=preferences.tellg();
          preferences.ignore(10000,'\n');
    }
    preferences.seekg(counter1-1);
  }
    preferences.clear();
    char comment;
    //cout<<preferences.tellg()<<"_";
    comment=preferences.peek();
    cout<<comment<<"..\n";
    if (comment == '#')
    { // Kommentarzeile lesen und Übergehen
  	   const int maxLengthOfLine = 10000; // max length of line = 20 char * 52 params
	   char temp[maxLengthOfLine];
	   preferences.getline(temp,maxLengthOfLine);
	   fpos = (unsigned int) preferences.tellg() -2;
	   preferences.close();
  	   return 1;
    }
    else
    { // Parameter einlesen
    //---Versions-ID ausgeben---
    preferences >> ID;cout<<"ID:"<<ID<<"  ";

    if (preferences.eof()) {preferences.close();
    fpos=0; return -1; }
    //---Laufzeit und # Wiederholungen einlesen---
    preferences >> maxYears >> maxRuns;
//cout<<"MaxRuns: "<<maxRuns<<"("<<preferences.tellg();
    //---Anteile potentieller und davon initial besetzter Habitate einlesen---
    float potHab_dummi;
    preferences >> potHab_dummi>> pcInitPop;
  //pcPotHab from this source obsolete at the moment (defined in lc-grid-file)
  //---Sukzessions- und Störungsrate einlesen---
    preferences >> disturbanceRate >> patchDecreaseRate;
  //---Competetive Ability einlesen---
    preferences >> compAbil;
  //---Anteil der weiter verbreiteten Samen einlesen---
    preferences >> dispersed;cout<<"disp: "<<dispersed;
  //---Einfluss von Management einfügen    (ab V2.02)
  /// \date 14.2.08 changed from eMode
    float mode=0;preferences >> mode;manMode= mode;
  cout<<"manmode"<<manMode;
  //double mE=1; preferences >> mE;
  //manEffect=mE;
  //-neue Zeile...
    float value=0;
    for(int i=0;i<5;i++) {
      preferences >> value;cout<<value<<" \t";
      setMan((process)i,value);
    }
//cout<<"after modes: "<<preferences.tellg();
  //---Matrix der Umweltschwankungen einlesen (ab V 0.09)---
cout<<"goodJs: ";
    for(int i=0;i<5;i++) {  //gute Jahre
      preferences >> value;cout<<value<<" \t";
      setPcQuality(GOOD,(process)i,value);
    } //-neue Zeile
    cout<<"badJs: ";
    for(int i=0;i<5;i++) {   //schlechte Jahre
      preferences >> value;cout<<value<<" \t";
      setPcQuality(BAD,(process)i,value);
    }
  //-neue Zeile...
  //---Übergangsmatrix einlesen---
//cout<<"transits: "<<preferences.tellg()<<endl;
    float transit[5][5];
    for (int i=SEED; i<=GEN; i++)  for (int j=SEED; j<=GEN; j++) {
      preferences >> transit[j][i];
    }
    cout<<"last trans: "<<transit[4][4];
    setTransitions(transit);
  //---Ende des Datensatzes---
    if (preferences.good()) {
 //     comment= preferences.peek();
      fpos=(unsigned int)preferences.tellg();//fpos-=1;
    }else {fpos=0;return -1;}
  }//end ifelse '#'
  cout<<fpos<<endl;
  //preferences.close();
  return 0;
}

//void addPrefs(const char *file){}

//---communicate with private data-------------------------
///\name communicate with private data
//@{
void Preferences::setTransitions(float matrix[5][5]){
  for (int i=SEED; i<=GEN; i++)  for (int j=SEED; j<=GEN; j++) {
    transitions[j][i]=matrix[j][i];
  }
}

float Preferences::get_p1()const{
        float e1=getOTransition(SEEDL,JUV);
        return e1/(1-get_p2());
}
float Preferences::get_p2()const{
        float e1=getOTransition(SEEDL,JUV), e2=getOTransition(SEEDL,VEG), e3=getOTransition(SEEDL,GEN);
        return (e2+e3)/(e1+e2+e3);
}
float Preferences::get_p3()const{
        float e3=getOTransition(SEEDL,GEN);
        return e3/(get_p1()*get_p2());
}
float Preferences::get_gr1()const{
        float e1=getOTransition(JUV,VEG), e2=getOTransition(JUV,GEN);
        return e1+e2;
}
float Preferences::get_gr2()const{
        float e2=getOTransition(JUV,GEN);
        return e2/get_gr1();
}
int Preferences::set_p1(float v){
cout<<"    p1: e1="<<o_transitions[SEEDL][JUV]<<" e2="<<o_transitions[SEEDL][VEG]<<" e3=" <<o_transitions[SEEDL][GEN]<<endl;
    float p1=v,p2=get_p2(),p3=get_p3();
    if (p1*(1-p1)<=0) return -1;//0<p<1
    float e1=p1*(1-p2);float e2=p1*p2*(1-p3);float e3=p1*p2*p3;
    o_transitions[SEEDL][JUV]=e1;o_transitions[SEEDL][VEG]=e2;o_transitions[SEEDL][GEN]=e3;
cout<<"new p1: e1="<<o_transitions[SEEDL][JUV]<<" e2="<<o_transitions[SEEDL][VEG]<<" e3=" <<o_transitions[SEEDL][GEN]<<endl;
    return 0;
}
int Preferences::set_p2(float v){
cout<<"    p2: e2="<<o_transitions[SEEDL][VEG]<<" e3=" <<o_transitions[SEEDL][GEN]<<endl;
    float p1=get_p1(),p2=v,p3=get_p3();
    if (p2*(1-p2)<=0) return -1;//0<p<1
    float e1=p1*(1-p2);float e2=p1*p2*(1-p3);float e3=p1*p2*p3;
    o_transitions[SEEDL][JUV]=e1;o_transitions[SEEDL][VEG]=e2;o_transitions[SEEDL][GEN]=e3;
cout<<"new p2: e2="<<o_transitions[SEEDL][VEG]<<" e3=" <<o_transitions[SEEDL][GEN]<<endl;
    return 0;
}
int Preferences::set_p3(float v){
cout<<" p3: e2="<<o_transitions[SEEDL][VEG]<<" e3=" <<o_transitions[SEEDL][GEN]<<endl;
    float p1=get_p1(),p3=v,p2=get_p2();
    if (p3*(1-p3)<=0) return -1;//0<p<1
    float e2=p1*p2*(1-p3);float e3=p1*p2*p3;
    o_transitions[SEEDL][VEG]=e2;o_transitions[SEEDL][GEN]=e3;
cout<<"new p3: e2="<<o_transitions[SEEDL][VEG]<<" e3=" <<o_transitions[SEEDL][GEN]<<endl;
    return 0;
}
int Preferences::set_gr1(float v){
cout<<"    gr1: e1="<<o_transitions[JUV][VEG]<<" e2=" <<o_transitions[JUV][GEN]<<endl;
    float gr2=get_gr2(),gr1=v;
    if (v*(1-v)<=0) return -1;//0<p<1
    float e1=gr1*(1-gr2);float e2=gr1*gr2;
    o_transitions[JUV][VEG]=e1;o_transitions[JUV][GEN]=e2;
cout<<"new gr1: e1="<<o_transitions[JUV][VEG]<<" e2=" <<o_transitions[JUV][GEN]<<endl;
    return 0;
    }
int Preferences::set_gr2(float v){
cout<<"    gr2: e1="<<o_transitions[JUV][VEG]<<" e2=" <<o_transitions[JUV][GEN]<<endl;
    float gr1=get_gr1(),gr2=v;
    if (v*(1-v)<=0) return -1;//0<p<1
    float e1=gr1*(1-gr2);float e2=gr1*gr2;
    o_transitions[JUV][VEG]=e1;o_transitions[JUV][GEN]=e2;
cout<<"new gr2: e1="<<o_transitions[JUV][VEG]<<" e2=" <<o_transitions[JUV][GEN]<<endl;
    return 0;
}

/**
  Function sets single object transition rates.
  The value is reduced if establishment or growth of a state would exceed 1.
  \date 24.04.08
  \date 10.10.08
  changed, such as unvalid values are ignored, not recalculated
  \param from the stage of origin
  \param to the stage to go to
  \param value the new value
*/
void  Preferences::setOTransition(state from,state to,float value){
  cout<<from<<";"<<to<<": "<<getOTransition(from,to)<<"-->"<<value;
  if (from==SEEDL&&to>SEEDL){//sum of all estab must be less than one
    float diff=getOTransition(from,to)-value;
    if (to==JUV) {set_p1(get_p1()+diff);}
    else if (to==VEG) {set_p2(get_p2()+diff);}
    else {set_p3(get_p3()+diff);}
  }
  else if(from==JUV&&to>JUV){
    float diff=getOTransition(from,to)-value;
    if (to==VEG) {set_gr1(get_gr1()+value);}
    else  {set_gr2(get_gr2()+value);}
  }
  else{
     o_transitions[from][to]=std::max((float)0.0,value);
     cout<<">"<<value<<endl;
  }
}
void Preferences::changeOTransition(state from,state to,float min,float max,float value){
  cout<<from<<";"<<to<<": "<<getOTransition(from,to)<<"--> change "<<value;
  if (from==SEEDL&&to>SEEDL){//sum of all estab must be less than one
    if (to==JUV) {set_p1(get_p1()+value);}
    else if (to==VEG) {set_p2(get_p2()+value);}
    else {set_p3(get_p3()+value);}
  }
  else if(from==JUV&&to>JUV){
    if (to==VEG) {set_gr1(get_gr1()+value);}
    else  {set_gr2(get_gr2()+value);}
  }
  else{
     float new_val=o_transitions[from][to]+value;
     if  (new_val<=min||new_val>=max) return;
     o_transitions[from][to]=new_val;
     cout<<">"<<new_val<<endl;
  }
    }

float Preferences::getTransition(state from,state to){
  return transitions[from][to];
}
float Preferences::getOTransition(state from,state to)const{
  return o_transitions[from][to];
}

int   Preferences::setPcQuality(gb mode,process p,float value){
  //setzt die Werte für 'Wahrscheinlichkeit für gutes Jahr'
    // und '.. für schlechtes ..'
    weather[mode][p]=value;
    if ((value<0) || (value>1) || ((weather[0][p]+weather[1][p])>1))  {
      //Application->MessageBox("Falsche Wetter-Vorgaben!!","Fehler",MB_OK);
      cout<<"Falsche Wetter-Vorgaben!!\n";
      return -1;
    }
    return 0 ;
}
void  Preferences::setOPcQuality(gb mode,process p,float value){
  o_weather[mode][p]=value;
}
float Preferences::getPcQuality(gb mode,process p){
  return weather[mode][p];
}
float Preferences::getOPcQuality(gb mode,process p){
  return o_weather[mode][p];
}

///management
void  Preferences::setMan(process p,float value){
     man[p]=value;
}
void  Preferences::setOMan(process p,float value){
     o_man[p]=value;
}
float Preferences::getMan(process p){
  return man[p];
}
float Preferences::getOMan(process p){
  return o_man[p];
}
//@}
//---------------------------------------------------------------------------

#pragma package(smart_init)
