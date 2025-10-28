#ifndef OPENDSPX_MODEL_TIMELINE_H
#define OPENDSPX_MODEL_TIMELINE_H

#include <QList>

#include <opendspx/label.h>
#include <opendspx/tempo.h>
#include <opendspx/timesignature.h>

namespace QDspx {

    struct Timeline {
        QList<Label> labels;
        QList<Tempo> tempos;
        QList<TimeSignature> timeSignatures;
    };

}

#endif //OPENDSPX_MODEL_TIMELINE_H