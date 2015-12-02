#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>

#include "classes/mresults.h"
#include "classes/random.h"
using namespace std;
//---------------------------------------------------------------------------
/**
  \file
  \brief vegetation model -sensitivity of pk-parameters (C++ Standard, console)
*/

/*! \mainpage
 *
 * \author Katrin Körner
 * \version 3.01
 * \date 2008-02-03
 * \par Type
 *  simulation model
 *
 * \section intro_sec Introduction
 *
 * \section verbal_descr Verbal Description of what the Code does
 * the code mofifies the parameter values of the pk initially given to make a sensitivity analysis
 * of the single model parameters. Runs are done with the modified parameter sets, results are documented
 *
 * \par Flow Chart
 *   \image html flow-small.GIF
 *
 * \section exp_input Expected Input
 *  Starting the Application needs one of the following source files:
 *  - a \p toRun.txt with the following internal structure:
      \verbatim
      <ParameterCombinationList> *.dat
      \endverbatim
      with two lines given the parameter boundaries of the mcmc optimization algorithm
 *  - if there is a file named like
      \verbatim
      <ParameterCombinationList> *.txt
      \endverbatim
      the last pk in this file is used as starting point for the new mcmc-chain
 *
 * \subsection lc_file Landscapes
 * \subsection pk_file Parameter Sets
 *
 * \section output Output
 *  Two Output-Files in ASCII-text format is written containing
 *  #) summaries for each %Run a line. For columns see description of VegModel Standard version,and
 *  #) containig a new parameter-kombination  list which is the chain
 * \par Requirements and Environment
 *  CodeBlocks C++ (MinGW), Linux standard Gnu C++ compiler (hopefully)
 *
 * \section sensi Sensitivity Analysis
 *  do add later
 *
 * \section vali Validation
 *  to be validated e.g. with Quillow-Data-Set
 *
 * \section source Sources
 *     \verbatim
 *     sensiVegModel.cpp
 *     classes\*.*
 *     \endverbatim
 *
 * \section refs Publications and Applications referring to the code
 *  Körner and Jeltsch (2008). Ecological Modelling
 *  Körner et al. (accepted). Journal of Vegetation Science
 *
 * \note Chapters 'Bugs' and 'Flow Chart' are included
 *         elsewhere in this Documentation
 */


Results* ergs=new Results();
const bool  MCMCVersion=true;//true;
string FileName="";
int IndexID=1;

void MCMC(float adj=1.0,int repeat=5,int length=20000);
float Trial(const int,Version*);
char OneRun(Version*);

//-----------------------------------
//------MAIN---------------------------------------------------
/// Main Function
/**
  runs a MontecarloMarkovChain for optimization of population parameters in
  a standard case. Ranges for parameter values are read from a file,
  given as program parameter argument.
*/
int main(int argc, char *argv[])
{
   FileName=(string)"MCMC-test.dat";
   if (argc>=2) FileName=argv[1];
   // sechs versch Landschaften ab V080422
   //rastert_allregi1.aig//Q-lcs-dim3-ph1.gtg//qll-hgugdom-200.txt//qll-hgugdom-100.txt
   Results::iFileLandscape=(string)"qll-hgug100-a.aig";

   /// get landscape file...
   Landscape::loadLandscape(Results::iFileLandscape.c_str());//only for realistic landscapes
   /// get result cells...
   //qll_buff_1km-200.txt//qll_buff_1km-100.txt
   Landscape::loadResultCells("qll_buff_1km-sites-100.aig");//ignore buffer zone (1km)

   const float c=.75;//1; //< weighting factor for Metropolis-Hasings algorithm
   MCMC(c,5,1000);    ///Version mit mcmc-optimierung (weighting factor, parallel runs, chain length)

   if (ergs) delete ergs; ergs=0;
//   if (true) system("PAUSE");//no interaction needed
   return EXIT_SUCCESS;
}
//---------------------------------------------------------
/// run replicates of a parameter combination
/**
  run the current version \verb{number} times and returns their mean risk
  values if all runs were valid. If not, -1 is returned.
  \param number number of runs to simulate
  \return -1 if at least one run was not valid; mean risk value else
*/

