/**  \file
  \brief Implementation der Datei-Ausgabe Funktionen

  \author Katrin Körner

Funktionen:
- Run::runly
- Run::runly_steps
- Results::docLocalPops
- Results::ausgabeYear
- Results::ausgabeYears
- Results::ausgabeGrid

\date 28.9.07
- Auslagerung der Dateiausgabe der Run und Results -Klasse

*/
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

#include "mresults.h"

//----Inline Funktionen für die Grids------------------
inline int getIndex(int x,int y){
//hier sinnhafte Wiederholung, da inline über Dateigrenzen scheinbar nicht erlaubt
  if((x<0)||(y<0)||(x>=Preferences::MaxX)||(y>=Preferences::MaxY))return -1;
  else return Preferences::MaxY*x+y;}
inline int getX(int index){return index/Preferences::MaxY;}
inline int getY(int index){return index%Preferences::MaxY;}

//------------------------------------------------
///
/// Auswertung des aktuellen Runs und Ausgabe in die Output-Datei
///
char Run::runly(Landscape * lregion,int maxYears, int ID,int run){ //
//cout<<"runly().."<<endl;

  timeToExt=(a->size());
  double mpothab=0; double moccup=0; double mfreehabs=0;
  double mmetacap=0; double msumseeds=0;
  double mcol=0; double mext=0;
  double mcrashes=0;

  string exttime="---";
  unsigned int beg=Preferences::InitTime;//200;
//cout<<"get last "<<timeToExt-beg<<" years..\n";
  if(a->size()>=beg+50){     //wenn die Pop wenigstens 250J überlebte
    for(int i=beg-1;i<timeToExt;i++){       //(int i=beg;i<ltime;i++)
      Year* heu=&(a->at(i));    //this year
      Year* heu_1=&(a->at(i-1));//last year

      mpothab    += heu->potHab; //Test, ob Adresse existiert
      moccup     += heu->occupied;
      mfreehabs  += heu->freeHabs;
      mcol       += heu->sumColonized;
      mext       += heu->sumExtinct;
      msumseeds  += heu->sumOfSeeds;
      sumOfInds  += heu->sumOfInds;
      varSumOfInds += pow(heu->sumOfInds,2);
      mmetacap   += heu->metapopCapacity ;
      minOccupPops= min(minOccupPops,(heu->occupied/(double) heu->potHab));
      //if(i<(timeToExt-100)) msuminds50 +=a[i].sumOfInds; //berechnung der letzten 100J ?
      indsPerPop += heu->indsPerPop;
      varIndsPerPop+= pow(heu->indsPerPop,2);
      float addCR=0; float addER;
      //Kolonisationsrate:
      double dummi= heu_1->freeHabs*heu_1->occupied; //Ex Adr?
      if(dummi!=0)
        addCR= heu->sumColonized*heu_1->potHab/dummi;
      //Extinktionsrate:
      if(heu_1->occupied>0)
        addER= heu->sumExtinct/(double) heu_1->occupied;

      colRate += addCR; extRate += addER;
      varColRate += pow(addCR,2); varExtRate += pow(addER,2);
      //FluxIntensity
      if((heu->occupied+heu_1->occupied)>0)//wenn in einem der beiden Jahre besetzt
        fluxRate+=heu->flux*2.0/(heu->occupied+heu_1->occupied);
      //Number of crashing Years
      mcrashes+= heu->crashesPerYear;
    }

    //---Varianzen berechnen (nach Zar Eq.4.10):
    varSumOfInds-=pow(sumOfInds,2)/(timeToExt-beg);varSumOfInds/=(timeToExt-beg-1);
    varIndsPerPop-=pow(indsPerPop,2)/(timeToExt-beg);varIndsPerPop/=(timeToExt-beg-1);
    varColRate-=pow(colRate,2)/(timeToExt-beg);varColRate/=(timeToExt-beg-1);
    varExtRate-=pow(extRate,2)/(timeToExt-beg);varExtRate/=(timeToExt-beg-1);

    //---Mittelwerte berechnen:
    mpothab/=(timeToExt-beg); moccup/=(timeToExt-beg); mfreehabs/=(timeToExt-beg);
    msumseeds/=(timeToExt-beg);mmetacap/=(timeToExt-beg);
    sumOfInds/=(timeToExt-beg);
    indsPerPop/=(timeToExt-beg);//msuminds50 /=100;
    colRate/=(timeToExt-beg); extRate/=(timeToExt-beg);
    mcol/=(timeToExt-beg);mext/=(timeToExt-beg);
    if (mext!=0) turnOver=(mcol/mext);
    if (extRate!=0) turnOverRate=(colRate/extRate);
    fluxRate/=(timeToExt-beg);
    crashesPer100=mcrashes/(timeToExt-beg);
    //---Wachstumsrate berechnen: letzte 100 Jahre
    lambda=pow(((double)a->at(timeToExt-1).occupied+1)/
                (a->at(beg+1).occupied+1),1.0/(timeToExt-beg));
  } //Ende wenn mind 200J. durchlaufen
  //float hav = mpothab/((Preferences::MaxX)*(Preferences::MaxY));
  if (mpothab>0) occupPops=moccup/mpothab ;

  /// file output...
//cout<<"open ofile ..\n";
  const char* ziel=Results::oFile.c_str(); //Form1->SaveDialog1->FileName.c_str();
  ofstream ausgabe(ziel,ios::app);
  if(!ausgabe){
    cout<<"(Run::runly()) Die Ausgabedatei konnte nicht erfolgreich"
        <<" geöffnet werden.\n";
    exit(3);
  }
  ausgabe.seekp(0, ios::end);
  int length = ausgabe.tellp();
  if (length==0){
   ausgabe << "ID\tRun\tESource\tTime\tMaxTime\t"
               "Nmp\tminAntBes\tAntBes\tIndsPerPop\t"
               "ExtRate\tColRate\t"
               "risk\tL20"<<endl;}
//cout<<"write now..\n";
  ausgabe <<ID <<"\t"<< run <<"\t" ;
  int nbpops=a->back().occupied;
//cout<<"a->back().occup "<<nbpops<<endl;
  char esource='r';
  if (nbpops==0) esource='e';else if (timeToExt!=maxYears) esource='o';
  ausgabe << esource<<"\t";
  ausgabe<<timeToExt <<"\t"<<maxYears
          <<"\t"<<setprecision(4)<< sumOfInds
          <<"\t"<<setprecision(4)<< minOccupPops
          <<"\t"<<setprecision(4)<< occupPops
          <<"\t"<<setprecision(4)<< indsPerPop
          <<"\t"<<setprecision(4)<< extRate
          <<"\t"<<setprecision(4)<< colRate ;
//  ausgabe <<"\t"<< turnOver<<"\t"<< turnOverRate<<"\t"<<fluxRate;
  ausgabe  <<"\t"<<setprecision(4)<< risk();
//cout<<"risk() calculated.\n";
  /// \date 20.2.08 output of initial growth rate
  ausgabe  <<"\t"<<setprecision(4)<< gmLambda();
//cout<<"gmLambda() calculated.\n";
  ausgabe <<endl;
  ausgabe.close();
  return esource;
//cout<<"end runly().\n";
} //end runly

