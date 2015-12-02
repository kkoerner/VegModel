/**  \file
\brief Implementation of classes Landscape, Climate and Feld

Deklarationen in der Headerdatei:

\author Katrin Körner
\date 15.3.06
- ausmisten
*/

//#pragma hdrstop
#include <fstream>
#include <math.h>
#include <iostream>
//#include <cstdlib>
//#include "meta.h"
#include "prefs.h"
#include "mresults.h"
//#include "species.h" //in header included
#include "mclasses.h"
#include "random.h"

using namespace std;
//vector<Feld> lc_muster(0,Feld());
 //----------- Definition statischer Variablen  -Default-werte-veraltet
  float Climate::actYear[6]={0,0,0,0,0,0};
//--class Climate---------------------------------------------------
/**
  calcWeatherInit berechnet das jährliche Wetter zur Init-Zeit. Die
  Wahrscheinlichkeiten für gutes und schlechtes Wetter sind hier auf
  Standardwerte festgelegt (20% für alle)
  \sa Climate::calcWeather
*/
double Climate::calcWeatherInit(){
    float mYear=0.0;
    float gP[]={0.2,0,0.2,0,0.2};
    float bP[]={0.2,0,0.2,0,0.2};

    int maxx= Preferences::MaxX; int maxy= Preferences::MaxY;
    int length=maxx*maxy;
   // regWeather.assign(length,0.0);
    /// \todo später: reg Wetter einlesen

    //Berechnung ActYear:: gut:1.5  mittel:1.0  schlecht:0.5
    bool weather_autocorrelated = false; //<- Wetter für alle Prozesse im Jahr gleich? stdrd=False
    double wert=rand()/(double)RAND_MAX;
    for(int i=0; i<5; i+=2){
      if (!weather_autocorrelated) wert=rand()/(double)RAND_MAX;
      if      (wert<gP[i])      actYear[i]=1.5;
      else if (wert>=1.0-bP[i]) actYear[i]=0.5;
      else                      actYear[i]=1.0;
      mYear+=actYear[i];
    }
    actYear[5]= mYear/3.0;
    return actYear[5]; //Rückgabe: mittlere Qualität des Jahres
  }

/**
  CalcWeather berechnet das jährliche Wetter. Für potentiell fünf Prozesse
  werden Zufallswerte gezogen, nach denen entschieden wird, ob ein Jahr
  gut(1.5), mittel(1.0) oder schlecht(0.5) für den jeweiligen Prozess ist.

  Alternativ kann <TT>weather_autocorrelated = true</TT> gesetzt werden,
  womit ein Jahr für alle Prozesse den selben Qualitätswert bekommt.

  \return{ mean year's quality}

  \todo{regionale 'Wetterkarte' einlesen (fractal clouds); stdrd: weather
         spatially fully autocorrelated}
*/

double Climate::calcWeather(){
    float mYear=0.0;

    int maxx= Preferences::MaxX; int maxy= Preferences::MaxY;
    int length=maxx*maxy;
    //regWeather.assign(length,0.0);
    /// \todo später: reg Wetter einlesen

    //Berechnung ActYear:: gut:1.5  mittel:1.0  schlecht:0.5
    bool weather_autocorrelated = false; //<- Wetter für alle Prozesse im Jahr gleich? stdrd=False
    double wert=rand()/(double)RAND_MAX;
    for(int i=0; i<5; i+=2){
      if (!weather_autocorrelated) wert=rand()/(double)RAND_MAX;
      if(wert<Preferences::getPcQuality((gb)0,(process)i)) actYear[i]=1.5;
      else if(wert>=1.0-Preferences::getPcQuality((gb)1,(process)i)) actYear[i]=0.5;
      else actYear[i]=1.0;
      mYear+=actYear[i];
    }
    actYear[5]= mYear/5.0;
    return actYear[5]; //Rückgabe: mittlere Qualität des Jahres
  }

