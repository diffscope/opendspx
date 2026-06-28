#ifndef OPENDSPX_MODEL_PARAMS_H
#define OPENDSPX_MODEL_PARAMS_H

#include <map>
#include <string>

#include <opendspx/param.h>

namespace opendspx {

    class Params : public std::map<std::string, Param> {
    public:
        using std::map<std::string, Param>::map;
    };

}

#endif //OPENDSPX_MODEL_PARAMS_H