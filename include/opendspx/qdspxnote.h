#ifndef QDSPXNOTE_H
#define QDSPXNOTE_H

#include <opendspx/qdspxbase.h>

namespace QDspx {

    // 音素
    struct Phoneme {
        enum Type {
            __qas_attr__("ahead")  //
            Ahead,
            __qas_attr__("normal") //
            Normal,
            __qas_attr__("final")  //
            Final,
        };
        QAS_JSON(Type)

        Type type;
        QString token;
        int start;

        // 不定长信息
        Extra extra;
        Workspace workspace;

        // 构造器
        inline Phoneme() : type(Normal), start(0){};
    };

    // 音素信息
    struct PhonemeInfo {
        __qas_attr__("original")
        QList<Phoneme> org;

        QList<Phoneme> edited;
    };

    // 颤音信息
    struct VibratoInfo {
        double start;
        double end;
        double freq;
        double phase;
        double amp;
        double offset;
        QList<DoublePoint> points;

        // 构造器
        inline VibratoInfo() : start(0), end(1), freq(2), phase(0), amp(1), offset(0){};
    };

    // 发音
    struct Pronunciation {
        __qas_attr__("original")
        QString org;
        QString edited;
    };

    // 音符
    struct Note {
        int pos;
        int length;
        int keyNum;
        QString language;
        QString lyric;
        Pronunciation pronunciation;
        PhonemeInfo phonemes;
        VibratoInfo vibrato;

        // 不定长信息
        Extra extra;
        Workspace workspace;

        // 构造器
        inline Note() : Note(0, 480, 60){};
        inline Note(int pos, int length, int keyNum) : pos(pos), length(length), keyNum(keyNum){};
    };

    QAS_JSON_NS(Phoneme)
    QAS_JSON_NS(PhonemeInfo)
    QAS_JSON_NS(VibratoInfo)
    QAS_JSON_NS(Pronunciation)
    QAS_JSON_NS(Note)

}

#endif // QDSPXNOTE_H