/**
  gibt das regionale Wetter bezüglich der Position im Grid zurück

  \todo include spatial position in grid for spatial heterogeneities
  in weather

*/
double Climate::getWeather(int param,int index){return actYear[param];}
//---------------------------------------------------------------------
//--class Landscape-------------------------------------------------

Landscape::Landscape(void){
    clima = new Climate();
    setSize(Preferences::MaxX,Preferences::MaxY);
//    int length=Preferences::MaxX*Preferences::MaxY;
////    lc.assign(length,Feld());
//    pop.assign(length,Population());
//    initVerteilung(10000); //former:2000
}
Landscape::~Landscape(){
  delete clima;
  pop.clear();
}

void Landscape::setSize(int x, int y){
     pop.clear();
     pop.assign(x*y,Population());
     initVerteilung(10000);
}

//@{
/// Umrechnungsfunktionen des Index in Koordinaten und umgekehrt
inline int getIndex(int x,int y){
  if((x<0)||(y<0)||(x>=Preferences::MaxX)||(y>=Preferences::MaxY))return -1;
  else return Preferences::MaxY*x+y;}
inline int getX(int index){return index/Preferences::MaxY;}
inline int getY(int index){return index%Preferences::MaxY;}
//@}
//-----------------------------------------

/// get MaxCap for different HGUGs
/**

  \date V080813
  \param value the value to be interpreted (HGUG or nodata)
  \param method one of either 'binary' or 'weight'
  \return maximum capacity for the value given
*/
float ValueToMaxCap(float value, int nodata, char* method){//"binary" or "weight"
  if (method=="binary" ) {
    if (value==nodata) return 0;
    else return 50000;
  }
  if (method=="weight"){
    switch((int)value){
    case  913:
    case  914:
    case  513:
    case  915:
      return 50000;
    case  515:
    case 1011:
    case 1015:
      return 40000;
    case  512:
    case  514:
    case 1214:
    case 1215:
    case 1123:
      return 30000;
    case  511:
    case  826:
    case 1010:
    case  828:
    case 1016:
    case 1000:
      return 20000;
    case 1212:
    case  711:
    case  710:
    case  713:
    case  717:
    case  719:
    case  714:
    case 1213:
    case 1224:
    case  718:
    case  824:
    case  715:
      return 10000;
    default:return 0;
    }
//    return 1;
  }
  return -1;
}

