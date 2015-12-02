#include <cstdlib>
#include <iostream>

//#include <math.h>
#include "classes/mclasses.h"
#include "classes/mresults.h"
#include "classes/prefs.h"
#include "classes/random.h"//#include "rng.h"
using namespace std;
//---------------------------------------------------------------------------
/**
  \file
  \brief vegetation model (C++ Standard, console)
*/

/*! \mainpage
 *
 * \author Katrin K�rner
 * \version 3.01x
 * \date 2008-02-03
 * \par Type
 *  simulation model
 *
 * \section intro_sec Introduction
 *
 * \section verbal_descr Verbal Description of what the Code does
 *
 * \par Flow Chart
 *   \image html flow-small.GIF
 *
 * \section exp_input Expected Input
 *  Starting the Application needs a PK-source file name as argument. This file
 *  contains a tab-delimited constant structure of parameter values, Parameter
 *  sets are separated by carriage return. to comment out a line, set a '#'
 *  character first after newline.
 *
 *  Additionally the model needs a ASCII coded landscape file
 *  (see section \ref lc_file) with a structure generated by my Landscape Generator.
 *
 * \subsection lc_file Landscapes
 *  add here: structure of Landscapes-file
 * \subsection pk_file Parameter Sets
 *  add here: structure of Parameter-file
 *
 * \section output Output
 *  An Output-File in ASCII-text format is written containing
 *  summaries for each %Run a line. column names are:
 *    - \b ID            \t ID of Parameter Set
 *    - \b %Run          \t Number of %Run
 *    - \b D             \t fractal dimension of landscape used
 *    - \b Time          \t %Run time (<MaxTime if went extinct)
 *    - \b MaxTime
 *    - \b Nmp           \t mean number of Individuals at the entire grid
 *    - \b minAntBes     \t minimum portion occupied habitats during runtime (first 200 years excluded)
 *    - \b AntBes        \t mean portion occupied habitats
 *    - \b IndsPerPop    \t mean Number of Individuals per occupied habitat
 *    - \b ExtRate       \t mean extinction rate
 *    - \b ColRate       \t mean colonization rate
 *    - \b riskValue     \t (AntBes-minAntBes)/AntBes
 * \par Requirements and Environment
 *  Dev C++, Linux standard Gnu C++ compiler (hopefully)
 *
 * \section sensi Sensitivity Analysis
 *  do add later
 *
 * \section vali Validation
 *  to be validated e.g. with Quillow-Data-Set
 *
 * \section source Sources
 *
 * \section refs Publications and Applications referring to the code
 *  Koerner and Jeltsch (2008). Ecological Modelling
 *
 * \note Chapters 'Bugs' and 'Flow Chart' are included
 *         elsewhere in this Documentation
 */

Results* ergs=new Results();
const int   initTime=200;   //initialization time
string FileName="";

void OneFile();
void OneRun(Version*);

