#ifndef QDSPXBASE_H
#define QDSPXBASE_H

#include <qjsonstream.h>

#include <opendspx/opendspx_global.h>

namespace QDspx {

    using Extra = QMap<QString, QJsonObject>;      // 其他参数，不定长
    using Workspace = QMap<QString, QJsonObject>;  // 状态信息，不定长
    using SourceInfo = QMap<QString, QJsonObject>; // 外部资源信息，不定长

    // 主控
    struct OPENDSPX_EXPORT Control {
        double gain;
        bool mute;

        // 构造器
        inline Control() : Control(0, false){};
        inline Control(double gain, bool mute) : gain(gain), mute(mute){};
    };

    // 音轨主控
    struct OPENDSPX_EXPORT TrackControl : public Control {
        double pan;
        bool solo;

        // 构造器
        inline TrackControl() : TrackControl(0, false){};
        inline TrackControl(double pan, bool solo) : pan(pan), solo(solo){};
        inline TrackControl(double gain, double pan, bool mute, bool solo)
            : Control(gain, mute), pan(pan), solo(solo){};
    };

    // 泛型点
    template <class T>
    struct OPENDSPX_EXPORT Point {
        T x;
        T y;

        Point() : Point(0, 0){};
        Point(T x, T y) : x(x), y(y){};
    };

    using IntPoint = Point<int>;
    using DoublePoint = Point<double>;

    // 控制点
    struct OPENDSPX_EXPORT AnchorPoint : public IntPoint {
        enum Interpolation {
            __qas_attr__("none")    //
            None,
            __qas_attr__("linear")  //
            Linear,
            __qas_attr__("hermite") //
            Hermite,
        };
        QAS_JSON(Interpolation)

        Interpolation interp;

        // 构造器
        inline AnchorPoint() : interp(Linear){};
        inline AnchorPoint(int x, int y, Interpolation i = Linear) : IntPoint(x, y), interp(i){};
    };

    QAS_JSON_NS(Control)
    QAS_JSON_NS(TrackControl)
    QAS_JSON_NS(IntPoint)
    QAS_JSON_NS(DoublePoint)
    QAS_JSON_NS(AnchorPoint)

}

#endif // QDSPXBASE_H
