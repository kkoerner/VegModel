/**  \file
  \brief Implementation der Auswertungsklassen für das Metapopulationsmodell

  \author Katrin Körner \date 15.3.06
  - ausmisten
  \date 07.02.2008 - more documentation
*/
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include "mresults.h"

// initialization of static variables
//unsigned int Results::iv=0;
//unsigned int Results::ir=0;
//unsigned int Results::ia=0;
string Results::iFileLandscape="";
string Results::iFilePreferences="";
string Results::oFile="";
string Results::path="";
//---------------------------------------------------------------------------
Run::Run(){
        a=new vector<Year>(0);
        a->reserve(500);
        cellOccupied.reserve(Preferences::MaxX*Preferences::MaxY);
        timeToExt=0;keycode=0;
        dimension=Preferences::dimension;maxYears=Preferences::maxYears;
        sumOfInds=0;varSumOfInds=0;lambda=1;minOccupPops=5;occupPops=0;
        indsPerPop=0;varIndsPerPop=0;turnOver=0;turnOverRate=0;
        varExtRate=0;varColRate=0;extRate=0;colRate=0; fluxRate=0;
//cout<<"new Run().\n";
  };
//---------------------------------------------------------------------------
Run::~Run(){
    if (a) delete a; a=NULL;
 //   cout<<"del Run.\n";
}
//---------------------------------------------------------------------------
Run::Run(const Run& x){
  a= new vector<Year>(0);                    ///< vector of years
  for (int i=0;i<x.a->size();i++) a->push_back(x.a->at(i));                    ///< vector of years
  timeToExt=a->size();//x.timeToExt;                     ///< time to extinction
  maxYears=x.maxYears;                      ///< maximum number of years to simulate
  keycode=x.keycode;                       ///< sample site for documentation (obsolete)
  dimension=x.dimension;                  ///< fractal dimension of LC (obsolete)
  cellOccupied=x.cellOccupied;         ///<Belegungsmuster des Grids
  //Output Values
  sumOfInds=x.sumOfInds;             ///< O-Var: msuminds
  lambda=x.lambda;
  minOccupPops=x.minOccupPops;             ///< O-Var: minbes
  occupPops=x.occupPops;                ///< O-Var: aoccup
  indsPerPop=x.indsPerPop;               ///< O-Var: mIndsPerPop
  extRate=x.extRate;
  colRate=x.colRate;          ///< O-Var: extRate,colRate
  turnOver=x.turnOver;
  turnOverRate=x.turnOverRate;    ///< O-Var: Turnover,Turnoverrate
  varSumOfInds=x.varSumOfInds;
  varIndsPerPop=x.varIndsPerPop;///< O-Var: s2suminds,s2indsperpop
  varExtRate=x.varExtRate;
  varColRate=x.varColRate;    ///< O-Var: s2extrate,s2colrate
  fluxRate=x.fluxRate;                 ///< O-Var:  Ext+Col/occup
  gmlambda=x.gmlambda;                 ///< maxLambda(20)
//cout<<"new Run(copy).\n";
}
//---------------------------------------------------------------------------
Run& Run::operator=(const Run& x){
  a->clear();
  for (int i=0;i<x.a->size();i++) a->push_back(x.a->at(i));                    ///< vector of years
  timeToExt=a->size();//x.timeToExt;                     ///< time to extinction
  maxYears=x.maxYears;                      ///< maximum number of years to simulate
  keycode=x.keycode;                       ///< sample site for documentation (obsolete)
  dimension=x.dimension;                  ///< fractal dimension of LC (obsolete)
  cellOccupied=x.cellOccupied;         ///<Belegungsmuster des Grids
  //Output Values
  sumOfInds=x.sumOfInds;             ///< O-Var: msuminds
  lambda=x.lambda;
  minOccupPops=x.minOccupPops;             ///< O-Var: minbes
  occupPops=x.occupPops;                ///< O-Var: aoccup
  indsPerPop=x.indsPerPop;               ///< O-Var: mIndsPerPop
  extRate=x.extRate;
  colRate=x.colRate;          ///< O-Var: extRate,colRate
  turnOver=x.turnOver;
  turnOverRate=x.turnOverRate;    ///< O-Var: Turnover,Turnoverrate
  varSumOfInds=x.varSumOfInds;
  varIndsPerPop=x.varIndsPerPop;///< O-Var: s2suminds,s2indsperpop
  varExtRate=x.varExtRate;
  varColRate=x.varColRate;    ///< O-Var: s2extrate,s2colrate
  fluxRate=x.fluxRate;                 ///< O-Var:  Ext+Col/occup
  gmlambda=x.gmlambda;                 ///< maxLambda(20)
  //cout<<"copy Run\n";
  return *this;
}
//---------------------------------------------------------------------------
Version::Version(){//lr=new vector<Version*>();
             lr= new vector<Run*>;
             //lr.assign(1,new Run());
             params=new Preferences;
             ID=params->ID;//999999;
             maxRuns=params->maxRuns;//0;
}; // maxYears=0;

