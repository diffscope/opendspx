#ifndef OPENDSPX_MODEL_SOURCES_H
#define OPENDSPX_MODEL_SOURCES_H

#include <string>
#include <vector>

#include <opendspx/dynamicmixinganchor.h>
#include <opendspx/singer.h>
#include <opendspx/singlesinger.h>
#include <opendspx/mixedsinger.h>

namespace opendspx {

    struct Sources {
        std::string category;
        std::vector<DynamicMixingAnchor> mix;
        std::vector<SingerRef> singers;
    };

}

#endif //OPENDSPX_MODEL_SOURCES_H