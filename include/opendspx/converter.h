#ifndef CONVERTER_H
#define CONVERTER_H

#include <opendspx/qdspxmodel.h>

namespace QDspx {

    class OPENDSPX_EXPORT Converter {
    public:
        virtual ~Converter() = default;

        virtual ReturnCode load(const QString &path, Model *out, const QVariantMap &args) = 0;
        virtual ReturnCode save(const QString &path, const Model &in, const QVariantMap &args) = 0;
    };

}

#endif // CONVERTER_H
