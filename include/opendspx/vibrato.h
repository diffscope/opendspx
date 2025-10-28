#ifndef OPENDSPX_MODEL_VIBRATO_H
#define OPENDSPX_MODEL_VIBRATO_H

#include <opendspx/vibratopoints.h>

namespace QDspx {

    struct Vibrato {
        double start{0.0};
        double end{1.0};
        int amp{0};
        double freq{0.0};
        double phase{0.0};
        int offset{0};
        VibratoPoints points;
    };

}

#endif //OPENDSPX_MODEL_VIBRATO_H