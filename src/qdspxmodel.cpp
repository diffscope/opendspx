#include "qdspxmodel.h"

#include <QFile>
#include <QJsonDocument>

namespace QDspx {

    Result Model::load(const QString &filename) {
        // Read file
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            return {Result::File, file.error()};
        }
        QByteArray data = file.readAll();
        file.close();
        return loadData(data);
    }

    Result Model::save(const QString &filename) const {
        // Create file
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly)) {
            return {Result::File, file.error()};
        }

        // Serialize and write
        file.write(QJsonDocument(qAsClassToJson(*this)).toJson(QJsonDocument::Compact));
        file.close();

        return Result::Success;
    }

    Result Model::loadData(const QByteArray &data) {
        // Parse json
        QJsonParseError parseError{};
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
        if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
            return Result::InvalidFormat;
        }

        // Deserialize
        if (qAsJsonTryGetClass(doc.object(), this))
            return Result::Success;
        return Result::InvalidFormat;
    }

    QByteArray Model::saveData() const {
        return QJsonDocument(qAsClassToJson(*this)).toJson(QJsonDocument::Compact);
    }

}
