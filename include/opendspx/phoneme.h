#ifndef OPENDSPX_MODEL_PHONEME_H
#define OPENDSPX_MODEL_PHONEME_H

#include <QString>

namespace QDspx {

    struct Phoneme {
        QString language;
        int start{0};
        QString token;
        bool onset{false};
    };

}

#endif //OPENDSPX_MODEL_PHONEME_H