//---------------------------------------------------------------------------
Version::~Version(){
   if (this->params!=NULL){
      delete this->params;params=NULL;
   }
   if (lr!=0){
      for (int i=0;i<GetNORuns();i++) {
          delete lr->at(i);lr->at(i)=NULL;
      }
      lr->clear(); delete lr; lr=NULL;}
   r=NULL;//r is only a copy of the last element in lr
   cout<<"del Version.\n";
}

//---------------------------------------------------------------------------
Version::Version(const Version& old){//lr=new TList;
             lr= new vector<Run*>(0);
             // make real copies of runs
             for (int i=0; i<old.GetNORuns();i++){
               Run* dummi=(Run*)old.lr->at(i);
               Run* dummi1=new Run(*dummi);
               lr->push_back(dummi1);
             }
             ID=old.ID;maxRuns=old.maxRuns;
             params=new Preferences(*(old.params));
             r=old.r;//r is a reference to the last element in lr
             //params=old.params;
  };
//---------------------------------------------------------------------------
///Überladen des Zuweisungsoperatoren =
/**
   !!!kopiert auch vorhandene Runs
*/
Version& Version::operator=(const Version& x)
{
  if (this==&x) return *this;
  // 1. alten Speicherbereich freigeben
      delete this->params;params=NULL;
      for (int i=0;i<GetNORuns();i++) {
          delete lr->at(i);lr->at(i)=NULL;
      }
      lr->clear();

 // this->~Version();
  // 2. neu besetzen
      //       lr= new vector<Run*>(0);
             for (int i=0; i<x.GetNORuns();i++){
               Run* dummi=(Run*)x.lr->at(i);
               Run* dummi1=new Run(*dummi);
               lr->push_back(dummi1);
             }
             ID=x.ID;maxRuns=x.maxRuns;
             params=new Preferences(*(x.params));
             r=x.r;//r is a reference to the last element in lr
  return *this;
};
//---------------------------------------------------------------------------
void Version::delRuns(){
     for (int i=0;i<GetNORuns();i++) {
          delete lr->at(i);lr->at(i)=NULL;
      }
      lr->clear();
}
//---------------------------------------------------------------------------
Results::Results()//{iv=ir=ia=0;prefFilePos=0;eofPref=false;}; //v.resize(1);iv=v.size()-1;
    {//lv=new TList;
    v=new Version();
    //iv=ir=ia=0;
    prefFilePos=0;eofPref=false;}
