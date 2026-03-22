#ifndef OPENDSPX_MODEL_MODEL_H
#define OPENDSPX_MODEL_MODEL_H

#include <opendspx/content.h>

namespace opendspx {

    struct Model {
        enum Version {
            V1,
        };
        Version version{V1};
        Content content;
    };

}

#endif //OPENDSPX_MODEL_MODEL_H
