#ifndef OPENDSPX_MODEL_SINGINGCLIP_H
#define OPENDSPX_MODEL_SINGINGCLIP_H

#include <QList>

#include <opendspx/clip.h>
#include <opendspx/note.h>
#include <opendspx/params.h>
#include <opendspx/sources.h>

namespace QDspx {

    struct SingingClip : Clip {
        SingingClip(const QString &name = {}, const BusControl &control = {}, const ClipTime &time = {}, const Workspace &workspace = {})
            : Clip(Singing, name, control, time, workspace) {
        }

        QList<Note> notes;
        Params params;
        Sources sources;
    };
    
    using SingingClipRef = QSharedPointer<SingingClip>;

}

#endif //OPENDSPX_MODEL_SINGINGCLIP_H