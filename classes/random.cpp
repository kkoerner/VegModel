//#include "rng.h"
#include "random.h"
RNG *MRandom::randomNb=new RNG();

double MRandom::g_rnd_range(const double minv, const double maxv,
    const double point, const double change){
    //return  std::min(std::max(point+(maxv-minv)*randomNb->normal(0,change/6),minv),maxv);
    double value=point;
    if (minv!=maxv) do {value=point+(maxv-minv)*randomNb->normal(0,change/6);}
      while (value<=minv||value>=maxv);
    return  value;
  }
