#ifndef OPENDSPX_MODEL_MIXEDSINGER_H
#define OPENDSPX_MODEL_MIXEDSINGER_H

#include <utility>
#include <vector>

#include <opendspx/singer.h>
#include <opendspx/sourcemixingratio.h>

namespace opendspx {

    struct MixedSinger : Singer {
        MixedSinger(std::vector<SingerRef> singers = {}, SourceMixingRatio ratio = {}, nlohmann::json extra = {}, Workspace workspace = {})
            : Singer(Type::Mixed, std::move(extra), std::move(workspace)), singers(std::move(singers)), ratio(std::move(ratio)) {
        }

        std::vector<SingerRef> singers;
        SourceMixingRatio ratio;
    };

    using MixedSingerRef = std::shared_ptr<MixedSinger>;

}

#endif //OPENDSPX_MODEL_MIXEDSINGER_H