//---------------------------------------------------------------------------
Results::~Results(){
//    for (int i=0; i<lv.size();i++)  {
//     Version* dummi=(Version*) lv[i];
//     if (dummi) delete dummi; dummi=0;
//    }
//    lv.clear();
 if (v) delete v; v=NULL;
 cout<<"del Results.\n";
}
//---------------------------------------------------------------------------
/// free memory on Version-level (how many Versions to remember)
/**
  function to safe workspace for long simulations: the first Versions Results
  are deleted (+pointers are set to zero).
  If size is below remember nothing happens

  \param remember number of Versions not to delete; default is zero
*/
void Results::freeMem(const int remember){
    //int length=lv.size()-remember-2;
    //for (int i=length; i>=0;i--)  {
    // Version* dummi=(Version*) lv[i];
    // if (dummi) delete dummi; dummi=0;
    //}
}

//---------------------------------------------------------------------------
/**
  Sets the output file for the run-summaries. Creates the file and adds headers
  for the output variables used.
  \sa Run::runly here this file is appended
*/
void Results::setOFile(){
   const char* ziel= oFile.c_str();
  ofstream ausgabe(ziel);
  if(!ausgabe){
    //Öffnen ist gescheitert =>Fehlerbehandlung
    cout<<"(Results::setOFile()) Die Ausgabedatei konnte nicht erfolgreich"
        <<" geöffnet werden.";
    exit(3);
  }
  ausgabe << "ID\tRun\tD\tTime\tMaxTime\t"//Lambda\t"
               //"Kmetapop\tNmetapop\t"
               //"Npatches\tNoccup\t"
               //"Hav\tAntBes\tlambda\t"
               "Nmp\tminAntBes\tAntBes\tIndsPerPop\t"
               "ExtRate\tColRate\t"
               //"Turnover\tTurnoverRate\t"
               //"varNmp\tvarIndsPerPop\tvarExtRate\tvarColRate\t"
               //"fluxIntensity"
               "risk"<<endl;
  ausgabe.close();
  //Form1->Memo1->Lines->Append("Output-Datei: "+ (AnsiString) oFile.c_str());
  cout<< "Output-Datei: "<< oFile.c_str()<<"\n";
}
//---------------------------------------------------------------------------
/**
  Initialization of a new %Version. If this is the first version, the function
  tests whether the input file exists. Then the first (next) version is read
  from file (versionInit). At the end it is tested, whether the end of the
  input file is reached (the eofPref-flag is set).

  \return number of current version
*/
int Results::initVersion(void){ //once a Version
  const char* file=iFilePreferences.c_str();
  delete v; v= new Version();
  //if (iv==0) {    //erste Version?

    //liest Preferences aus Datei
    ifstream preferences(file);
    if(!preferences){
      //Öffnen ist gescheitert =>Fehlerbehandlung
      cout<<"Die Preferences-Datei konnte nicht erfolgreich"
          <<" geöffnet werden.\n" ;exit(3);
  }//}// end if version 0
  //einlesen und in Textfeldern ausgeben
  prefFilePos= Preferences::versionInit(file);
  v->params->static2object();
  //---Versions-ID ausgeben---
  v->ID=Preferences::ID; cout<< "Versions-ID: "<<Preferences::ID<<"\n";
  //---Laufzeit und # Wiederholungen einlesen---
  v->maxRuns=Preferences::maxRuns;// v->maxYears=Preferences::maxYears;
  //---Ende des Datensatzes---
//  lv.push_back(v); iv=lv.size()-1;
  if (prefFilePos==0)eofPref=true;
  return 0; //iv;
}

