#pragma once
#include "../http_server.h"

namespace api {
namespace pam {

void registerEndpoints(SimpleHttpServer& server);

// Handlery endpointów
Poco::JSON::Object handleAuthenticate(const Poco::JSON::Object& request);
Poco::JSON::Object handleVerify(const Poco::JSON::Object& request);
Poco::JSON::Object handleGetStatus(const Poco::JSON::Object& request);

}  // namespace pam
}  // namespace api 