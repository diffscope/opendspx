#ifndef OPENDSPX_SINGLESINGER_H
#define OPENDSPX_SINGLESINGER_H

#include <string>
#include <utility>

#include <opendspx/singer.h>

namespace opendspx {

    struct SingleSinger : Singer {
        SingleSinger(std::string id = {}, stdc::JsonValue extra = {}, Workspace workspace = {})
            : Singer(Type::Single, std::move(extra), std::move(workspace)), id(std::move(id)) {
        }

        std::string id;
    };

    using SingleSingerRef = std::shared_ptr<SingleSinger>;

}

#endif //OPENDSPX_SINGLESINGER_H
