#include "Server/world_state.h"
#include <gtest/gtest.h>

namespace test {

    class WorldStateTest : public testing::Test {
    public:
        WorldStateTest() = default;
        proto::Element CreateBasicGroundElement(
            proto::Vector3 vector3, 
            double mass, 
            double radius) const;
        proto::Vector3 CreateBasicVector3(double x, double y, double z) const;

    protected:
        std::unique_ptr<darwin::WorldState> world_state_;
    };

} // namespace test.
