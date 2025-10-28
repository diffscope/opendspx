#ifndef OPENDSPX_MODEL_CLIPTIME_H
#define OPENDSPX_MODEL_CLIPTIME_H

namespace QDspx {

    struct ClipTime {
        int start{0};
        int length{0};
        int clipStart{0};
        int clipLen{0};
    };

}

#endif //OPENDSPX_MODEL_CLIPTIME_H
