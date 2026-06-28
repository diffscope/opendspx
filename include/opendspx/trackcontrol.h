#ifndef OPENDSPX_MODEL_TRACKCONTROL_H
#define OPENDSPX_MODEL_TRACKCONTROL_H

namespace opendspx {

    struct TrackControl {
        double gain{0.0};
        double pan{0.0};
        bool mute{false};
        bool solo{false};
    };

}

#endif //OPENDSPX_MODEL_TRACKCONTROL_H
