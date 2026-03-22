#ifndef OPENDSPX_MODEL_CONTENT_H
#define OPENDSPX_MODEL_CONTENT_H

#include <QList>

#include <opendspx/global.h>
#include <opendspx/master.h>
#include <opendspx/timeline.h>
#include <opendspx/track.h>
#include <opendspx/workspace.h>

namespace opendspx{

    struct Content {
        Global global;
        Master master;
        Timeline timeline;
        QList<Track> tracks;
        Workspace workspace;
    };

}

#endif //OPENDSPX_MODEL_CONTENT_H
