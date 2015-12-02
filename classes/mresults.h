#ifndef mresultsH
#define mresultsH
/** \file
  \brief Headerdatei für die Auswertungsklasse des Metapopulationsmodells

  ..zur Auswertung der Simulationen und Darstellung der Ergebnisse auf der Form
    excl. Schreiben in Dateien (siehe fileout.cpp)

  \author Katrin Körner

  \date 15.03.06  - ausmisten
  \date 16.03.06  - Klasse strukturieren
  \date 07.02.08  - V3.01x add comments
  \date 16.09.08  - version mit ressourcenschonung
  */
#include <vector>
#include <string>
#include "mclasses.h"
using namespace std;

/// Deklaration der Auswerteklasse für ein Jahr
class Year{
public:
  double long metapopCapacity;///< sum of local habitat capacities
  double long sumOfInds;      ///< sum of Individuals on LC
  double long sumOfSeeds;     ///< sum of Seeds on LC
  int sumExtinct;             ///< Zahl erloschener Populationen
  int sumColonized;           ///< Zahl neu etablierter Populationen
  int potHab;                 ///< # Potentieller Habitate
  int occupied;               ///< # besetzter Habitate
  int freeHabs;               ///< # freier, geeigneter Habitate
  int flux;                   ///< sum of extinction and colonization events
  double indsPerPop;          ///< mean population size
  double crashesPerYear;      ///< mean of annual population crashes
//  vector<int> cellState;      ///< current occupancy of landscape
//  vector<bool> managed;       ///< managed habitats

  Year();                        ///< constructor
//  ~Year(){cellState.clear();};   ///< Destructor
  Year(const Year&);             ///< Copy-Constructor
  Year& operator=(const Year&);  ///< assignment operator
  /// calculate annual summaries
  int yearly(Landscape * lregion,vector<bool>& cellOccupied, int);
  int yearly_selcells(Landscape * lregion,vector<bool>& cellOccupied, int);//,vector<bool>& cellsToAnalyze
};

/// Deklaration der Auswerteklasse für ein Run
class Run{
public:
  vector<Year> *a;                    ///< vector of years
  int timeToExt;                     ///< time to extinction
  int maxYears;                      ///< maximum number of years to simulate
  int keycode;                       ///< sample site for documentation (obsolete)
  double dimension;                  ///< fractal dimension of LC (obsolete)
  vector<bool> cellOccupied;         ///<Belegungsmuster des Grids
  //Output Values
  double long sumOfInds;             ///< O-Var: msuminds
  double   lambda;
  double   minOccupPops;             ///< O-Var: minbes
  double   occupPops;                ///< O-Var: aoccup
  double   indsPerPop;               ///< O-Var: mIndsPerPop
  double   extRate,colRate;          ///< O-Var: extRate,colRate
  double   turnOver,turnOverRate;    ///< O-Var: Turnover,Turnoverrate
  double   varSumOfInds,varIndsPerPop;///< O-Var: s2suminds,s2indsperpop
  double   varExtRate,varColRate;    ///< O-Var: s2extrate,s2colrate
  double   fluxRate;                 ///< O-Var:  Ext+Col/occup
  float    gmlambda;                 ///< maxLambda(20)
  double   crashesPer100;            ///< mean number of crashing years in 100Years
  Run();                             ///< constructor
  ~Run();//{if (a) delete a;};
  Run(const Run&);             ///< Copy-Constructor
  Run& operator=(const Run&);  ///< assignment operator
  int initYear(void);               // /<
/// calculate summaries per run
  char runly(Landscape * lregion, int,int,int);
  /// calculate summaries per run (several time slides)
  void runly_steps(Landscape * lregion,const int,const int,int,
                const int lap=50, const int slides=5);
  /// compares the results of two Runs
  static float compare(Run&,Run&,string v="risk");
  ///maximum population growth rate in the first time years
  float gmLambda(int time=20);
  /// returns the risk value of a run
  float risk(){
    //return (occupPops-minOccupPops)/(occupPops+1e-10);
    return crashesPer100;
    };

  /// clear vector<> a for memory saving
  void freeMem(){a->clear();};
/// \addtogroup spatc
//@{
  /// returns a grid of Occupancy states
  vector<bool> getOccupancy() const;
//@}
};

/// Deklaration der Auswerteklasse für eine Version
class Version{
public:
  long int ID;           ///< version ID
  int maxRuns;           ///< number of replicate runs
  vector<Run*> *lr;       ///< contains a list of runs
  Run *r;                ///< link to current run
  Preferences *params;   ///< contains parameterset
  Version();             ///< constructor
  /// copy constructor
  Version(const Version& old);
  ~Version();            ///< destructor
  /// assignment operator
  Version& operator=(const Version& x);
  /// compares the results of two Runs
  static float compare(Version&,Version&);
  ///returns number of runs
  int initRun(void);                // /<
  int GetNORuns()const{return lr->size();};
  Run* GetRunX(int x){return lr->at(x);};/// \todo thow exception if out_of_bounds
  Run* GetLastRun(){return lr->at(GetNORuns()-1);};
  /// clear existing runs
  void delRuns();
};

/// Deklaration der Klasse Results
/**
  entält Funktionen zum Start, Verwaltung und Auwertung der Simulationen
*/
class Results {
public:
  /** \name initialization functions
    read Preferences and reset variables for new Runs, Years...
  */
  //@{
  int setPreferences();             ///< get Parameter Set
  int initVersion(void);              // /<
  //@}

  /** \name file output-functions
    file output of grid, sites or yearly summary
  */
  //@{
  void ausgabeGrid(vector<float>& grid, int, char* datei);
  void ausgabeGrid(vector<int>& grid, int, char* datei);
  void ausgabeYears(char* datei);
  void ausgabeYear(char* datei, Landscape * lregion);
  void docLocalPops(const char* datei, Landscape&);
  static void setOFile();
  static void addToOFile();
  //@}

  /// choose a grid cell for documentation
  void getKeycode(Landscape * lregion);

  static string path;                  ///< global reference path for file output
  static string iFileLandscape;        ///< name if Landscape file
  static string iFilePreferences;      ///< name of Preferences file
  unsigned long int prefFilePos;       ///< file position in Preferences-file
  bool eofPref;                        ///< eof flag of Pref-file
  static string oFile;                 ///< name of Output file
//  vector<Version*> lv;                 ///< contains a list of versions
  Version *v;                          ///< link to current version
  /// indices for current version, run, and year
  //static unsigned int iv,ir,ia;
  Results();                           ///< constructor
  /// destructor
  /** no explicit destructor needed, because
    v is only a copy of the last element in lv
  */
  ~Results();
  /// free memory on Version-level (how many Versions to remember)
  void freeMem(const int remember=0);
};
#endif
