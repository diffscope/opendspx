#ifndef OPENDSPX_MODEL_CLIP_H
#define OPENDSPX_MODEL_CLIP_H

#include <QSharedPointer>
#include <QString>

#include <opendspx/buscontrol.h>
#include <opendspx/cliptime.h>
#include <opendspx/workspace.h>

namespace QDspx {

    struct Clip {
        enum Type {
            Audio,
            Singing,
        };
        Type type;
        QString name;
        BusControl control;
        ClipTime time;
        Workspace workspace;
    };

    using ClipRef = QSharedPointer<Clip>;

}

#endif //OPENDSPX_MODEL_CLIP_H