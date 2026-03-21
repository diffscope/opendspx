#ifndef OPENDSPX_MODEL_NOTE_H
#define OPENDSPX_MODEL_NOTE_H

#include <QString>

#include <opendspx/phonemes.h>
#include <opendspx/pronunciation.h>
#include <opendspx/vibrato.h>
#include <opendspx/workspace.h>

namespace QDspx {

    struct Note {
        int pos{0};
        int length{0};
        int keyNum{0};
        int centShift{0};
        QString language;
        QString lyric;
        Pronunciation pronunciation;
        Phonemes phonemes;
        Vibrato vibrato;
        Workspace workspace;
    };

}

#endif //OPENDSPX_MODEL_NOTE_H