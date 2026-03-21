#ifndef OPENDSPX_MODEL_PARAMCURVE_H
#define OPENDSPX_MODEL_PARAMCURVE_H

#include <QSharedPointer>

namespace QDspx {

    struct ParamCurve {
        enum Type {
            Anchor,
            Free,
        };
        Type type;
        int start;

    protected:
        ParamCurve(Type type, int start = 0)
            : type(type), start(start) {
        }
    };

    using ParamCurveRef = QSharedPointer<ParamCurve>;

}

#endif //OPENDSPX_MODEL_PARAMCURVE_H