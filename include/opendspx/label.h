#ifndef OPENDSPX_MODEL_LABEL_H
#define OPENDSPX_MODEL_LABEL_H

#include <string>

namespace opendspx {

    struct Label {
        int pos{0};
        std::string text;
    };

}

#endif //OPENDSPX_MODEL_LABEL_H