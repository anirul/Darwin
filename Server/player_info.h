#pragma once

#include "Common/darwin_service.grpc.pb.h"

namespace darwin {

    struct PlayerInfo {
        double time;
        proto::Player player;
    };

}  // End namespace darwin.
