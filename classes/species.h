//---------------------------------------------------------------------------

#ifndef speciesH
#define speciesH
//---------------------------------------------------------------------------
#include "prefs.h"
/** \file
\brief  Definition of class Population

\author Katrin K�rner
\date 15.3.06
- ausmisten
----------------------------------------------------------
*/

  //export enum state{SEED,SEEDL,JUV,VEG,GEN};

/** \brief lokale Populationsdynamik und Klassenvariablen

class Population
...enth�lt sowohl die Eigenschaften der betrachteten Art in statischen Variablen, wie auch
die Eigenschaften der konkreten Populationen (Objekte) und die Funktionen zu ihrere Manipulierung
- Arteigenschaften -static-
  - transitions[5][5] �bergangsrate in verschiedene Stadien
      [seed][seedling]        Keimrate (0-1): 1.0
      [seedling][generativ]   Etablierungserfolg bis zur generativen Phase (0-1): 1.0
      [generativ][seed]       produzierte Samen pro Individuum (double): 100.0
      [seed][seed]            Samenmortalit�t (0-1): 0.1
      [generativ][generativ]  Mortalit�t der Individuen in einem Zeitschritt(0-1): 1(ann)
  - dispersed:       Anteil Samen, der au�erh. des Feldes/ der Popul. verbreitet wird (0-1): 0.1

- Populationseigenschaften
  - seedProd         Samenproduktion (Integer) des aktuellen Jahres
  - adults           Summe der im aktuellen Jahr reproduzierenden Individuen
  - maxCapacity      maximale Kapazit�t C
  - actCapacity      aktuelle Feldkapazit�t K
  - stadium[5]       Gr��e der aktuellen Population, aufgeteilt in Stadien[5]

public:
- Funktionen
  - getInds(): gibt die Summe aller Individuen >Keimlinge zur�ck
  - setInds(int): f�r Initialisierung: setzt Individuenzahl einer Population
  - germEstab(): berechnet Keimung und Etablierung; aktual. N; gibt Zahl gekeimter Samen zur�ck
  - seedProduction(wetter): berechnet Samenproduktion des Jahres; gibt seedProd zur�ck
  - seedMort(): berechnet Samenmortalit�t; gibt Zahl gestorbener Samen zur�ck
  - mortality(): berechnet die Mortalit�t und aktualisiert N
*/
class Population{
   //Zahl Inds; fluktuierend
  //enum Stadien {seeds,seedlings,juvenile,vegetativ,generativ} ;

  long stadium[5];   ///<Inhalt: Zahl der Individuen in den einzelnen Stadien
  long summerpop[5]; ///<#der Inds zur Reproduktionszeit
  long seedProd;
  long indsReg; ///<Individuen zur Regenerationszeit - Zwischenspeicher -

public:
//typedef state state;
//enum state{SEED,SEEDL,JUV,VEG,GEN};
  float actCapacity;
  float maxCapacity;
  long adults;        ///<zahl reproduktiver Individuen
  bool managed;
  bool disturbed;

  void init(void);
  long getIndsReg(void) const {return indsReg;};///< #Inds zur Samenproduktionszeit
  void resetIndsReg(void){indsReg=0;};
  long getInds(void) const;
  void resetInds(int);
  long getSeeds() const;
  void addSeeds(long, int);
  long getSeedProd(void) const {return seedProd;};
  long getStadium(state) const;
//  long getSummerPop(state);
  long getSummerPop(int) const;

  Population();
  ~Population(){};
/** \addtogroup demogr Demography
    \brief functions of local plant demography
*/
/// \{
  void aging(float);
  void retrogression(float);
  void seedProduction(float); ///<Errechnet Samenproduktion
  void clonalGrowth(float);
  void mortality(float,float);
  void germEstab(float);
  void seedMort(void);
/// \}
}  ;
//----------------------------------------------------------------------------
#endif
