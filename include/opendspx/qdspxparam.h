#ifndef QDSPXPARAM_H
#define QDSPXPARAM_H

#include <opendspx/qdspxbase.h>

namespace QDspx {

    // 参数曲线基类
    struct ParamCurve {
        enum Type {
            __qas_attr__("anchor") //
            Anchor,
            __qas_attr__("free")   //
            Free,
        };
        QAS_JSON(Type)

        Type type;

        // 构造器
        ParamCurve() : ParamCurve(Anchor){};
        inline explicit ParamCurve(Type type) : type(type){};
        virtual ~ParamCurve() = default;
    };

    using ParamCurveRef = QSharedPointer<ParamCurve>;

    // 手绘参数
    struct ParamFree : public ParamCurve {
        int start;
        int step;
        QList<int> values;

        // 构造器
        inline ParamFree() : ParamFree(0){};
        inline explicit ParamFree(int start) : ParamCurve(Free), start(start), step(5){};
    };

    using ParamFreeRef = QSharedPointer<ParamFree>;

    // 锚点参数
    struct ParamAnchor : public ParamCurve {
        int start;
        QList<AnchorPoint> nodes;

        // 构造器
        inline ParamAnchor() : ParamCurve(Anchor){};
    };

    using ParamAnchorRef = QSharedPointer<ParamAnchor>;

    // 参数结构
    struct ParamInfo {
        __qas_attr__("original")
        QList<ParamCurveRef> org;

        QList<ParamCurveRef> edited;

        QList<ParamCurveRef> envelope;
    };

    // 单线条参数
    struct SingleParam {
        ParamInfo pitch;
        ParamInfo energy;
        ParamInfo tension;
        ParamInfo breathiness;
    };

    QAS_JSON_NS(ParamCurve)
    QAS_JSON_NS(ParamFree)
    QAS_JSON_NS(ParamAnchor)
    QAS_JSON_NS(ParamInfo)
    QAS_JSON_NS(SingleParam)
    QAS_JSON_NS_IMPL(ParamCurveRef)

}

#endif // QDSPXPARAM_H
