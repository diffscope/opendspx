#ifndef OPENDSPX_SVIP_H
#define OPENDSPX_SVIP_H

#include <opendspx/converter.h>

namespace QDspx {

    class OPENDSPX_EXPORT SvipConverter : public Converter {
    public:
        Result load(const QString &path, Model *out, const QVariantMap &args) override;
        Result save(const QString &path, const Model &in, const QVariantMap &args) override;
    };

}

#endif // OPENDSPX_SVIP_H
