#pragma once

#include "Common/darwin_service.grpc.pb.h"

namespace darwin {

    struct CharacterInfo {
        double time;
        proto::Character character;
    };

}  // End namespace darwin.
