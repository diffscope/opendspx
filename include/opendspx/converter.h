#ifndef CONVERTER_H
#define CONVERTER_H

#include <opendspx/qdspxmodel.h>

namespace QDspx {

    class OPENDSPX_EXPORT Converter {
    public:
        virtual ~Converter() = default;

        virtual Result load(const QString &path, Model *out, const QVariantMap &args) = 0;
        virtual Result save(const QString &path, const Model &in, const QVariantMap &args) = 0;
    };

}

#endif // CONVERTER_H
