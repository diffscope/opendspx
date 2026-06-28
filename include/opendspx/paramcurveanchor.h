#ifndef OPENDSPX_MODEL_PARAMCURVEANCHOR_H
#define OPENDSPX_MODEL_PARAMCURVEANCHOR_H

#include <vector>
#include <utility>

#include <opendspx/paramcurve.h>
#include <opendspx/anchornode.h>

namespace opendspx {

    struct ParamCurveAnchor : ParamCurve {
        ParamCurveAnchor(int start = 0, std::vector<AnchorNode> nodes = {})
            : ParamCurve(Anchor, start), nodes(std::move(nodes)) {
        }

        std::vector<AnchorNode> nodes;
    };
    
    using ParamCurveAnchorRef = std::shared_ptr<ParamCurveAnchor>;

}

#endif //OPENDSPX_MODEL_PARAMCURVEANCHOR_H