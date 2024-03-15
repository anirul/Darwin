#pragma once

#include "Common/darwin_service.grpc.pb.h"

namespace proto {

    // Needed for map and set.
    bool operator<(const proto::Character& lhs, const proto::Character& rhs);

}  // namespace darwin.
