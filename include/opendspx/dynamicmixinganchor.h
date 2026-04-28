#ifndef OPENDSPX_MODEL_DYNAMICMIXINGANCHOR_H
#define OPENDSPX_MODEL_DYNAMICMIXINGANCHOR_H

#include <opendspx/sourcemixingratio.h>

namespace opendspx {

    struct DynamicMixingAnchor {
        int pos{0};
        SourceMixingRatio ratio;
    };

}

#endif //OPENDSPX_MODEL_DYNAMICMIXINGANCHOR_H