//------------------------------------------------
///
/// Auswertung des aktuellen Runs und Ausgabe in die Output-Datei
///
/**
   Eine Zusammenfassung des aktuellen runs wird in eine Ausgabedatei
   geschrieben. runly_steps fasst den Zustand des Grids zu mehreren Zeitpunkten
   zusammen. (50er Schritte von 250 bis 500)
*/
void Run::runly_steps(Landscape * lregion,const int maxYears,const int ID,int run,
          const int lap, const int slides){ //

  //Datei-Ausgabe einrichten
  string s=Results::oFile;
  //---
  stringstream strstream;
  strstream << lap;
//  std::string str = strstream.str();
  //---
  string s1=string("-s")+strstream.str()+".txt";
  const char* ziel=(s.replace(s.rfind(".txt"),4,s1)).c_str(); //Form1->SaveDialog1->FileName.c_str();
  ofstream ausgabe(ziel,ios::app);
  if(!ausgabe){
    cout<<"(Run::runly_steps()) Die Ausgabedatei konnte nicht erfolgreich"
        <<" geöffnet werden.\n";
    exit(3);
  }
  /// fileoutput headline...
  ausgabe.seekp(0, ios::end);
  int length = ausgabe.tellp();
  if (length==0){
   ausgabe << "ID\tRun\tD\tTime\tMaxTime\t"
               "Nmp\tminAntBes\tAntBes\tIndsPerPop\t"
               "ExtRate\tColRate\t"
               "risk"<<endl;}

  timeToExt=(a->size());
  int inittime=Preferences::InitTime;
  double mpothab=0; double moccup=0; double mfreehabs=0;
  double mmetacap=0; double msumseeds=0;
  double mcol=0; double mext=0;
  /// summarize (means) lap time for each lap time step
  //'For'-Schleife für alle 50er zwischen 250 und 500:
  for (int tstep=inittime+lap;
       tstep<=min((inittime+slides*lap),timeToExt-1);
       tstep+=lap){
    int beg=tstep-lap;
    for(int i=beg-1;i<tstep;i++){
      Year* heu=&(a->at(i));
      mpothab  +=heu->potHab; //Test, ob Adresse existiert
      moccup   +=heu->occupied;
      mfreehabs += heu->freeHabs;
      mcol += heu->sumColonized;
      mext+=  heu->sumExtinct;
      msumseeds+=heu->sumOfSeeds;
      sumOfInds +=heu->sumOfInds;
      mmetacap +=heu->metapopCapacity ;
      minOccupPops= min(minOccupPops,(heu->occupied/
                  (double) heu->potHab));
      indsPerPop +=heu->indsPerPop;
      float addCR=0; float addER;
      //Kolonisationsrate:
      double dummi=a->at(i-1).freeHabs*(*a)[i-1].occupied; //Ex Adr?
      if(dummi!=0)
        addCR= heu->sumColonized*(*a)[i-1].potHab/dummi;
      //Extinktionsrate:
      if((*a)[i-1].occupied>0)
        addER= heu->sumExtinct/(double) (*a)[i-1].occupied;

      colRate += addCR; extRate += addER;
      //FluxIntensity
      if((heu->occupied+(*a)[i-1].occupied)>0)//wenn in einem der beiden Jahre besetzt
        fluxRate+=heu->flux*2.0/(heu->occupied+(*a)[i-1].occupied);
    }

    //---Mittelwerte berechnen:
    float dummi=tstep-beg; if (dummi>0){
      mpothab/=dummi; moccup/=dummi; mfreehabs/=dummi;
      msumseeds/=dummi;mmetacap/=dummi;
      sumOfInds/=dummi;
      indsPerPop/=dummi;
      colRate/=dummi; extRate/=dummi;
      mcol/=dummi;mext/=dummi;
      if (mext!=0) turnOver=(mcol/mext);
      if (extRate!=0) turnOverRate=(colRate/extRate);
      fluxRate/=dummi;
      if (mpothab>0) occupPops=moccup/mpothab ;
    }
    /// file output of results
    ausgabe <<ID <<"\t"<< run <<"\t" << Preferences::dimension<<"\t"
          <<tstep <<"\t"<<maxYears
          <<"\t"<<setprecision(4)<<sumOfInds
          <<"\t"<<setprecision(4)<<minOccupPops
          <<"\t"<<setprecision(4)<<occupPops
          <<"\t"<<setprecision(4)<< indsPerPop
          <<"\t"<<setprecision(4)<< extRate
          <<"\t"<<setprecision(4)<< colRate ;
    ausgabe  <<"\t"<<setprecision(4)<< risk();
    ausgabe <<endl;

  } //end for 50er (lap) schritte tstep
  ausgabe.close();
} //end runly_steps

