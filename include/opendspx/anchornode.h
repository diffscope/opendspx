#ifndef OPENDSPX_MODEL_ANCHORNODE_H
#define OPENDSPX_MODEL_ANCHORNODE_H

namespace QDspx {

    struct AnchorNode {
        enum Interpolation {
            None,
            Linear,
            Hermite,
        };
        Interpolation interp{None};
        int x{0};
        int y{0};
    };

}

#endif //OPENDSPX_MODEL_ANCHORNODE_H