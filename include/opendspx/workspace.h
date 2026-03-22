#ifndef OPENDSPX_MODEL_WORKSPACE_H
#define OPENDSPX_MODEL_WORKSPACE_H

#include <map>
#include <string>

#include <nlohmann/json.hpp>

namespace opendspx{

    class Workspace : public std::map<std::string, nlohmann::json> {
    public:
        using std::map<std::string, nlohmann::json>::map;
    };

}

#endif //OPENDSPX_MODEL_WORKSPACE_H