//---------------------------------------------------------------------------
/**
  Initialization of a new %Run. Adds a new Run object to the current version.
  \return number of current run/replicate
*/
int Version::initRun(void){   //once a run
//  v=(Version*) lv[lv.size()-1];
  //neuenRun anfügen
//  cout<<"initRun()..\n";
  r=new Run();
//  cout<<" new Run() succ.\n";
  lr->push_back(r);
//  cout<<" pushed_back().\n";
  //ir=GetNORuns()-1;
//  cout<<" got NORuns()\n";
  //für jede Wiederholung: Grids initialisieren
  r->cellOccupied.assign((Preferences::MaxX*Preferences::MaxY),false);
//  cout << " initRun() done.\n";
  return GetNORuns();
}
//---------------------------------------------------------------------------
/**
  Initialization of a new %Year. Adds a new Year to the a-list of the current run.
  \return the number of the current year
*/
int Run::initYear(){            //once a year
//Dem Ergebnis nächstes Jahr anfügen
  // v=(Version*) lv[lv.size()-1];
   //v->r= v->GetLastRun();// lr->at(v->GetNORuns()-1);
   Year locj= Year();
   a->push_back(locj);//ia=v->r->a->size()-1;
//   v->r->a.back().cellState.assign(
//         (Preferences::MaxX*Preferences::MaxY),0);
   return a->size();
}

//---------------------------------------------------------------------------
Year::Year(){//cellState.reserve(130*130);
         metapopCapacity=0;sumOfInds=0;sumOfSeeds=0;sumExtinct=0;
         sumColonized=0;potHab=0;occupied=0;freeHabs=0;indsPerPop=0;
         flux=0;};

//---------------------------------------------------------------------------
Year::Year(const Year& y){
  //cellState=y.cellState;
  metapopCapacity=y.metapopCapacity;
  sumOfInds=y.sumOfInds;
  sumOfSeeds=y.sumOfSeeds;
  sumExtinct=y.sumExtinct;
  sumColonized=y.sumColonized;
  potHab=y.potHab;
  occupied=y.occupied;
  freeHabs=y.freeHabs;
  indsPerPop=y.indsPerPop;
}

//---------------------------------------------------------------------------
Year& Year::operator=(const Year& y){
 if (this!=&y){
//   cellState=y.cellState;
   metapopCapacity=y.metapopCapacity;
   sumOfInds=y.sumOfInds;
   sumOfSeeds=y.sumOfSeeds;
   sumExtinct=y.sumExtinct;
   sumColonized=y.sumColonized;
   potHab=y.potHab;
   occupied=y.occupied;
   freeHabs=y.freeHabs;
   indsPerPop=y.indsPerPop;
 }
 return *this;
}