//------------------------------------------------------------------------
/**
  Reads habitat information from a file given in Results::iFileLandscape.

  For gridfiles (*.gtg extension, ASCII coded) randomly chooses one of the
  first 100 grids in the gridfile to use for the current landscape.
  Values are interpreted as maximum Capacities.

  For ArcInfo raster data (*.aig extension, ASCII coded) interpretes world file
  and loads habitat information. Values are german habitat codes and interpreted
  in function ValueToMaxCap.

  The function stores the resulting raster in Preferences::MusterCapacity.

  \date V080814
  \param file landscape file to be loaded
*/
void Landscape::loadLandscape(const char* file){
  cout<<"loading landscape file ...";
  ifstream eingabe(file);
  if(!eingabe){
    //Öffnen ist gescheitert =>Fehlerbehandlung
    //Application->MessageBox("Die Griddatei konnte nicht erfolgreich"
    //                        " geöffnet werden.",NULL,MB_OK);
    std::cout<<"Die Griddatei konnte nicht erfolgreich"
        <<" geöffnet werden.\n";
//    system("PAUSE");
    exit(3);
  }

  string file_id=((string) file).substr(((string) file).length() -3,3);
  if (file_id=="gtg"){
    int noGrids=100;
    //!for current (Jnc-)versions *.gtg ever contain
    //! (at least) 100 landscapes
    //eingabe>> noGrids;
    int grid=int(MRandom::rnd01()*noGrids);//cout<<"GridID: "<<grid<<endl;//
    for (int i=0; i<grid; i++){   //Results::ir+1
      eingabe.ignore(1000000,'\n');
      if (eingabe.eof()){
        //Application->MessageBox("Number existiert nicht in Griddatei",NULL,MB_OK);
        cout<<"Nummer existiert nicht in Griddatei\n";
        exit(3);
      }
    }
    int xmax, ymax;
    eingabe >>Preferences::dimension>>Preferences::pcPotHab>> xmax >>ymax;
    Preferences::MusterCapacity.assign(xmax*ymax,-1);
    for (int index=0; index<(xmax*ymax); index++){
      float value=0;
      eingabe >> value;
            //pop[index].maxCapacity=value*10; //ab V071120
      if (value>0) Preferences::MusterCapacity[index] =5000;
      else Preferences::MusterCapacity[index]=0; //ab V080422
    }
  }// end if 'gtg'
  else if (file_id=="aig"){//aig is arc-info grid
    //get world file data:
    int xmax, ymax, nodata;
    eingabe.ignore(20,' ');eingabe>>ymax;eingabe.ignore(2,'\n');
    eingabe.ignore(20,' ');eingabe>>xmax;eingabe.ignore(2,'\n');
    for (int i=0; i<3; i++) eingabe.ignore(2000,'\n');
    eingabe.ignore(20,' ');eingabe>>nodata;eingabe.ignore(2,'\n');
    Preferences::MaxX=xmax;Preferences::MaxY=ymax;
    Preferences::MusterCapacity.assign(xmax*ymax,-1);

    //get grid data:
    for (int index=0; index<xmax*ymax; index++) {
      float value=0;
      eingabe >> value;
      Preferences::MusterCapacity[index]=value;//ValueToMaxCap(value,nodata,"weight");
//     //..for testing get only first half of landscape
//      if (index>(xmax*ymax/4.0)) Preferences::MusterCapacity[index]=0;
    }
  }// end if 'aig'
  cout<<"done"<<endl;
}// end function loadLandscape
//------------------------------------------------------------------------
/**
  Function loads file that defines the cells to analyse in
    Year::yearly_selcells().

  The elements in vector Preferences::MusterResultCells get true if the value
    is 1 elsewhere the elements is set false.
  The spatial scale of the file is compared to that of the underlying landscape.

  \date V080820
  \param file landscape file to be loaded
*/
void Landscape::loadResultCells(const char* file){
  cout<<"loading ResultCell file ...";
  ifstream eingabe(file);
  if(!eingabe){
    //Öffnen ist gescheitert =>Fehlerbehandlung
    std::cout<<"Die Griddatei konnte nicht erfolgreich"
        <<" geöffnet werden.\n";
    exit(3);
  }
    ///get world file data
    int xmax, ymax, nodata;
    eingabe.ignore(20,' ');eingabe>>ymax;eingabe.ignore(2,'\n');
    eingabe.ignore(20,' ');eingabe>>xmax;eingabe.ignore(2,'\n');
    for (int i=0; i<3; i++) eingabe.ignore(2000,'\n');
    eingabe.ignore(20,' ');eingabe>>nodata;eingabe.ignore(2,'\n');
    ///test for consistency
    if (Preferences::MaxX!=xmax||Preferences::MaxY!=ymax)
      cout<<"loadResultCells: falsche Zellgröße\n";
    Preferences::MusterResultCells.assign(xmax*ymax,-1);

    ///get grid data
    for (int index=0; index<xmax*ymax; index++) {
      float value=0;
      eingabe >> value;
      if (value==0) value=1;
      if (value<1) value=0;
      Preferences::MusterResultCells[index]=value;
    }
  cout<<"done"<<endl;
}// end function loadResultCells

