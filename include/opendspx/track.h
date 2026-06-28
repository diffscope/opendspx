#ifndef OPENDSPX_MODEL_TRACK_H
#define OPENDSPX_MODEL_TRACK_H

#include <vector>
#include <string>

#include <opendspx/audioclip.h>
#include <opendspx/clip.h>
#include <opendspx/singingclip.h>
#include <opendspx/trackcontrol.h>
#include <opendspx/workspace.h>

namespace opendspx {

    struct Track {
        std::string name;
        TrackControl control;
        std::vector<ClipRef> clips;
        Workspace workspace;
    };

}

#endif //OPENDSPX_MODEL_TRACK_H