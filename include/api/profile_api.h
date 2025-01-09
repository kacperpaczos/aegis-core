#pragma once
#include "../http_server.h"

namespace api {
namespace profile {

void registerEndpoints(SimpleHttpServer& server);

// Handlery endpointów
Poco::JSON::Object handleGetProfile(const Poco::JSON::Object& request);
Poco::JSON::Object handleUpdateProfile(const Poco::JSON::Object& request);
Poco::JSON::Object handleListProfiles(const Poco::JSON::Object& request);

}  // namespace profile
}  // namespace api 