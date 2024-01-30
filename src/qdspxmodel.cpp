#include "qdspxmodel.h"

#include <QFile>
#include <QJsonDocument>

namespace QDspx {

    ReturnCode Model::load(const QString &filename) {
        // Read file
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            return {ReturnCode::File, file.error()};
        }
        QByteArray data = file.readAll();
        file.close();

        // Parse json
        QJsonParseError parseError{};
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
        if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
            return ReturnCode::InvalidFormat;
        }

        // Deserialize
        if (qAsJsonTryGetClass(doc.object(), this))
            return ReturnCode::Success;
        
        return ReturnCode::InvalidFormat;
    }

    ReturnCode Model::save(const QString &filename) const {
        // Create file
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly)) {
            return {ReturnCode::File, file.error()};
        }

        // Serialize and write
        file.write(QJsonDocument(qAsClassToJson(*this)).toJson());
        file.close();

        return ReturnCode::Success;
    }

}
