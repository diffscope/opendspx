#ifndef OPENDSPX_MODEL_VIBRATOPOINTS_H
#define OPENDSPX_MODEL_VIBRATOPOINTS_H

#include <vector>

#include <opendspx/controlpoint.h>

namespace opendspx {

    struct VibratoPoints {
        std::vector<ControlPoint> amp;
        std::vector<ControlPoint> freq;
    };

}

#endif //OPENDSPX_MODEL_VIBRATOPOINTS_H