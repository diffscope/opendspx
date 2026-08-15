#ifndef OPENDSPX_DYNAMICMIXINGANCHOR_H
#define OPENDSPX_DYNAMICMIXINGANCHOR_H

#include <opendspx/sourcemixingratio.h>

namespace opendspx {

    struct DynamicMixingAnchor {
        int pos{0};
        SourceMixingRatio ratio;
    };

}

#endif //OPENDSPX_DYNAMICMIXINGANCHOR_H
