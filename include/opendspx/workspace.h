#ifndef OPENDSPX_WORKSPACE_H
#define OPENDSPX_WORKSPACE_H

#include <functional>
#include <map>
#include <string>

#include <stdcorelib/support/json.h>

namespace opendspx{

    // A class of its own rather than an alias for the map, so that it stays a distinct type for
    // template matching. An alias would be the same type as any other map spelled the same way,
    // and the serializer's Mapping<Workspace> specialization would then claim that type too.
    class Workspace : public std::map<std::string, stdc::JsonObject, std::less<>> {
    public:
        using std::map<std::string, stdc::JsonObject, std::less<>>::map;
    };

}

#endif //OPENDSPX_WORKSPACE_H
