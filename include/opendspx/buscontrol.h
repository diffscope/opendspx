#ifndef OPENDSPX_MODEL_BUSCONTROL_H
#define OPENDSPX_MODEL_BUSCONTROL_H

namespace QDspx {

    struct BusControl {
        double gain{0.0};
        double pan{0.0};
        bool mute{false};
    };

}

#endif //OPENDSPX_MODEL_BUSCONTROL_H
