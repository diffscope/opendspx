#ifndef OPENDSPX_MODEL_MODEL_H
#define OPENDSPX_MODEL_MODEL_H

#include <opendspx/content.h>

namespace QDspx {

    struct Model {
        enum Version {
            V1_0_0,
        };
        Version version{V1_0_0};
        Content content;
    };

}

#endif //OPENDSPX_MODEL_MODEL_H