//---------Auswertung------------------------------------------------
/**
  Jährliche Auswertung für ein Grid
  Ermittelte Output-Variablen:
  - potHab (nb potential habitats)
  - occupied (nb occupie habitats + sb only)
  - freeHabs (nb free habitats)
  - sumOfSeeds (sum of seeds on grid)
  - sumOfInds  (size of summer population -sb )
  - sumExtinct (nb of extinct pops)
  - sumColonized (nb of newly colonized pops)
  - metapopCapacity (capacity of whole landscape)
  - flux        (sum of ext and colonized pops)
  - indsPerPop (mean pop size of visual pops)
.
  \return flag for extinction or overperformance of the run
  (0=OK;-1=Populations extinct;-2=sum of inds exceed half of metapop capacity)
*/
int Year::yearly(Landscape * lregion,vector<bool>& cellOccupied,int ltime){
//    Auswertung jährlich
//    cout<<"calc year "<<ltime<<endl;
    long reprod=0; long inds=0; long indsreg=0; long metacap=0;
    double long seed=0; double long seedProd=0;
    int occup=0; int pothab=0; int freehabs=0;
    int visualpops=0;
//    int class1=0;int class2=0; int class4=0; int class5=0; int class3=0;
    //zwischenspeichern der Populationsgrößen: Inds+Seeds
    int extinct=0; int colonized=0;

    vector<int>cellState;//vector<bool>
    cellState.assign((Preferences::MaxX*Preferences::MaxY),0);
 //   managed.assign((Preferences::MaxX*Preferences::MaxY),false);
    /// get for each site ...
    int dummisize1=cellOccupied.size();
    int dummisize2=lregion->pop.size();
    for(int x=0;x<lregion->pop.size();x++){
      long int cap= (long)lregion->pop[x].actCapacity;
   //   /// \date 14.2.08 document management of landscape
   //   managed[x]=lregion->pop[x].managed;
       if(lregion->pop[x].getSeeds()>0) cellState[x]=2;
       if(lregion->pop[x].actCapacity>0){
         /// capacities and occupancy
         cellState[x]+=1; pothab++;
       }
       if(lregion->pop[x].getIndsReg()>0) {
         ///visuality, stage composition and seeds produced
       //IndsReg: getInds() nach der Etablierung
       //adults: gespeicherter Wert zur Reproduktionszeit; wg Anuelle
         cellState[x]=4;visualpops++;
         reprod+=lregion->pop[x].adults;
         inds+=lregion->pop[x].getInds();
         indsreg+=lregion->pop[x].adults
                  +lregion->pop[x].getSummerPop(2);//plus Juveniles
         seedProd+=lregion->pop[x].getSeedProd();
       }
       /// get extinction and colonization events...
       if(lregion->pop[x].maxCapacity>0){
         seed+= lregion->pop[x].getSeeds();
         metacap+=cap;
         //Zählen der neuen und ausgestorbenen Populationen...
         long individuals =lregion->pop[x].getIndsReg();
         long popsize=individuals + lregion->pop[x].getSeeds();
         if(!cellOccupied[x] && individuals>0) {
           colonized++; cellOccupied[x]=true;
         } else if(cellOccupied[x] && popsize<=0) {
           extinct++;   cellOccupied[x]=false;
         } else cellOccupied[x]=cellOccupied[x];
         if (cellOccupied[x]) occup++;
         //freie, geeignete Habitate zählen:
         if (!cellOccupied[x] && lregion->pop[x].actCapacity>0) freehabs++;
       }
    }
    /// set class variables...
    potHab      =pothab;
    occupied    =occup;
    freeHabs    =freehabs;
    sumOfSeeds  =seed;
    sumOfInds   =indsreg;//071012: reprod; //nicht: inds
    sumExtinct  =extinct;
    sumColonized=colonized;
    metapopCapacity=metacap;
    flux        =extinct+colonized;
    float perPop=0; if (visualpops) perPop= indsreg/(float) visualpops;
    indsPerPop=perPop;

    //testing exit condition
//    cout<<"  ReprodInds: "<<reprod<<"\tOccup: "<<occup<<" of "
//        <<pothab<<"\tCap: "<<metapopCapacity<<endl;
    if(!occupied) return -1;  //(inds+seed)==0
    /// \date 20.2.08 changed growth threshold from .5 to .2
    if((double)reprod/metapopCapacity>0.01){//0.005  //((inds+seed*0.1)>100000000){
      return -2;
    }
    return 0;
}//end yearly
//-----------------------------------------------------------
/**
  Jährliche Auswertung für ein Grid mit selektierten Auswertezellen
  Ermittelte Output-Variablen:
  - potHab (nb potential habitats)
  - occupied (nb occupie habitats + sb only)
  - freeHabs (nb free habitats)
  - sumOfSeeds (sum of seeds on grid)
  - sumOfInds  (size of summer population -sb )
  - sumExtinct (nb of extinct pops)
  - sumColonized (nb of newly colonized pops)
  - metapopCapacity (capacity of whole landscape)
  - flux        (sum of ext and colonized pops)
  - indsPerPop (mean pop size of visual pops)
.
  \return flag for extinction or overperformance of the run
  (0=OK;-1=Populations extinct;-2=sum of inds exceed half of metapop capacity)
*/
int Year::yearly_selcells(Landscape * lregion,vector<bool>& cellOccupied,
      int ltime){ //,vector<bool>& cellsToAnalyze
//    cout<<"CA.";//"calc year "<<ltime<<endl;
    unsigned long reprod=0, inds=0, indsreg=0, selcap=0;
    double long seed=0, seedProd=0;
    int occup=0, pothab=0, freehabs=0, selhabs=0;
    int visualpops=0;
    //zwischenspeichern der Populationsgrößen: Inds+Seeds
    int extinct=0, colonized=0; int crashes=0;

    vector<int> cellState;
    cellState.assign((Preferences::MaxX*Preferences::MaxY),0);
    /// get for each site ...
    for(int x=0;x<lregion->pop.size();x++)
    {
      /// get only for selected cells...
      int hgug=(int) Preferences::MusterCapacity[x];
      if(Preferences::MusterResultCells[x]){  //loaded Result cells
//      if(Preferences::MusterResultCells[x]&&  //loaded Result cells
//         pCurrCell->maxCapacity==5000){       //only top habitat
//         (hgug==913||hgug==914||hgug==915)){       //only arable fields
      Population* pCurrCell=&(lregion->pop[x]);
      unsigned long cap=(unsigned long)pCurrCell->actCapacity;
      selhabs++;//this is a segetal cell
      /// get extinction and colonization events...
         seed+= pCurrCell->getSeeds();
         selcap+=cap;
         //Zählen der neuen und ausgestorbenen Populationen...
         long individuals =pCurrCell->getIndsReg() - pCurrCell->getSummerPop(1);//don't count seedlings
         long popsize     =pCurrCell->getIndsReg() + pCurrCell->getSeeds();
         if(!cellOccupied[x] && pCurrCell->getIndsReg()>0) {//count seedlings
           colonized++; cellOccupied[x]=true;
         } else if(cellOccupied[x] && popsize<=0) {
           extinct++;   cellOccupied[x]=false;
         } else cellOccupied[x]=cellOccupied[x];
         if (cellOccupied[x]) occup++;
         //freie, geeignete Habitate zählen:
         if (!cellOccupied[x] && pCurrCell->actCapacity>0) freehabs++;
         // get cell state
         if(pCurrCell->getSeeds()>0) cellState[x]=2;
         if(pCurrCell->actCapacity>0){
           /// capacities and occupancy
           cellState[x]+=1; pothab++;
         }
         inds+=individuals;//pCurrCell->getInds();         //IndsReg: getInds() nach der Etablierung
         seedProd+=pCurrCell->getSeedProd();
         if(individuals>10) { //Eine Pop wird als solche im Feld erkannt, wenn mind 10 Inds vorhanden sind
           ///visuality, stage composition and seeds produced
           cellState[x]=4;visualpops++;
//           reprod+=pCurrCell->adults;          //adults: gespeicherter Wert zur Reproduktionszeit; wg Anuelle
           indsreg+=pCurrCell->getIndsReg() - pCurrCell->getSummerPop(1);//don't count seedlings
         }
        /// count population crashes
        if (pCurrCell->adults==0&&pCurrCell->getIndsReg()>=cap) crashes++;
      }//end if cap==5000
    }//end for all cells
    /// set class variables...
    potHab      =pothab;
    occupied    =visualpops;//occup;
    freeHabs    =freehabs;
    sumOfSeeds  =seed;
    sumOfInds   =indsreg;//071012: reprod; //nicht: inds
    sumExtinct  =extinct;
    sumColonized=colonized;
    metapopCapacity=selcap;
    flux        =extinct+colonized;
    float perPop=0; if (visualpops) perPop= indsreg/(float) visualpops;
    indsPerPop=perPop;
    //crashesPerYear=(double)crashes/selhabs;
    crashesPerYear=(double)crashes/occup;

    //testing exit condition
    //if(!metapopsize) return -1;  //(inds+seed)==0
//    if(!occupied) return -1;  //no occupied arable field site
    if(!visualpops) {  //no visible occupied arable field site
      cout<<"extinct\t\tYear: "<<ltime<<endl;return -1;
    }
    /// \date 20.8.08 changed growth threshold from .5 to .1
    if((double)inds/selcap>0.1){//.05 .1 0.5  //((inds+seed*0.1)>100000000){
//    if((double)occupied/selhabs>0.1){//more than 10% of cells are occupied
      cout<<"density: "<<(float)inds/selcap*100.0<<" %\tYear: "<<ltime<<endl;
      return -2;
    }
// cout<<"CO\n";
    return 0;
}//end yearly_selcells

