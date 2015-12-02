//---------------------------------------------------------------------------
/**
  \file
  \brief local vegetation model (C++-standard, console)
*/

/*! \mainpage
 *
 * \author Katrin Körner
 * \version 3.0x
 * \date 2008-02-07
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
 *  Starting the Application needs one of the following source files:
 *  - a source file name: the preferences file
 *
 *
 * \section output Output
 *  An Output-File in ASCII-text format is written containing
 *
 * \par Requirements and Environment
 *  Dev C++
 *
 * \section sensi Sensitivity Analysis
 *  do add later
 *
 * \section vali Validation
 *
 * \section source Sources
 *
 * \section refs Publications and Applications referring to the code
 *  not yet -but coming soon
 *
 * \note Chapters 'Bugs' and 'Flow Chart' are included
 *         elsewhere in this Documentation
 */
#include <cstdlib>
#include <iostream>
#include <sstream>
//#include <dir.h>
#include <fstream>

//#include "prefs.h"
//#include "species.h"
#include "classes/mresults.h"
using namespace std;

//---------------------------------------------------------------------------

Results* ergs   =new Results(); //< initiate result object
//const bool unGUI=false;
int initTime    =200;           //< number of years for initialzation
string FileName ="";            //< file name of preferences file
bool printInit=false;           //< flag whether to monitor initial time steps too

void OneFile();
void OneRun(Version*);
void DisplayAndWrite(Landscape&, int);

//---------------------------------------------------------------------------
///run simulations and monitor local sites
/**
  Run a file previosly generated or written and given as fist program argument.
  This File should contain a list of parameter sets separated by a
  newline-character.
  With an optional secound argument 'init' population history of initial years
  is documented too.

*/
int main(int argc, char *argv[])
{
  if (argc>=2) FileName=argv[1];
  if (argc>=3) {
    if (argv[2]==(string)"init") printInit=true;
  }

  Results::iFileLandscape="qll-hgug100-a.aig";//ifile2;
  /// get landscape file...
  Landscape::loadLandscape(Results::iFileLandscape.c_str());//only for realistic landscapes
  /// get result cells...
//  Landscape::loadResultCells("qll_buff_1km-sites-100.aig");//as in mcmc; ignore buffer zone (1km)
  Landscape::loadResultCells("qll_buff_1km-points-100.aig");//compare with empirical data; ignore buffer zone (1km)


  OneFile(); ///Version to run simulations
  if (ergs) delete ergs; ergs=0;

  if (false) system("PAUSE");//no interaction needed
  return EXIT_SUCCESS;
}
//---------------------------------------------------------------------------
/// local site monitoring
/**
  writes annual site properties to a text file
*/
void DisplayAndWrite(Landscape &lc, int ly)
{
  stringstream strstream;
  strstream << Preferences::ID;
  string ziel=string("monitor_")+strstream.str()+string(".txt");
  ergs->docLocalPops(ziel.c_str(),lc);
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
        OneRun(ergs->v);     // <-- hier läuft eine Wiederholung
      }while(ergs->v->GetNORuns()<max);//Ende Run
      //ergs->freeMem();// \bug
    }//end while versions
  cout<<"---- End of Runs ----\n" ;
  }//end if file given
} //end OneFile
//---------------------------------------------------------------------------
///  one simulation run
/**
  function doing one simulation run. A new Run-object is added to the global
  results-object. Then the simulation is initiated, and successive years/
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
  // ergs->v=(Version*) ergs->lv.at(ergs->lv.size()-1);
   Run* locr= (Run*) cVers->GetLastRun();

   //Umgebung (Landscape) erstellen
   Landscape* region= new Landscape();
   region->setPopulations();
   //Zeitschritte
   int time=0;
   do{
     time=locr->initYear();
     if (time<initTime) region->oneYear(true,true);  // <-- hier läuft ein Zeischritt
                                                //     (mit Init-time),2nd arg = disturbances yes or no
     else region->oneYear(false,true);//     (außerhalb der Init-time),2nd arg = disturbances yes or no

     //Auswertung
     int test=locr->a->back().yearly_selcells(
         region,locr->cellOccupied,time);
     if ((test<0)||(time>=Preferences::InitTime||printInit))
       DisplayAndWrite(*region,time);           //<Write Results
     if(test==-1&&time>30) {//suvive at least first 30 years (standard is 10)
       cout<<">>> Pops ausgestorben!!\n";
       break;                   //abbrechen, wenn Metapop ausgestorben
     }
     if(test==-2) {
       cout<<">>> zu viel Inds!!\n";
///comment out -- not stopping because of overflow
//       break;                   //abbrechen, wenn overgrowth
     }
   }while(time<Preferences::maxYears);          //Ende Time
   //Auswertung nach einer Wiederholung (run)
   locr->runly(
     region,Preferences::maxYears,Preferences::ID,cVers->GetNORuns());
   //locr->freeMem();
   delete region;region=0;
}
//-eof--------------------------------------------------------------------------
