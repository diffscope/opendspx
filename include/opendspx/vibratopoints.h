#ifndef OPENDSPX_MODEL_VIBRATOPOINTS_H
#define OPENDSPX_MODEL_VIBRATOPOINTS_H

#include <QList>

#include <opendspx/controlpoint.h>

namespace QDspx {

    struct VibratoPoints {
        QList<ControlPoint> amp;
        QList<ControlPoint> freq;
    };

}

#endif //OPENDSPX_MODEL_VIBRATOPOINTS_H