//-----------------------------------------------------------
/**
 Finds a site with high capacity potential to document its population dynamics.
 This function and the correxsponding variable keycode are not used anymore since
 in function docLocalPops all potential habitats are monitored.
 \see Results::docLocalPops
*/
void Results::getKeycode(Landscape * lregion){
 // v=(Version*) lv[lv.size()-1];
  v->r=v->GetLastRun(); //lr->at(v->GetNORuns()-1);

  for (int x=0;x<lregion->pop.size();x++) {
     if (lregion->pop[x].maxCapacity>100 && lregion->pop[x].getIndsReg()>0)
     {v->r->keycode=x;break; }
  }
}
//--------------------------------------------------
/**
 Returns the geometric mean growth rate (lambda) of the first years of the
 simulation. If the replicate goes extinct before the time'th year, mean is
 calculated of the previos years.

 When Habitats are initiated with low individual numbers, this function can
 measure the potential growth rate.
 \param time length of period to measure (standard is 20)
*/
float Run::gmLambda(int time){
   float maxL=0.0;
   int yrs=min((time+5),timeToExt);
   if (yrs>5){
     long double inda=a->at(4).sumOfSeeds;// .sumOfInds;
     long double indo=a->at(yrs-1).sumOfSeeds;//sumOfInds;
     if (inda)
     maxL=pow((double)(indo/inda),(double)1/(yrs-5.0));
   }
   return maxL;
}// end gmLambda

