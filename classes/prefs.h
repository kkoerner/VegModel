//---------------------------------------------------------------------------

#ifndef prefsH
#define prefsH
//----------------------------------
//#include <vcl>
#include <string>
#include<vector>
using namespace std;
/**\file
   \brief declaration of class Preferences
*/

//  enum eMode{NONE=0,COMPENSE,OVERCOMP};
  /// good and bad weather conditions
  enum gb{GOOD,BAD};
  /// processes to be influenced by weather
  enum process{GERM,D1,PROD,D2,SURV};
  // state;//{SEED,SEEDL,JUV,VEG,GEN};
  /// morphological states a plant can be assigned to
  enum state{SEED,SEEDL,JUV,VEG,GEN};

/** \brief Class that stores parameter sets

   class, preferences from MetaModel are stored
   for each version
    - reads/writes data from/to file
    - stores parameter data
    - provides <schnittstelle> for other classes
*/
class Preferences{
private:
//  vector<float> maxCapacity;
  // / Habitat preferences

///\name species specific...
//@{
  static double transitions[5][5];
  double o_transitions[5][5]; ///< Übergangsmatrix (eigene funktionsweise)

  float get_p1()const;
  float get_p2()const;
  float get_p3()const;
  float get_gr1()const;
  float get_gr2()const;
  int set_p1(float);
  int set_p2(float);
  int set_p3(float);
  int set_gr1(float);
  int set_gr2(float);
//@}

  ///\name Weather
  //@{
  static double weather[2][5];///< Array für Wetterqualität
  double o_weather[2][5];///< Array für Wetterqualität
   //@}
  //@{
  ///\name management
  static double man[5] ;///< Array für Managementeinfluss
  double o_man[5];///< Array für Managementeinfluss
   //@}

public:  //@{     //@}
  static int ID;int o_ID;
  /// -->class Version
  static int maxRuns;static int maxYears;
  static int InitTime;

  ///\name Habitat preferences
  //@{
  static int MaxX;    //--geht nicht hier zu definieren
  static int MaxY;
  static vector<float> MusterCapacity;  ///<Landschaftsgrid laut File
  static vector<bool> MusterResultCells;  ///<Grid der SampleCells laut File

  static double pcPotHab;
  double o_pcPotHab;
  static double pcInitPop;
  double o_pcInitPop;

  static double patchDecreaseRate;
  double o_patchDecreaseRate;

  static double disturbanceRate;
  double o_disturbanceRate;
 //@}

///\name species specific...
//@{
  static double dispersed; ///< Anteil verbreiteter Samen
  double o_dispersed; ///< Anteil verbreiteter Samen

  static double compAbil;  ///< competetive ability high:+x neutral:0 low:-x
  double o_compAbil;  ///< competetive ability high:+x neutral:0 low:-x
   //@}
  static double dimension;
  static string path;
//  static AnsiString iFileLandscape;
//  static AnsiString iFilePreferences;
//  static AnsiString oFile;
  static string startFile;

//functions:
  Preferences();///<Konstruktor
  Preferences(const Preferences&);
  bool const operator==(const Preferences&);


///\name generate new parameter sets
//@{
  ///generate a new set by giving values ranges
  void getNewPrefSet(const Preferences&, const Preferences&);
  void getNewPrefSet(const Preferences*, const Preferences*);
  ///varying a set by giving values ranges
  void getVarPrefSet(const Preferences&, const Preferences&, const float change=1);
  void getVarPrefSet(const Preferences*, const Preferences*, const float change=1);
  ///varying one parameter by giving values ranges (normal distribution)
  void getGVarPrefSet(const Preferences&, const Preferences&, const float change=1);
  void getGVarPrefSet(const Preferences*, const Preferences*, const float change=1);
  ///varying several parameters by giving values ranges (normal distribution)
  void getGNVarPrefSet(const Preferences&, const Preferences&,
       const float change=1, const int nchange=1);
  void getGNVarPrefSet(const Preferences*, const Preferences*,
       const float change=1, const int nchange=1);
//@}

  //  Preferences(char*,int);
  void static2object(void);
  void object2static(void);

  static short versionInit(const char*  ifile);
  static short getPrefs(const char* iFile, string method);//"next" or "last"
  void addPrefs(const char* ofile);
  /// stages of plant ontogenesis
  static void  setTransitions(float[5][5]);
 // static void  setTransition(state,state,float);
  static float getTransition(state,state);
  float getOTransition(state,state)const;
  void setOTransition(state,state,float);
  void changeOTransition(state,state,float,float,float);

/*
    static void  setTransition(state,state,float);
  static float getTransition(state,state);
  float getOTransition(state,state);
  void setOTransition(state,state,float);

*/
  /**\name IO-Functions */
  //@{
  static int   setPcQuality(gb,process,float);
  static float getPcQuality(gb,process);
  float getOPcQuality(gb,process);
  void setOPcQuality(gb,process,float);
  //@}

 /**\name management options*/
  //@{
//management
  /// \date 14.2.08 manMode changed to probability of current year's management
  static float manMode;//eMode manMode;
  float o_manMode;//eMode o_manMode;
  static double manEffect;double o_manEffect;  ///<effect of Management= level of Cmax that is saved by management

  static void  setMan(process,float);
  static float getMan(process);
  float getOMan(process);
  void  setOMan(process,float);
  //@}
};
//bool operator==(Preferences&,Preferences&);
//---------------------------------------------------------------------------
#endif