//-------------------------------------------------------
/**
  \brief annual file output of complex cell properties
  file output of annual complex cell properties of a previously chosen cell.
  \see Results::getKeycode
*/
void Results::ausgabeYear(char* datei, Landscape * lregion){
//Ausgabe komplexer Zelleigenschaften pro Jahr, keycode bekannt
//  v=(Version*) lv[lv.size()-1];
  v->r=(Run*) v->GetLastRun();// >lr[v->lr->size()-1];

  stringstream dummi0; dummi0<<v->ID;
  string dummi1= path+("TS-1pop"+dummi0.str()+".txt") ;
  const char* ziel1= dummi1.c_str();//= datei;
  ofstream data1(ziel1,ios::app);
  if(!data1){
    cout<<"Die Ausgabedatei konnte nicht erfolgreich"
        <<" geöffnet werden.\n";
    exit(3);
  }

  data1 << v->GetNORuns() <<'\t'<< v->r->a->size()<<'\t';  //Run tab Jahr
  int key = v->r->keycode;
  for (int i=0; i<5; i++)
    data1 << lregion->clima->getWeather(i,key)<<'\t';
  data1 << lregion->pop[key].actCapacity<<'\t';
  for (int h=0; h<5; h++)  data1<<lregion->pop[key].getSummerPop(h)<<'\t';
  for (int j=0; j<5; j++)  data1<<lregion->pop[key].getStadium((state)j)<<'\t';
  data1<<v->r->cellOccupied[key]<<'\n';
}//end AusgabeYear
//-------------------------------------------------------
/// file output of all potential habitats
/**
  Opens a file to append current site-information on stage composition and
  seed production.
  \param datei file name of ASCII-txt file to append data
  \param lsc landscape with potential habitat sites to document
  \warning this function can result in very large files
*/
void Results::docLocalPops(const char* datei, Landscape & lsc){
//Ausgabe komplexer Zelleigenschaften pro Jahr, keycode bekannt
  ofstream ausgabe(datei,ios::app|ios::ate);
  if(!ausgabe){
    //Öffnen ist gescheitert =>Fehlerbehandlung
    cout<<"(Results::docLocalPops()) Die Ausgabedatei konnte nicht erfolgreich"
        <<" geöffnet werden.";
    exit(3);
  }
  if ((short)ausgabe.tellp()==0) ausgabe<<"Version\tRun\tYear\tSiteID\tHabTyp\t"
     <<"SEED\tSEEDL\tJUV\tVEG\tGEN\tSProd\tCurrCap\tdisturbed"<<endl;
  long VersionID=v->ID;
  int Year=v->r->a->size();
  //für alle pot geeigneten Habitate
  for(int i=0;i<lsc.pop.size();i++)
//    if(lsc.pop[i].maxCapacity>0){
    if(Preferences::MusterResultCells[i]){
        int LocID=i;
        Population *currPop=&(lsc.pop[i]);
        ausgabe<<VersionID<<'\t'<<v->lr->size()<<'\t'<<Year<<'\t'<< LocID<<'\t'<<dec<<Preferences::MusterCapacity[i]<<'\t';
        typedef state State;
        for (int j=(int)SEED; j<=(int)GEN; j=j+1) ausgabe<< currPop->getSummerPop(j)<<'\t';
        ausgabe<<currPop->getSeedProd()<<'\t'
           <<dec<<currPop->actCapacity<<'\t'<<boolalpha<<currPop->disturbed<<endl;
  }
}//end docLocalPops
//--------------------------------------------------
///
///  gibt ein einzelnes Grid in eine Datei aus
///  \param datei Ausgabedatei
///  \param grid auszugebendes Grid
///
void Results::ausgabeGrid(vector<float>& grid, int loop, char* datei){
  char* ziel= datei;//"grid.txt";
  ofstream data(ziel,ios::app);
  if(!data){
    cout<<"(Results::ausgabeGrid())Die Ausgabedatei konnte nicht erfolgreich"
        <<" geöffnet werden.\n";
    exit(3);
  }
  for (int x=0; x<grid.size(); x++){
    data <<loop <<"\t"<< getX(x) <<"\t"<< getY(x) <<"\t"<< grid[x] <<"\n";
  }
}
//-------------------------------------------------------
void Results::ausgabeGrid(vector<int>& grid, int loop, char* datei){
  vector<float> gridFloat;
  for(unsigned x=0; x<grid.size(); x++){
    gridFloat.push_back(float(grid[x]));
  }
  ausgabeGrid(gridFloat, loop, datei);
}