//------------------------------------------------------------------------------
///initiate Landscape und Populations
/**
 The information of maximum capacity is copied from Preferences::Muster.

 A constant proportion (Preferences::pcInitPop) of
  potential suitable habitats gets an initial population of 10 to 50
  adult individuals (uniformly distributed).
  \date 080909 poisson distributed number of individual seeds with mean=10 are set
*/
void Landscape::setPopulations(){ // char* file, int number
   int max=Preferences::MaxX * Preferences::MaxY;
   for (int index=0; index<max; index++){
      /// Übernehme realistic Landscape
      pop[index].maxCapacity=ValueToMaxCap(Preferences::MusterCapacity[index],9999,"weight");
      pop[index].actCapacity=pop[index].maxCapacity*0.99;//ab V080422
      //davon zufällig mit 10-50 Individuen bestücken ..ab V071121
      if(MRandom::rnd01()<Preferences::pcInitPop){ //&&
//                        isArableField( (int) Preferences::MusterCapacity[index])){
//        int nmb=(int)MRandom::randomNb->poisson(100); //rnd_range(10,50);
        int nmb=10000;(int)MRandom::randomNb->poisson(100); //rnd_range(10,50);
        pop[index].addSeeds(nmb,0);//resetInds(nmb);//int(pop[index].actCapacity/10.0));
      }
   }
}// end function setPopulations()

//-----------------------------------------

/// get Disturbance Rate for different HGUGs
/**

  \date V081006
  \param value the value to be interpreted (HGUG or nodata)
  \return disturbance rate for the value given
*/
float ValueToDisturbanceRate(float value, int nodata=9999){
    switch((int)value){
    case  913:
    case  914:
    case  915:
      return 0.95;//0.95;//arable field
    case 1010:
    case 1011:
    case 1015:
      return 0.9;//gardens - Gärten
    case 0511:
    case 0717:
      return 0.1;//moist meadows - Frischwiesen
    case 1214:
    case 1215:
      return 0.4;//anthopog Sonderflächen
    case  826:
    case  828:
      return 0.05;//Rodung, Aufforstung
    case 1212:
    case 1016:
    case 1024:
      return 0.2;//Siedlung
    case  711:
    case  710:
    case  715:
      return 0.05;//Feldgehölze
    case  718:
    case  719:
    case  714:
      return 0.05;//?Baumreihen
    case  513:
    case 1000:
      return 0.6; //Ruderalflächen(graslandartig)
    case  515:return 0.3;//Intensivgrasland
    case  512:return 0.1; //Trockenrasen
    case  514:return 0.05;//Staudenfluren feucht bis nass
    case  713:return 0.05;//Hecken
    case 1213:return 1.0;//Verkehrsanlagen
    case  824:return 0.01;//Baumplantagen
    case 1123:return 0.01;//off. Rieselfelder
    default:return 0;
    }
}
//-----------------------------------------
///calculate dynamic landscape
/**
  lässt die aktuelle Kapazität der Patches dank einer Sukzession
  sinken (logistisch) and generates new disturbed areas

  at initialization time disturbance rate and manmode are constant parameters
  - competitive ability is set to 0
  - disturbance rate is set to 0.05 (or once in 20years)
  - management mode is set to managed (compensing)
  \date 2008/10/01 init obsolete
  \param init obsolete
  \param method possible methods are 1) random, and 2) habitat
*/
void Landscape::patchDynamics( char* method,bool init){
  float Rk=Preferences::patchDecreaseRate;   //Patch growth rate
  float cA=Preferences::compAbil;
  float dR=Preferences::disturbanceRate;//global disturbance rate
  float mode=Preferences::manMode;//eMode mode;
  /// \todo define init time with standard values; now file-given values are used for all the time
 // if (init){
  //cA=0;
 // dR=0.75;//mode=COMPENSE;
 // }//else{
 //   cA=Preferences::compAbil;
 //   dR=Preferences::disturbanceRate;
 //   mode= Preferences::manMode;
 // }

  float Ki=0;      //actual Capacity
  //gehe durch alle Felder..
  for(int x=0;x<pop.size();x++){
    pop[x].managed=false;
    pop[x].disturbed=false;

    //wenn pot. geeignet..
    float d=pop[x].maxCapacity ;
    if(d>0){
      //cout<<"SiteID"<< x<<" Cap: "<<d<<endl;

      //get site-specific disturbance rate
      if (method=="habitat") dR=ValueToDisturbanceRate(Preferences::MusterCapacity[x]);
      double dummi=MRandom::rnd01(); //zufallswert(0-1)
      if(dummi<dR){
        //setzte Kapazität hoch und lösche Population aus
        pop[x].actCapacity=d*0.99;
        pop[x].resetInds(0);
        //document event
        pop[x].disturbed=true;
      }else{ //sonst lasse Sukzession fortschreiten  (incl.Management ab V2.02)
        Ki=pop[x].actCapacity;
        //float amount=Rk*Ki*(1-Ki/d) ;
        float amount=Rk*Ki*(1-Ki/d) ;//logistic form

          //Managementeinfluss  ab V2.02
        /// \date 12.2.08 changed management: probability of management, if yes: no additional succession
        if (MRandom::rnd01()<mode) {amount=0;pop[x].managed=true;}
//        switch (mode) {
//          case COMPENSE:amount=0;break;
//          case OVERCOMP:amount*=-0.5;break;
//          default:break;  //no management
//        }
          //Korrekturfaktor: Konkurrenzkraft der Art
        pop[x].actCapacity-=amount/(1.0+cA);
        //negativ logistisches Wachstum
        if(pop[x].actCapacity<1) pop[x].actCapacity=0;
        //max-wert nicht überschreiten
        if(pop[x].actCapacity>d)
           pop[x].actCapacity=d;

      }//ende Sukzession

    }//ende wenn pot Habitat
  }//ende aktuelles Feld
}
//-----------------------------------------
void Landscape::Disturb(unsigned i){
   //setzte Kapazität hoch und lösche Population aus
   pop[i].actCapacity=pop[i].maxCapacity*0.99;
   pop[i].resetInds(0);
   //document event
   pop[i].disturbed=true;
    }
