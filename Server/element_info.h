#pragma once

#include "Common/darwin_service.grpc.pb.h"

namespace darwin {

struct ElementInfo {
  double time;
  proto::Element element;
};

}  // End namespace darwin.
