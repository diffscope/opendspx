#ifndef OPENDSPX_PHONEMES_H
#define OPENDSPX_PHONEMES_H

#include <vector>

#include <opendspx/phoneme.h>

namespace opendspx {

    struct Phonemes {
        std::vector<Phoneme> original;
        std::vector<Phoneme> edited;
    };

}

#endif //OPENDSPX_PHONEMES_H