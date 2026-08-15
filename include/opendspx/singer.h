#ifndef OPENDSPX_SINGER_H
#define OPENDSPX_SINGER_H

#include <memory>
#include <utility>

#include <stdcorelib/support/json.h>

#include <opendspx/workspace.h>

namespace opendspx {

    struct Singer {
        enum class Type {
            Single,
            Mixed,
        };
        Type type;
        stdc::JsonValue extra;
        Workspace workspace;

    protected:
        Singer(Type type, stdc::JsonValue extra = {}, Workspace workspace = {})
            : type(type), extra(std::move(extra)), workspace(std::move(workspace)) {
        }
    };

    using SingerRef = std::shared_ptr<Singer>;

}

#endif //OPENDSPX_SINGER_H
