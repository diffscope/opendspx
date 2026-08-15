#ifndef OPENDSPX_PHONEME_H
#define OPENDSPX_PHONEME_H

#include <string>

namespace opendspx {

    struct Phoneme {
        std::string language;
        std::string token;
        int start{0};
        bool onset{false};
    };

}

#endif //OPENDSPX_PHONEME_H