#ifndef OPENDSPX_MODEL_PARAMCURVEFREE_H
#define OPENDSPX_MODEL_PARAMCURVEFREE_H

#include <QList>

#include <opendspx/paramcurve.h>

namespace QDspx {

    struct ParamCurveFree : ParamCurve {
        ParamCurveFree(int start = 0, int step = 5)
            : ParamCurve(Free, start), step(step) {
        }

        int step;
        QList<int> values;
    };
    
    using ParamCurveFreeRef = QSharedPointer<ParamCurveFree>;

}

#endif //OPENDSPX_MODEL_PARAMCURVEFREE_H