void Landscape::Success(unsigned i){
  float Rk=Preferences::patchDecreaseRate;   //Patch growth rate
  float cA=Preferences::compAbil;
  float d=pop[i].maxCapacity ;
  float Ki=pop[i].actCapacity;      //actual Capacity
  //float amount=Rk*Ki*(1-Ki/d) ;
  float amount=Rk*Ki*(1-Ki/d) ;//logistic form

  //Managementeinfluss  ab V2.02
  /// \date 12.2.08 changed management: probability of management, if yes: no additional succession
//  if (MRandom::rnd01()<mode) {amount=0;pop[i].managed=true;}
  //Korrekturfaktor: Konkurrenzkraft der Art
  pop[i].actCapacity-=amount/(1.0+cA);
  //negativ logistisches Wachstum
  if(pop[i].actCapacity<1) pop[i].actCapacity=0;
  //max-wert nicht überschreiten
  if(pop[i].actCapacity>d)
  pop[i].actCapacity=d;
    }

/**
  recursive function to search for cells of single patches
  \sa Disturb()
  \sa Succession()
*/
void Landscape::NextInPatch(unsigned int i,bool cDist,float habType,vector<bool>& lcheck){
  //cout<<"test i="<<i<<" i*="<<getIndex(getX(i),getY(i))<<"  ";
  //cout<<"test maxX:"<<Preferences::MaxX<<" maxY:"<<Preferences::MaxY<<"  ";
//  cout<<getX(i)<<";"<<getY(i)<<"\t";
  if(cDist)
     Disturb(i);
  else
     Success(i);
  lcheck[i]=true;
  int ni;
  //search south
  ni=getIndex(getX(i)+1,getY(i));
  if (ni!=-1&&Preferences::MusterCapacity[ni]==habType && lcheck[ni]==false)
      NextInPatch(ni,cDist,habType, lcheck);
  //search east
  ni=getIndex(getX(i),getY(i)+1);
  if (ni!=-1&&Preferences::MusterCapacity[ni]==habType && lcheck[ni]==false)
      NextInPatch(ni,cDist,habType, lcheck);
  //search north
  ni=getIndex(getX(i)-1,getY(i));
  if (ni!=-1&&Preferences::MusterCapacity[ni]==habType && lcheck[ni]==false)
      NextInPatch(ni,cDist,habType, lcheck);
  //search west
  ni=getIndex(getX(i),getY(i)-1);
  if (ni!=-1&&Preferences::MusterCapacity[ni]==habType && lcheck[ni]==false)
      NextInPatch(ni,cDist,habType, lcheck);
    }

