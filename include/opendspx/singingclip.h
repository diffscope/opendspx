#ifndef OPENDSPX_MODEL_SINGINGCLIP_H
#define OPENDSPX_MODEL_SINGINGCLIP_H

#include <utility>
#include <vector>

#include <opendspx/clip.h>
#include <opendspx/note.h>
#include <opendspx/params.h>
#include <opendspx/sources.h>

namespace opendspx {

    struct SingingClip : Clip {
        SingingClip(const std::string &name = {}, const BusControl &control = {}, const ClipTime &time = {}, const Workspace &workspace = {}, std::vector<Note> notes = {}, const Params &params = {}, const Sources &sources = {})
            : Clip(Type::Singing, name, control, time, workspace), notes(std::move(notes)), params(params), sources(sources) {
        }

        std::vector<Note> notes;
        Params params;
        Sources sources;
    };
    
    using SingingClipRef = std::shared_ptr<SingingClip>;

}

#endif //OPENDSPX_MODEL_SINGINGCLIP_H