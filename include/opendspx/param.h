#ifndef OPENDSPX_MODEL_PARAM_H
#define OPENDSPX_MODEL_PARAM_H

#include <vector>

#include <opendspx/paramcurve.h>
#include <opendspx/paramcurveanchor.h>
#include <opendspx/paramcurvefree.h>

namespace opendspx {

    struct Param {
        std::vector<ParamCurveRef> original;
        std::vector<ParamCurveRef> transform;
        std::vector<ParamCurveRef> edited;
    };

}

#endif //OPENDSPX_MODEL_PARAM_H