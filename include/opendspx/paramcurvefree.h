#ifndef OPENDSPX_MODEL_PARAMCURVEFREE_H
#define OPENDSPX_MODEL_PARAMCURVEFREE_H

#include <QList>

#include <opendspx/paramcurve.h>

namespace QDspx {

    struct ParamCurveFree : ParamCurve {
        ParamCurveFree(int start = 0, int step = 5, const QList<int> &values = {})
            : ParamCurve(Free, start), step(step), values(values) {
        }

        int step;
        QList<int> values;
    };
    
    using ParamCurveFreeRef = QSharedPointer<ParamCurveFree>;

}

#endif //OPENDSPX_MODEL_PARAMCURVEFREE_H