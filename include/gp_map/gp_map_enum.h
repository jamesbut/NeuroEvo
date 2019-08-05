#ifndef _GPMAP_ENUM_H_
#define _GPMAP_ENUM_H_

/*
    Specifies the indentifiers for the different
    GPMaps.
    This has to be declated in a separate header
    fil because of circular dependancy issues,
*/

namespace NeuroEvo {
namespace GPMaps {

enum gpmaps {
    NoMap,
    Matrix,
    InverseDCT
};

} // namespapce GPMaps
} // namespace NeuroEvo

#endif
