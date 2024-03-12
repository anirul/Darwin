#pragma once

#include "Common/darwin_service.grpc.pb.h"

namespace proto {

    bool operator<(const proto::Character& lhs, const proto::Character& rhs);

}  // namespace darwin.
