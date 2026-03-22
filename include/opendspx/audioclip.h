#ifndef OPENDSPX_MODEL_AUDIOCLIP_H
#define OPENDSPX_MODEL_AUDIOCLIP_H

#include <opendspx/clip.h>

#include <utility>

namespace opendspx {

    struct AudioClip : Clip {
        AudioClip(std::string name = {}, const BusControl &control = {}, const ClipTime &time = {}, Workspace workspace = {}, std::string path = {})
            : Clip(Type::Audio, std::move(name), control, time, std::move(workspace)), path(std::move(path)) {
        }

        std::string path;
    };
    using AudioClipRef = std::shared_ptr<AudioClip>;

}

#endif //OPENDSPX_MODEL_AUDIOCLIP_H