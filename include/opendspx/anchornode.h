#ifndef OPENDSPX_ANCHORNODE_H
#define OPENDSPX_ANCHORNODE_H

namespace opendspx {

    struct AnchorNode {
        enum class Interpolation {
            None,
            Linear,
            Hermite,
        };
        Interpolation interp{Interpolation::None};
        int x{0};
        int y{0};
    };

}

#endif //OPENDSPX_ANCHORNODE_H