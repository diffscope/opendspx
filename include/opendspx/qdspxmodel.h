#ifndef QDSPXMODEL_H
#define QDSPXMODEL_H

#include <opendspx/qdspxtimeline.h>
#include <opendspx/qdspxtrack.h>

namespace QDspx {

    struct Result {
        enum Type {
            Success,
            File,
            InvalidFormat,
            Empty,
            Aborted,
            User = 1024,
        };

        int type;
        int code;

        inline Result(int type = Success, int code = 0) : type(type), code(code) {};
    };

    // 文件的元信息，包括版本号、工程名、作者等
    struct Metadata {
        QString name;
        QString author;
    };

    // 总线控制
    struct Master {
        Control control;
    };

    // 工程可编辑区域
    struct Content {
        Metadata metadata;
        Master master;
        Timeline timeline;
        QList<Track> tracks;
        Workspace workspace;
    };

    // 工程
    struct OPENDSPX_EXPORT Model {
        QString version;
        Content content;

        Result load(const QString &filename);
        Result save(const QString &filename) const;

        Result loadData(const QByteArray &data);
        QByteArray saveData() const;
    };

    QAS_JSON_NS(Metadata)

    QAS_JSON_NS(Master)

    QAS_JSON_NS(Content)

    QAS_JSON_NS(Model)

}

using QDspxModel = QDspx::Model;

#endif // QDSPXMODEL_H
