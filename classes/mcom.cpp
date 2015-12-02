/** \file
 \brief contains communication functions with possible GUIs
 \author Katrin Körner
 \date 2008/10/09

 Functions

 Landscape
   -# getPopSizes()
   -# getSeedLoads()
   -# getCurrCapacities()

 Run
   -# getOccupancy()
*/
// class Landscape (file mclasses.h)--------------------------------------
#include "mclasses.h"
  vector<int> Landscape::getPopSizes(int stage) const {
      vector<int> values(this->pop.size());
      if (stage==5) for (int i=0;i<this->pop.size();i++)
         values[i]=pop[i].getIndsReg();
      else for (int i=0;i<this->pop.size();i++)
         values[i]=pop[i].getSummerPop(stage);
      return values;
      }

  vector<int> Landscape::getSeedLoads() const{
      vector<int> values(this->pop.size());
      for (int i=0;i<this->pop.size();i++)
        values[i]=pop[i].getSeeds();
      return values;
      }

  vector<double> Landscape::getCurrCapacities() const{
      vector<double> values(this->pop.size());
      for (int i=0;i<this->pop.size();i++)
        values[i]=pop[i].actCapacity;
      return values;
      }

// class Run (mresults.h)---------------------------------------------------
#include "mresults.h"
  vector<bool> Run::getOccupancy() const{
      return this->cellOccupied;
      }

// eof ---------------------------------------------------
