#ifndef OPENDSPX_MODEL_TRACK_H
#define OPENDSPX_MODEL_TRACK_H

#include <QList>
#include <QString>

#include <opendspx/audioclip.h>
#include <opendspx/clip.h>
#include <opendspx/singingclip.h>
#include <opendspx/trackcontrol.h>
#include <opendspx/workspace.h>

namespace QDspx {

    struct Track {
        QString name;
        TrackControl control;
        QList<ClipRef> clips;
        Workspace workspace;
    };

}

#endif //OPENDSPX_MODEL_TRACK_H