#ifndef OPENDSPX_GLOBAL_H
#define OPENDSPX_GLOBAL_H

#include <string>

namespace opendspx {

    struct Global {
        std::string author;
        std::string name;
        int centShift{0};
        std::string editorId;
        std::string editorName;
    };

}

#endif //OPENDSPX_GLOBAL_H