//------MAIN---------------------------------------------------
/// main to run one parameter file
int main(int argc, char *argv[])
{
   FileName=(string)"ptyp.dat";
   if (argc>=2) {FileName=(string) argv[1];}
   //rastert_allregi1.aig//Q-lcs-dim3-ph1.gtg
   Results::iFileLandscape=(string)"qll-hgugdom-200.aig";
//   if (argc>=3) {
//      int lsc=atoi(argv[2]);
//      switch (lsc){
//        case 1:Results::iFileLandscape=(string)"Q-lcs-dim1-ph1.gtg";break;
//        case 2:Results::iFileLandscape=(string)"Q-lcs-dim2-ph1.gtg";break;
//        case 3:Results::iFileLandscape=(string)"Q-lcs-dim3-ph1.gtg";break;
//        case 4:Results::iFileLandscape=(string)"Q-lcs-dim1-ph2.gtg";break;
//        case 5:Results::iFileLandscape=(string)"Q-lcs-dim2-ph2.gtg";break;
//        case 6:Results::iFileLandscape=(string)"Q-lcs-dim3-ph2.gtg";break;
//        default:Results::iFileLandscape=(string)"";break;
//      }}

   /// get landscape file...
   Landscape::loadLandscape(Results::iFileLandscape.c_str());//only for realistic landscapes
   /// get result cells...
   Landscape::loadResultCells("qll_buff_1km-200.txt");//ignore buffer zone (1km)

   OneFile(); ///Version to run simulations

   if (ergs) delete ergs; ergs=0;
   if (false) system("PAUSE"); //no interaction needed
   return EXIT_SUCCESS;
}
//---------------------------------------------------------
///Run a whole File
/**
  run a file previosly generated or written. This File
  should contain a list of parameter sets separated by a newline-character.
*/
void  OneFile()
{
//  Results::iFileLandscape=(string)"g-rare-30.gtg";//ifile2;                 // path+

  //read parameter ranges
  if(FileName.length()>3)
  {
   // chdir(path);
    string file=FileName.c_str();
    Results::oFile=file;
    Results::oFile=Results::oFile.replace(Results::oFile.rfind("."),4,".txt").c_str() ;
   // string newSetsFile=ChangeFileExt(file,"-n.dat").c_str();//(string) path+"\\newSets.dat";   // path+
    //Results::setOFile();
    short eoFile=0;
    //Ergebnisklasse initialisieren
    while (Preferences::versionInit(file.c_str())==0){
      Version* current=new Version();
      //ergs->lv.push_back(current); //ergs->iv=ergs->lv->Count-1;

//      if (ergs->v) delete (ergs->v);
      (*ergs->v)=*current;//(ergs->lv.at(ergs->lv.size()-1));
      delete current;
      unsigned int max= ergs->v->maxRuns;
      //Wiederholungen
      do{
        OneRun(ergs->v);     // <-- hier l�uft eine Wiederholung
      }while(ergs->v->GetNORuns()<max);//Ende Run
      //ergs->freeMem();// \bug
    }//end while versions
  cout<<"---- Ende der L�ufe ----\n" ;
  }//end if file given
} //end OneFile
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

void OneRun(Version* cVers)
{
   cVers->initRun();
//   ergs->v=(Version*) ergs->lv.at(ergs->lv.size()-1);
   Run* locr= (Run*) cVers->GetLastRun();// >lr->at(ergs->v->lr->size()-1);

   /// create Landscape
   Landscape* region= new Landscape();
   region->setPopulations();
   //Zeitschritte
   int time=0;
   /// loop time steps
   do{
//cout<<".";
     time=locr->initYear();
     if (time<initTime) region->oneYear(true);            // <-- hier l�uft ein Zeischritt
                                                          //     (mit Init-time)
     else region->oneYear(false);
     /// get results of year
     int test=locr->a->back().yearly_selcells(
         region,locr->cellOccupied,time);//,&(locr->cellState),&(locr->managed)
     if(test==-1) {
       cout<<">>> Pops ausgestorben!!\n";
       break;                   //abbrechen, wenn Metapop ausgestorben
     }
     if(test==-2) {
       cout<<">>> zu viel Inds!!\n";
       break;                   //abbrechen, wenn Metapop ausgestorben
     }
   }while(time<Preferences::maxYears); //Ende Time   ergs->v->

   /// get and write results of run
    /// Output Time Slides
    /// if timeToExt<250 summary of last year
   locr->runly_steps(
     region,Preferences::maxYears,Preferences::ID,cVers->GetNORuns(),10);
   locr->runly_steps(
     region,Preferences::maxYears,Preferences::ID,cVers->GetNORuns());
    /// Output Time to Extinction
   locr->runly(region,Preferences::maxYears,Preferences::ID,cVers->GetNORuns());
   //locr->freeMem();

   delete region; region=0;
}
//---------------------------------------------------------------------------