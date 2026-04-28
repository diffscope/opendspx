#ifndef OPENDSPX_MODEL_SINGER_H
#define OPENDSPX_MODEL_SINGER_H

#include <memory>
#include <utility>

#include <nlohmann/json.hpp>

#include <opendspx/workspace.h>

namespace opendspx {

    struct Singer {
        enum class Type {
            Single,
            Mixed,
        };
        Type type;
        nlohmann::json extra;
        Workspace workspace;

    protected:
        Singer(Type type, nlohmann::json extra = {}, Workspace workspace = {})
            : type(type), extra(std::move(extra)), workspace(std::move(workspace)) {
        }
    };

    using SingerRef = std::shared_ptr<Singer>;

}

#endif //OPENDSPX_MODEL_SINGER_H
