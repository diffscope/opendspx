#ifndef OPENDSPX_MODEL_GLOBAL_H
#define OPENDSPX_MODEL_GLOBAL_H

#include <QString>

namespace QDspx {

    struct Global {
        QString author;
        QString name;
        int centShift{0};
        QString editorId;
        QString editorName;
    };

}

#endif //OPENDSPX_MODEL_GLOBAL_H