float Trial(const int number, Version* cVersion)
{
//cout<<"Trial ";
  //Version *cVersion;
  float lrisk=0;
  for (int i=0;i<number;i++){
    cout<<"\nTrial "<<i<<" ..";
    char result=OneRun(cVersion);
    lrisk+=cVersion->r->risk();
    //if extinct or too good --> return -1
    if (result!='r')//ab V081016
      {cout<<"Trial: failed\n";return -1;}
    cout<<lrisk;
  }
  lrisk/=number;
  //return mean risk value
  cout<<" Trial: "<<lrisk<<endl;
  return lrisk;
}
//---------------------------------------------------------------------------

///do one simulataion run
/**
  function doing one simulation run. A new Run-object is added to the global
  results-object. then the simulation is initiated, and successive years/
  time steps are calculated. The run stops if a) all populations died, or b) the
  number of individuals on the grid exceed a certain value (\sa Year::yearly()).

  Yearly and Runly summaries are done.

  \sa Year::yearly()
  \sa Run::runly()
  \sa Lansdscape::oneYear()
*/

char OneRun(Version* vers)
{
cout<<"\nRun...";
/* eine Wiederholung des Metapopulationsmodells*/
   //Umgebung (Landscape) erstellen
   Landscape* region= new Landscape();
   region->setPopulations();
 cout<<"pops set.\n";
   //Ergebniss-datensätze anlegen:
   vers->initRun();
// cout<<"initRun() done.\n";
   Run* locr= vers->GetLastRun(); //lr.at(ergs->v->lr.size()-1);
   vers->params->object2static();

//cout <<"init done.\n";

   //Zeitschritte
   int time=0; do{
//    cout<<"Y"<<time;
     time=locr->initYear();//cout<<"done: initYear\n";
     region->oneYear(true);            // <-- hier läuft ein Zeischritt
     //Auswertung
 //   cout<<" R";
     int test=locr->a->back().yearly_selcells(
         region,locr->cellOccupied,time);
 //   cout<<"\tok\n";
     if(test==-1) {
       cout<<">>> Pops ausgestorben!!\n";
       break;                   //abbrechen, wenn Metapop ausgestorben
     }
     if(test==-2) {
       cout<<">>> zu viel Inds!!\n";
       break;                   //abbrechen, wenn Metapop ausgestorben
     }
   }while(time<Preferences::maxYears); //Ende Time   ergs->v->
//    cout<<"YE "<<time<<endl;

   //anschließend: Ausgabe Time to Extinction
   char result=locr->runly(region,Preferences::maxYears,Preferences::ID,vers->lr->size());
//   cout<<"done\n";
   delete region; region=0;
   return result;
}
//---------------------------------------------------------------------------
/// file output with mcmc-documentation
void MCMCDoc(const char* file, int ID, short succ){
      //hier Dateiausgabe der Parameterkombinationen
   ofstream mcdoc(file,ios::app);
   if(!mcdoc){//Öffnen ist gescheitert =>Fehlerbehandlung
     cout<<"mcmc-doc file konnte nicht erfolgreich"
                            " geöffnet werden."<<endl;    exit(3);
        }
   mcdoc<<ID<<'\t'<<succ<<endl;

    }
