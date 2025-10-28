#ifndef OPENDSPX_MODEL_PARAMCURVEANCHOR_H
#define OPENDSPX_MODEL_PARAMCURVEANCHOR_H

#include <QList>

#include <opendspx/paramcurve.h>
#include <opendspx/anchornode.h>

namespace QDspx {

    struct ParamCurveAnchor : ParamCurve {
        ParamCurveAnchor(int start = 0)
            : ParamCurve(Anchor, start) {
        }

        QList<AnchorNode> nodes;
    };
    
    using ParamCurveAnchorRef = QSharedPointer<ParamCurveAnchor>;

}

#endif //OPENDSPX_MODEL_PARAMCURVEANCHOR_H