#ifndef OPENDSPX_MODEL_PHONEME_H
#define OPENDSPX_MODEL_PHONEME_H

#include <QString>

namespace opendspx {

    struct Phoneme {
        QString language;
        QString token;
        int start{0};
        bool onset{false};
    };

}

#endif //OPENDSPX_MODEL_PHONEME_H