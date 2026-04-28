#ifndef OPENDSPX_MODEL_SOURCEMIXINGRATIO_H
#define OPENDSPX_MODEL_SOURCEMIXINGRATIO_H

#include <algorithm>
#include <vector>

namespace opendspx {

    class SourceMixingRatio : public std::vector<double> {
    public:
        using std::vector<double>::vector;

        [[nodiscard]] constexpr bool valid() const {
            return std::ranges::all_of(*this, [](double x) { return x >= 0 && x <= 1; }) && std::reduce(begin(), end(), 0.0) <= 1.0;
        }
    };

}

#endif //OPENDSPX_MODEL_SOURCEMIXINGRATIO_H
