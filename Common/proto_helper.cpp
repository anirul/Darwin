#include "proto_helper.h"

namespace proto {

    bool operator<(const proto::Character& lhs, const proto::Character& rhs) {
        return lhs.name() < rhs.name();
    }

}  // namespace darwin.
