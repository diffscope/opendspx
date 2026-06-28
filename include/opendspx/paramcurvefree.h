#ifndef OPENDSPX_MODEL_PARAMCURVEFREE_H
#define OPENDSPX_MODEL_PARAMCURVEFREE_H

#include <vector>
#include <utility>

#include <opendspx/paramcurve.h>

namespace opendspx {

    struct ParamCurveFree : ParamCurve {
        ParamCurveFree(int start = 0, int step = 5, std::vector<int> values = {})
            : ParamCurve(Free, start), step(step), values(std::move(values)) {
        }

        int step;
        std::vector<int> values;
    };
    
    using ParamCurveFreeRef = std::shared_ptr<ParamCurveFree>;

}

#endif //OPENDSPX_MODEL_PARAMCURVEFREE_H