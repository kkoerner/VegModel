#ifndef RANDOM_H
#define RANDOM_H
/** \file
  \brief Headerfile for MRandom class

  MRandom class contains functions generating several kinds of random
  distributions. It is used for evenly and gaussian distributed values in the
  Vegetation model. The RNG class of Kenneth Wilder
  (\link http://finmath.uchicago.edu/~wilder/Code/random/) is applied here.
  \warning this pseudo random number generator is not fully 64bit-tested


  \author Katrin Körner

  \date 22.11.2007
 */

#include <algorithm>
#include "rng.h"

/// random number class
struct MRandom{
  static RNG *randomNb;
/// Functions to generate random numbers
//@{
//-uniform distributions----------------------------------------------------------------
  ///uniform random number between 0 and 1
  static inline double rnd01(){return randomNb->uniform();}//return rand()/(double)RAND_MAX;}
  ///uniform random number within a given range
  static inline double rnd_range(const double minv,const double maxv,const double change=1){
                            return (minv+maxv)/2.0 + change*(maxv-minv)*(rnd01()-0.5);
  }
  ///log-uniform random number within a given range
  static inline double rnd_log_range(const double minv,const double maxv,const double change=1){
    double lminv=log10(minv); double lmaxv=log10(maxv);
    return pow(10,rnd_range(lminv,lmaxv));
  }
//-normal distributions-----------------------------------------------------------------
  ///normal distributed random number relative to a given point and between given bounds
  static double g_rnd_range(const double minv, const double maxv,
    const double point, const double change=1);
 //   {
 //   //return  std::min(std::max(point+(maxv-minv)*randomNb->normal(0,change/6),minv),maxv);
 //   double value;
 //   do {value=point+(maxv-minv)*randomNb->normal(0,change/6);}
 //     while (value<=minv||value>=maxv);
 //   return  value;
 // }
  ///log-normal distributed random number relative to a given point and between given bounds
  static inline double g_rnd_log_range(const double minv, const double maxv,
    const double point, const double change=1){
    double lminv=log10(minv); double lmaxv=log10(maxv); double lpoint=log10(point);
    return  pow(10,g_rnd_range(lminv,lmaxv,lpoint));
  }
//@}
};

#endif //RANDOM_H