///calculate dynamic landscape with autocorrelated disturbances on patch scale
/**
  lässt die aktuelle Kapazität der Patches dank einer Sukzession
  sinken (logistisch) and generates new disturbed areas

  Disturbance events sind autokorreliert auf Patchebene (keine diagonalen Verbindungen).
  Patches werden rekursiv "ertastet".
  \date 2008/11/17
  \sa Landscape::NextInPatch
*/
void Landscape::patchDynamics_corr(bool disturb){
  float dR;//global disturbance rate
  vector<bool> check=vector<bool>(pop.size(),false);
  for(unsigned x=0;x<pop.size();x++){
    pop[x].managed=false;
    pop[x].disturbed=false;
  }
  //gehe durch alle Felder..
  for(unsigned x=0;x<pop.size();x++){
    //wenn pot. geeignet..
    if(pop[x].maxCapacity>0&&check[x]==false){
      //cout<<"SiteID"<< x<<" Cap: "<<d<<endl;
      //get site-specific disturbance rate
      dR=ValueToDisturbanceRate(Preferences::MusterCapacity[x]);
      float dummi=MRandom::rnd01(); //zufallswert(0-1)
//cout<<"Type "<<Preferences::MusterCapacity[x];
//if (dummi<dR) cout<<"D "; else cout<<"S ";cout<<dR<<endl;
if (disturb){      NextInPatch(x,dummi<dR,Preferences::MusterCapacity[x],check);}//normal runs
else{      NextInPatch(x,false,Preferences::MusterCapacity[x],check);}           //runs without disturbances
// cout<<"end Patch\n";
    }//ende wenn pot Habitat
  }//ende aktuelles Feld
}//end PatchDynamics_corr
//----------------------------------------------------------------------
/** ...core function to simulate one Year.
    Flow of simulation is as follows:
    -# calculate patch dynamics (patchDynamics())
    -# get this Years Weather qualities (calcWeather())
    -# for each (potentially suitable) grid cell (i.e. its population) do the following:
      - age all existing Individuals
      - calculate retrogression of generativ adults to vegetative state
      - calculate germination and establishment (germEstab())
      - calc. seed production of local population (seedProduction())
      - let clones grow laterally (clonalGrowth())
      - let some Individuals die (mortality())
      - let some seeds die (seedMort())
    -# disperse Seeds produced this year
*/
void Landscape::oneYear(bool init,bool disturbFlag){
  ///if init-time
  if (init){
    patchDynamics_corr(disturbFlag);//("habitat",true);  //it's init time
    clima->calcWeatherInit();
  }else{
    patchDynamics_corr(disturbFlag);//("habitat");
    clima->calcWeather();
  }
  int size=pop.size();
//cout<<" c";//most cumputing time is consumed here
  //für alle Felder im Raster
  for(int x=0;x<size;x++){
    //Wie ist das Wetter?
    pop[x].resetIndsReg(); //Zwischenspeicher zurücksetzen
    //für alle potentiell geeigneten Patches
    if(pop[x].maxCapacity>=1){
       //if (x%100==0) cout<<".";//"site: "<<x<<endl;
       // Altern (wetterwert nicht benutzt)
       float dummi=clima->getWeather(4,x);
       pop[x].aging(dummi);
       //Retrogression (abh. von Wetter zur Samenreife)
       pop[x].retrogression(clima->getWeather(4,x)*clima->getWeather(2,x));
       //Keimung und Etablierung
       pop[x].germEstab(clima->getWeather(0,x));
       //Samenproduktion
       pop[x].seedProduction(clima->getWeather(2,x));
       //klonales Wachstum
       pop[x].clonalGrowth(clima->getWeather(2,x));
       //Mortalität
       float sukzession=pop[x].actCapacity/pop[x].maxCapacity;
       pop[x].mortality(sukzession,clima->getWeather(4,x));
       //Samenmortalität
       pop[x].seedMort();
    }else if(pop[x].getSeeds()>0){
       //lösche Samenbank in ungeeigneten Patches
       pop[x].addSeeds(-pop[x].getSeeds(),x);
    }
  }//ende aktuelles Feld
//cout<<"d";
  //Samenverteilung für jedes Samen produzierende Feld ..
  for(int x=0;x<pop.size();x++){
    if(pop[x].getSeedProd()>0){
      seedDispersal(x);
    }
  }
}
//---KW----------------------------------------------------------------------
/**
  berechnet ein langes Array mit Koordinaten bezüglich des K.-Ursprungs

  je näher das Feld dem Ursprung (0,0) desto häufiger (exponentiell) das Auftreten im Array
  die Koordinaten (0,0) werden ausgeschlossen

  ab Version 2.05: Basis 129*129 Zellen groß
*/
void Landscape::initVerteilung(int laenge){
  verteilung.xCoord.clear();verteilung.yCoord.clear();
  int z=0; int x,y;
  double dist,p;
  while(z<laenge){
    //Zufallskoordinaten in 21 X 21 Um-Feldern
    //ab V205: 129*129Felder
    //ab V080527: gesamtes Grid; freie gridgröße
    int xmax=Preferences::MaxX, ymax=Preferences::MaxY;
    x=rand()%xmax-xmax/2; y=rand()%ymax-ymax/2;
    dist=sqrt(x*x+y*y);         //Distanzberechnung
    p=7*exp(-2*dist);//pow(0.6,2*dist);
    //exp. mit Distanz abfallender Wahrscheinlichkeitswert (max. 94%)
    if(((MRandom::rnd01())<p)&&((x!=0)||(y!=0))){
      //ans Array anhängen..
      verteilung.xCoord.push_back(x);
      verteilung.yCoord.push_back(y);
      z++;
    }
  }//ende while
}