//-------------------------------------------------------
///
///Ausgabe der jährlichen Daten zur Populationsentwicklung in eine Textdatei
///(obsolete)
void Results::ausgabeYears(char* datei){
//  v=(Version*) lv[lv.size()-1];
  v->r=(Run*) v->GetLastRun();// >lr)[v->lr->size()-1];
  if (true){
   //Ausgabe der "percent occupied"(pco)
   stringstream dummi1; dummi1<< "TS-pco"<<v->ID<<".txt" ;
   const char* ziel1= dummi1.str().c_str();//= datei;//"lifetime.txt";
   ofstream data1(ziel1,ios::app);
   if(!data1){
     //Öffnen ist gescheitert =>Fehlerbehandlung
     cout<<"(Results::ausgabeYears()) Die Ausgabedatei 1 konnte nicht erfolgreich"
         <<" geöffnet werden.\n";
     exit(3);
   }
   //Ausgabe der Populationsgröße (nip)
   stringstream dummi2; dummi2<<"TS-nip"<<v->ID<<".txt" ;
   const char* ziel2= dummi2.str().c_str();//= datei;//"lifetime.txt";
   ofstream data2(ziel2,ios::app);
   if(!data2){
     //Öffnen ist gescheitert =>Fehlerbehandlung
     cout<<"(Results::ausgabeYears()) Die Ausgabedatei 2"
         <<   "konnte nicht erfolgreich geöffnet werden.\n";
     exit(3);
   }
   //int run=0;
   for (int i=0; i<v->lr->size(); i++ ) {
     Run* locr= (Run*) (*v->lr)[i];
     for (int year=0; year<locr->a->size(); year++){
       //if (year<timeToExt[i]) {
         //if (run>=sumOfInds.size()) break;
         data1 << (*locr->a)[year].occupied/
                  float((*locr->a)[year].potHab) << "\t" ;
         data2 << (*locr->a)[year].indsPerPop << "\t" ;
         //data << i << "\t" <<sumOfInds[run] << endl;
         //run++;
       //} else{ data1 << 0 << "\t"; data2 << 0 << "\t";}
     } data1 << endl; data2 << endl;
   }
  }//ende if false / true
}//end ausgabeYears

//--------------end of file 'fileout.cpp'-------------------------------------------------