//---------------------------------------------------------------------------
///generates a mcmc-chain to optimize parameter sets
/**
  find suitable sets of plant type parameter-settings
  using MCMC method (Metropolis-Hastings method)
  \param adj calibrates M-H-algorithm to accept 1/4th of pks circa
  \param repeat how many trials to judge a pk
  \param length length of Markov chain to calculate
*/
void MCMC(float adj,int repeat,int length)
{
  Results::oFile=(string) "MCMC-test.txt";//ofile;

  //read parameter ranges
  if(FileName.length()>3)
  {
    string file=FileName.c_str();
    Results::oFile=file;
    Results::oFile=Results::oFile.replace(Results::oFile.rfind("."),4,".txt").c_str() ;
    string newSetsFile=FileName.replace(FileName.rfind("."),4,"-n.txt") ;
    string docMCFile=FileName.replace(FileName.rfind("."),4,"-d.txt") ;
    Preferences::versionInit(file.c_str());
    Version* RangeMin=new Version();
    Preferences::versionInit(file.c_str());
    Version* RangeMax=new Version();
    if (ergs->prefFilePos==0)ergs->eofPref=true;
  ///get next version to test sensitivities
  short file_eof;//test for eof in input-file
  do{

    /// choose (valid) starting point
  cout<<"MCMC choose starting point ...";
      Version* current;
     float lrisk; //risk of starting point
  cout<<"continuing ...";
      /// \todo read in last valid version
  current=new Version();
//  Preferences::getPrefs(file.c_str(),"next");//start reading pks
  file_eof=Preferences::versionInit(file.c_str());//start reading pks
  current->params->static2object();
  ergs->v=current;//lv.push_back(current);
  //lrisk=max((float)0,Trial(repeat,current));
  lrisk=Trial(repeat,current);

  cout<<"done"<<endl;
  if (lrisk<0.0)lrisk=0.99;//sollte die vorgegebene PK nicht funktionieren...
     /// one markov chain (length number of generations)
     for (int MC_i=0; MC_i<length; MC_i++){
  cout<<"\nlrisk= "<<lrisk;
       Version *vers1=current;//ergs->v;//current

  cout<<"\nGeneration: "<<MC_i;
       ///repeat testing new versions until (randomly) better than last version
       bool valid=false,better=false; float llrisk;
         current=new Version((const Version)*vers1);
  cout<<"\tID: "<<current->params->o_ID<<endl;
         ///change parameters depending on last set
           current->delRuns();
           current->params->getGNVarPrefSet(*(RangeMin->params),*(RangeMax->params),
             0.25,1);//change only one parameter a step;  1,1 is default
         current->params->object2static();
         //run
         //cout<<"current..size() 1)"<<current->lr->size()<<"\n";
         llrisk=Trial(repeat,current);
         //cout<<"current..size() 2)"<<current->lr->size()<<"\n";

         //compare results
         //better=MRandom::rnd01()<adj*((1-llrisk)/(1-lrisk));
         //better=MRandom::rnd01()<adj*(lrisk/llrisk);//doesn't work if lrisk is zero
         better=MRandom::rnd01()<adj*((lrisk+1e-3)/(llrisk+1e-3));

  cout<<"comp is "<<((lrisk+1e-3)/(llrisk+1e-3))<<endl;
         //'valid' version?
         valid=(llrisk!=-1)&&better;//&&(MRandom::rnd01()<(comp/adj)); //constant tuned to accept 1/4 of trials
         if (!valid){
           //nicht aus der Liste löschen, sondern nur Speicher freigeben
           current->params->addPrefs(newSetsFile.c_str());
           if (current) delete current;current=NULL;
           current=vers1;
           MCMCDoc(docMCFile.c_str(),Preferences::ID,0);//failed
           //MC_i--;//count trial only, if succeeded
     cout<<"failed\n";
         }else{
           current->params->addPrefs(newSetsFile.c_str());
           if (current) delete current;current=NULL;
           current=vers1;
           lrisk=llrisk;
           MCMCDoc(docMCFile.c_str(),Preferences::ID,1);//succeeded
     cout<<"succeeded\n";
         }//end else !valid
         //cout<<"current..size() 3)"<<current->lr->size()<<"\n";

       }//end MC_i
     //if (current) delete current; current=NULL;
    }while (file_eof!=-1);//get pks until eof

  }//end if valid source file
}//end MCMC
//-eof--------------------------------------------------------------------------
