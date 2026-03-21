#ifndef OPENDSPX_MODEL_TIMESIGNATURE_H
#define OPENDSPX_MODEL_TIMESIGNATURE_H

namespace QDspx {

    struct TimeSignature {
        int index{0};
        int numerator{4};
        int denominator{4};
    };

}

#endif //OPENDSPX_MODEL_TIMESIGNATURE_H