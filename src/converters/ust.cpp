#include "converters/ust.h"

#include <fstream>
#include <filesystem>

#include <QFile>
#include <QTextCodec>

#include <stdutau/ustfile.h>

namespace QDspx {

    Result UstConverter::load(const QString &path, Model *out, const QVariantMap &args) {
        std::ifstream fs(std::filesystem::path(path.toStdWString()));
        if (!fs.is_open())
            return {Result::File, QFileDevice::ReadError};

        Utau::UstFile file;
        if (!file.read(fs)) {
            return Result::InvalidFormat;
        }

        // Convert

        return Result::Success;
    }

    Result UstConverter::save(const QString &path, const Model &in, const QVariantMap &args) {
        std::ofstream fs(std::filesystem::path(path.toStdWString()));
        if (!fs.is_open())
            return {Result::File, QFileDevice::WriteError};

        Utau::UstFile file;

        // Convert

        if (!file.write(fs)) {
            return {Result::File, QFileDevice::WriteError};
        }
        return Result::Success;
    }

}