///long distance seed dispersal (LDD)
/**
  Annually produced seeds of a site (index) are dispersed to
  the mother population (1-Preferences::dispersed), and via LDD to neighbouring
  cells. For LDD a cell is chosen uniformly random from the list generated by
  function Landscape::initVerteilung. With respect to torus boundary conditions
  packets of seeds are added to the seed bank of the chosen cell. One packet
  contains two seeds minimum, and grows linearly if the number of seeds to
  disperse exceeds 2000.
  Packets are dispersed repeatedly until all seeds are gone.
  \param index mother site adress
*/
void Landscape::seedDispersal(int index){
  int paket=2;                 //Paketgröße
  long sprd=pop[index].getSeedProd();
  //Samenfall innerhalb der Population..
  long selbst=long(sprd*(1.0-Preferences::dispersed));
  pop[index].addSeeds(selbst,index);
  sprd-=selbst;  //restliche Samen ausserhalb verteilen..
  if (sprd/paket>1000) paket = sprd/1000;
  while(sprd>0){ //bis Samen alle: wähle zufällig Feld aus Array
    int zufeld=rand()%verteilung.xCoord.size();
    int wertx=verteilung.xCoord[zufeld]+getX(index);
    int werty=verteilung.yCoord[zufeld]+getY(index);
    if(sprd<paket) paket=sprd;
    bool weiter=false;
    /// Landscape with absorbing boundaries
    if(getIndex(wertx,werty)>=0)
      pop[getIndex(wertx,werty)].addSeeds(paket,0);//0 is dummy value

//
// //  Torus-Definition
//    while(!weiter){
//      int MaxX=Preferences::MaxX; int MaxY=Preferences::MaxY;
//      if(wertx>=MaxX) wertx-=MaxX; //Metapopulation als Torus ab V0.04
//      if(wertx<0) wertx+=MaxX;
//      if(werty>=MaxY) werty-=MaxY;
//      if(werty<0) werty+=MaxY;
//      if(getIndex(wertx,werty)>=0){
//        pop[getIndex(wertx,werty)].addSeeds(paket,getIndex(wertx,werty));
//        weiter=true;
//      }
//    }//while weiter

    sprd-=paket; //Paket verteilt
  } // Samen alle?
}
//-----------------------------------------------------------------------


