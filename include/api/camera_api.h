#pragma once
#include "../http_server.h"

namespace api {
namespace camera {

void registerEndpoints(SimpleHttpServer& server);

// Handlery endpointów
Poco::JSON::Object handleGetStream(const Poco::JSON::Object& request);
Poco::JSON::Object handleSetConfig(const Poco::JSON::Object& request);
Poco::JSON::Object handleGetDevices(const Poco::JSON::Object& request);

}  // namespace camera
}  // namespace api 