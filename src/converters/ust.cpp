#include "converters/ust.h"

#include <fstream>
#include <filesystem>

#include <QFile>

#include <stdutau/ustfile.h>

namespace QDspx {

    ReturnCode UstConverter::load(const QString &path, Model *out, const QVariantMap &args) {
        std::ifstream fs(std::filesystem::path(path.toStdWString()));
        if (!fs.is_open())
            return {ReturnCode::File, QFileDevice::ReadError};

        Utau::UstFile file;
        if (!file.read(fs)) {
            return ReturnCode::InvalidFormat;
        }

        // Convert

        return ReturnCode::Success;
    }

    ReturnCode UstConverter::save(const QString &path, const Model &in, const QVariantMap &args) {
        std::ofstream fs(std::filesystem::path(path.toStdWString()));
        if (!fs.is_open())
            return {ReturnCode::File, QFileDevice::WriteError};

        Utau::UstFile file;

        // Convert

        if (!file.write(fs)) {
            return {ReturnCode::File, QFileDevice::WriteError};
        }
        return ReturnCode::Success;
    }

}