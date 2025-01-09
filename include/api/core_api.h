#pragma once
#include "../http_server.h"

namespace api {
namespace core {

void registerEndpoints(SimpleHttpServer& server);

// Handlery endpointów
Poco::JSON::Object handleHealthCheck(const Poco::JSON::Object& request);
Poco::JSON::Object handleVersion(const Poco::JSON::Object& request);
Poco::JSON::Object handleStatus(const Poco::JSON::Object& request);

}  // namespace core
}  // namespace api 