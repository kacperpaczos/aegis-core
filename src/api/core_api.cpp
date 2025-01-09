#include "api/core_api.h"

namespace api {
namespace core {

void registerEndpoints(SimpleHttpServer& server) {
    server.addEndpoint("/api/v1/health", handleHealthCheck);
    server.addEndpoint("/api/v1/version", handleVersion);
    server.addEndpoint("/api/v1/status", handleStatus);
}

Poco::JSON::Object handleHealthCheck(const Poco::JSON::Object&) {
    Poco::JSON::Object response;
    response.set("status", "ok");
    response.set("version", std::string(SERVER_VERSION));
    return response;
}

Poco::JSON::Object handleVersion(const Poco::JSON::Object&) {
    Poco::JSON::Object response;
    response.set("version", std::string(SERVER_VERSION));
    response.set("api_version", "1.0");
    return response;
}

Poco::JSON::Object handleStatus(const Poco::JSON::Object&) {
    Poco::JSON::Object response;
    response.set("status", "running");
    response.set("uptime", 123); // TODO: implement actual uptime
    return response;
}

}  // namespace core
}  // namespace api 