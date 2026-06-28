#ifndef OPENDSPX_MODEL_PHONEMES_H
#define OPENDSPX_MODEL_PHONEMES_H

#include <vector>

#include <opendspx/phoneme.h>

namespace opendspx {

    struct Phonemes {
        std::vector<Phoneme> original;
        std::vector<Phoneme> edited;
    };

}

#endif //OPENDSPX_MODEL_PHONEMES_H