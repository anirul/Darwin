#include "Common/darwin_service.grpc.pb.h"

#include <grpc++/grpc++.h>

namespace darwin {

class DarwinServiceImpl final : public proto::DarwinService::Service {
 public:
  grpc::Status Update(
      grpc::ServerContext* context, const proto::UpdateRequest* request,
      grpc::ServerWriter<proto::UpdateResponse>* writer) override;
};

}  // End namespace darwin.