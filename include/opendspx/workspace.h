#ifndef OPENDSPX_MODEL_WORKSPACE_H
#define OPENDSPX_MODEL_WORKSPACE_H

#include <stdcorelib/support/json.h>

namespace opendspx{

    class Workspace : public stdc::JsonObject {
    public:
        using stdc::JsonObject::map;
    };

}

#endif //OPENDSPX_MODEL_WORKSPACE_H
