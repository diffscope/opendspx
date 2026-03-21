#ifndef OPENDSPX_MODEL_PHONEMES_H
#define OPENDSPX_MODEL_PHONEMES_H

#include <QList>

#include <opendspx/phoneme.h>

namespace QDspx {

    struct Phonemes {
        QList<Phoneme> original;
        QList<Phoneme> edited;
    };

}

#endif //OPENDSPX_MODEL_PHONEMES_H