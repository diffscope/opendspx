#ifndef OPENDSPX_MODEL_CLIPTIME_H
#define OPENDSPX_MODEL_CLIPTIME_H

namespace opendspx {

    struct ClipTime {
        int pos{0};
        int length{0};
        int clipStart{0};
        int clipLen{0};
    };

}

#endif //OPENDSPX_MODEL_CLIPTIME_H
