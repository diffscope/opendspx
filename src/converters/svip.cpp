#include "converters/svip.h"

#include <QFile>

#include <QNrbfStream.h>

namespace QDspx {

    ReturnCode SvipConverter::load(const QString &path, Model *out, const QVariantMap &args) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            return {ReturnCode::File, file.error()};
        }

        QByteArray bytes = file.readAll();
        file.close();

        QNrbf::XSAppModel xsProject;

        // Read model
        QNrbfStream in(bytes);
        QString svipName;
        QString version;
        in >> svipName;
        in >> version;
        if (in.status() != QDataStream::Ok) {
            return ReturnCode::InvalidFormat;
        }
        in >> xsProject;
        if (in.status() != QDataStream::Ok) {
            return ReturnCode::InvalidFormat;
        }

        // Convert

        return ReturnCode::Success;
    }

    ReturnCode SvipConverter::save(const QString &path, const Model &in, const QVariantMap &args) {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly))
            return {ReturnCode::File, file.error()};

        QNrbf::XSAppModel xsProject;

        // Convert

        // Write model
        QNrbfStream output(&file);
        output << "SVIP"
               << "6.0.0" << xsProject;
        file.close();
        return ReturnCode::Success;
    }

}