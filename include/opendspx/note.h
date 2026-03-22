#ifndef OPENDSPX_MODEL_NOTE_H
#define OPENDSPX_MODEL_NOTE_H

#include <string>

#include <opendspx/phonemes.h>
#include <opendspx/pronunciation.h>
#include <opendspx/vibrato.h>
#include <opendspx/workspace.h>

namespace opendspx {

    struct Note {
        int pos{0};
        int length{0};
        int keyNum{0};
        int centShift{0};
        std::string language;
        std::string lyric;
        Pronunciation pronunciation;
        Phonemes phonemes;
        Vibrato vibrato;
        Workspace workspace;
    };

}

#endif //OPENDSPX_MODEL_NOTE_H