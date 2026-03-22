#ifndef OPENDSPX_MODEL_CLIP_H
#define OPENDSPX_MODEL_CLIP_H

#include <memory>
#include <string>
#include <utility>

#include <opendspx/buscontrol.h>
#include <opendspx/cliptime.h>
#include <opendspx/workspace.h>

namespace opendspx {

    struct Clip {
        enum class Type {
            Audio,
            Singing,
        };
        Type type;
        std::string name;
        BusControl control;
        ClipTime time;
        Workspace workspace;

    protected:
        Clip(Type type, std::string name = {}, const BusControl &control = {}, const ClipTime &time = {}, Workspace workspace = {})
            : type(type), name(std::move(name)), control(control), time(time), workspace(std::move(workspace)) {
        }
    };

    using ClipRef = std::shared_ptr<Clip>;

}

#endif //OPENDSPX_MODEL_CLIP_H