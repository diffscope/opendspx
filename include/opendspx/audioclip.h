#ifndef OPENDSPX_MODEL_AUDIOCLIP_H
#define OPENDSPX_MODEL_AUDIOCLIP_H

#include <opendspx/clip.h>

namespace opendspx {

    struct AudioClip : Clip {
        AudioClip(const QString &name = {}, const BusControl &control = {}, const ClipTime &time = {}, const Workspace &workspace = {}, const QString &path = {})
            : Clip(Audio, name, control, time, workspace), path(path) {
        }

        QString path;
    };
    using AudioClipRef = QSharedPointer<AudioClip>;

}

#endif //OPENDSPX_MODEL_AUDIOCLIP_H