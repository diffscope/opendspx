#ifndef OPENDSPX_MODEL_TIMELINE_H
#define OPENDSPX_MODEL_TIMELINE_H

#include <vector>

#include <opendspx/label.h>
#include <opendspx/tempo.h>
#include <opendspx/timesignature.h>

namespace opendspx {

    struct Timeline {
        std::vector<Label> labels;
        std::vector<Tempo> tempos;
        std::vector<TimeSignature> timeSignatures;
    };

}

#endif //OPENDSPX_MODEL_TIMELINE_H