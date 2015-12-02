#ifndef mclassesH
#define mclassesH
/** \file
\brief Declaration of classes Landscape and Climate

\author Katrin Körner

\date 15.3.06
- ausmisten
\date 23.3.06
- Änderung Results-Klasse--> setPopulations() ohne Argumente
\date 01.06.06
- class Feld löschen
- statische Variablen in class Preferences ('prefs.h')
\date 20.9.07
- ad init-time for disturbances and weather changes
*/

#include <vector>
#include "species.h"

/// \brief   Weather and Climate
/**
class Climate
...definiert das Climaszenario und berechnet die jährlichen Wetterbedingungen
- in Version 0.0-0.06 nur ein doube-Wert (actYear) zwischen 0 und 1 enthalten als Qualitätsfaktor
- ab Version 0.07 Wetter in Vector regionalisiert; Wetterwert auf Zelle basierend
- für die Juncus-clima-versuche wird die Funktion der Klasse erweitert: es wird
  eine Init-Phase eingefügt, nach der sich Parameter ändern

- calcWeather(): berechnet das jährliche Wetter und gibt actYear zurück
- getWeather(index): gibt das regionale Wetter(index) zurück
*/
class Climate{
  static float actYear[6];
  //vector<double> regWeather;    ///<regionales Wetter
public:
  Climate(){};
  ~Climate(){};
  /// calculates annual weather conditions
  double calcWeather(void);
  /// calculates annual weather conditions at initialization time
  double calcWeatherInit(void);
  /// returns current weather conditions for a specified demographic process
  double getWeather(int,int);
}   ;

//-class Landscape----------------------------------------------------
///  \brief spatial definition of habitat quality
/**
class Landscape  -alles public-
...dient als Grundlage für die Modellfunktionen
- hier ist das Klima (T Climate*) und das zweidimensionale Raster (T Feld**) lokalisiert
- von hier aus werden die Populationen initiiert (setPopulations())
                      und Samen verbreitet(seedDispersal)
- oneYear() beschreibt einen Jahreslauf (Zeitschritt) des Modells
*/
class Landscape{
  /**\addtogroup hab_dyn   */
  //@{
  void Disturb(unsigned i);
  void Success(unsigned i);
  void NextInPatch(unsigned int i,bool cDist,float habType,vector<bool>& lcheck);
  //@}
public:
  Climate * clima;
  vector<Population> pop;

  ///LDDispersal-Felder
  struct Tvert{
     vector<int> xCoord;
     vector<int> yCoord;
  }verteilung;

  Landscape();
  ~Landscape();

  void setSize(int x, int y);

  /**\addtogroup hab_dyn Habitat Dynamics
    simulating dynamics of habitat capacities on landscape scale
  */
  //@{
  /// test for habitat value beeing arable field id
  static bool isArableField(int value){if (value==913||value==914||value==915)return true;else return false;};
  /// reads single Landscape from File and stores it in Preferences::MusterCapacity
  static void loadLandscape(const char* file);
  /// reads sample Cells from File and stores it in Preferences::MusterCapacity
  static void loadResultCells(const char* file);
  /// calculates Sukzession, management and stochastic disturbance events
  void patchDynamics(char* method="random", bool init=false );
  /// calculate dynamic landscape with autocorrelated disturbances on patch scale
  void patchDynamics_corr(bool disturb=true);
  //@}


  /** \addtogroup spat    */
  //@{
  /// yearly local population dynamics
  void oneYear(bool init=false,bool disturbFlag=true);
  //@}

  /**\addtogroup hab_dyn  */
  //@{
  void setPopulations();// char* file, int
  //@}

  /**\addtogroup spat Spatial interaction */
  //@{
  void initVerteilung(int);
  void seedDispersal(int);  ///< Verbreitet zu verteilende Samen
  //@}

  /**\addtogroup spatc Spatial communication */
  //@{
  ///returns a grid with current population sizes
  vector<int> getPopSizes(int stage=5) const;
  ///returns a grid with current seed bank loads
  vector<int> getSeedLoads() const;
  ///returns a grid with current Capacities
  vector<double> getCurrCapacities() const;
  //@}

}  ;

#endif
