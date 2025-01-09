#pragma once
#include "../http_server.h"

namespace api {
namespace faceent {

void registerEndpoints(SimpleHttpServer& server);

// Handlery endpointów
Poco::JSON::Object handleDetect(const Poco::JSON::Object& request);
Poco::JSON::Object handleRecognize(const Poco::JSON::Object& request);
Poco::JSON::Object handleEnroll(const Poco::JSON::Object& request);

}  // namespace faceent
}  // namespace api 