//--------------------------------------------------
/**
  Compares Results of two Runs. Returns a float quotient of two risk values.
  If Run2 is riskier than Run1, the value is greater than 1.
  Elsewhere it's between 0 and 1.
  \param v gives the comparing variant: "risk", "lambda"
\verbatim
  run#1      ext   leg   over
  run#2 ext   1     0     0.5
        leg   1     x     1
        over  0.5   0     1
\endverbatim
  */
float Run::compare(Run& run1,Run& run2, string v){
  bool ext1 = run1.timeToExt<run1.maxYears;
  bool ext2 = run2.timeToExt<run2.maxYears;
  bool over1= run1.timeToExt>run1.maxYears;
  bool over2= run2.timeToExt>run2.maxYears;

  if ((ext2||over2) && !(over1||ext1)) return 0;
  else if ((over1 && ext2)||(over2 && ext1)) return 0.5;
  else if (ext1 && !over2) return 1;
  else if (over1 && !ext2) return 1;

  if (v=="risk") {
   float risk1= run1.risk();
   float risk2= run2.risk();
   return (risk2/risk1);//(0.5+(risk2-risk1)/2.0);
  }else{//if v=="lambda" do...
    float dr=run2.gmlambda-run1.gmlambda;
    return 1.0-dr;//lambda should be minimized
  }
}//end compare Runs
//--------------------------------------------------
/**
  vergleiche zwei Versionen, indem alle Kombinationen von
  einzelnen Runs miteinander verglichen werden (\sa Run::compare()).
  gibt den Mittelwert aller Vergleiche aus.
*/
float Version::compare(Version& v1,Version& v2)
{
  int c=0;float lcomp=0;
  for (int run1=0; run1<v1.lr->size();run1++){
    Run* cRun1= (Run*) (*v1.lr)[run1];
    for (int run2=run1; run2< (*v2.lr).size();run2++){
      c++;
      Run* cRun2= (Run*) (*v2.lr)[run2];
      lcomp+=Run::compare(*cRun1,*cRun2);
  }}
  //if (c==0) cerr<<"nothing to compare\n";
  return lcomp/c;
}
//----------------------------------------end file
