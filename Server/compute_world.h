#pragma once

#include "Server/darwin_service_impl.h"
#include "Server/world_state.h"

namespace darwin {

    void ComputeWorld(
        darwin::DarwinServiceImpl& service, 
        WorldState& world_state);

} // namespace darwin.
