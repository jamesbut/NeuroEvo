#ifndef _NORMALISATION_H_
#define _NORMALISATION_H_

/*
 * A collection of functions that perform normalisation
 */

namespace NeuroEvo {

//Normalises some value from an original range [orig_min, orig_max] for a new range
//[new_min, new_max]
double normalise(const double value, const double orig_min, const double orig_max,
                 const double new_min, const double new_max);



} // namespace NeuroEvo

#endif
