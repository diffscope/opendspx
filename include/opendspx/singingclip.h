#ifndef OPENDSPX_MODEL_SINGINGCLIP_H
#define OPENDSPX_MODEL_SINGINGCLIP_H

#include <QList>

#include <opendspx/clip.h>
#include <opendspx/note.h>
#include <opendspx/params.h>
#include <opendspx/sources.h>

namespace opendspx {

    struct SingingClip : Clip {
        SingingClip(const QString &name = {}, const BusControl &control = {}, const ClipTime &time = {}, const Workspace &workspace = {}, const QList<Note> &notes = {}, const Params &params = {}, const Sources &sources = {})
            : Clip(Singing, name, control, time, workspace), notes(notes), params(params), sources(sources) {
        }

        QList<Note> notes;
        Params params;
        Sources sources;
    };
    
    using SingingClipRef = QSharedPointer<SingingClip>;

}

#endif //OPENDSPX_MODEL_SINGINGCLIP_H