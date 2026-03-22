#ifndef OPENDSPX_MODEL_PARAM_H
#define OPENDSPX_MODEL_PARAM_H

#include <QList>

#include <opendspx/paramcurve.h>
#include <opendspx/paramcurveanchor.h>
#include <opendspx/paramcurvefree.h>

namespace opendspx {

    struct Param {
        QList<ParamCurveRef> original;
        QList<ParamCurveRef> transform;
        QList<ParamCurveRef> edited;
    };

}

#endif //OPENDSPX_MODEL_PARAM_H