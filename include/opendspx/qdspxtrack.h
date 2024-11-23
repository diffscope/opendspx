#ifndef QDSPXTRACK_H
#define QDSPXTRACK_H

#include <opendspx/qdspxparam.h>
#include <opendspx/qdspxnote.h>

namespace QDspx
{
    // 音轨区间时间信息
    struct ClipTime {
        int start;
        int length;
        int clipStart;
        int clipLen;

        // 构造器
        inline ClipTime() : ClipTime(0, 4800) {};
        inline ClipTime(int start, int length) : ClipTime(start, length, 0, length) {};

        inline ClipTime(int start, int length, int clipStart, int clipLen)
            : start(start), length(length), clipStart(clipStart), clipLen(clipLen) {};
    };

    // 音轨区间
    struct Clip {
        enum Type {
            __qas_attr__("singing") //
            Singing,

            __qas_attr__("audio") //
            Audio,
        };

        QAS_JSON(Type)

        Type type;
        ClipTime time;
        QString name;
        Control control;

        // 不定长信息
        Extra extra;
        Workspace workspace;

        // 构造器
        inline Clip() : type(Singing) {};
        inline explicit Clip(Type type) : type(type) {};
        virtual ~Clip() = default;
    };

    using ClipRef = QSharedPointer<Clip>;

    // 波形区间
    struct AudioClip : public Clip {
        QString path;

        // 构造器
        AudioClip() : Clip(Audio) {};
    };

    using AudioClipRef = QSharedPointer<AudioClip>;

    // 人声区间
    struct SingingClip : public Clip {
        QList<Note> notes;
        SingleParam params;

        QString language;
        QString g2pId;

        // 不定长信息
        SourceInfo sources;

        // 构造器
        SingingClip() : Clip(Singing) {};
    };

    using SingingClipRef = QSharedPointer<SingingClip>;

    // 音轨
    struct Track {
        QString name;
        TrackControl control;
        QList<ClipRef> clips;

        QString language;
        QString g2pId;

        // 不定长信息
        Extra extra;
        Workspace workspace;
    };

    QAS_JSON_NS(ClipTime)
    QAS_JSON_NS(Clip)
    QAS_JSON_NS(AudioClip)
    QAS_JSON_NS(SingingClip)
    QAS_JSON_NS(Track)
    QAS_JSON_NS_IMPL(ClipRef)
}

#endif // QDSPXTRACK_H
