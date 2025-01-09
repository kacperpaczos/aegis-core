#include "api/profile_api.h"
#include <Poco/JSON/Array.h>

namespace api {
namespace profile {

void registerEndpoints(SimpleHttpServer& server) {
    server.addEndpoint("/api/v1/profile/list", handleListProfiles);
    server.addEndpoint("/api/v1/profile/get", handleGetProfile);
    server.addEndpoint("/api/v1/profile/update", handleUpdateProfile);
}

Poco::JSON::Object handleGetProfile(const Poco::JSON::Object& request) {
    Poco::JSON::Object response;
    
    if (request.has("id")) {
        response.set("id", request.get("id"));
        response.set("name", "Default Profile");
        response.set("created_at", "2024-03-20T10:00:00Z");
        response.set("settings", Poco::JSON::Object());
    } else {
        response.set("error", "Profile ID not provided");
    }
    
    return response;
}

Poco::JSON::Object handleUpdateProfile(const Poco::JSON::Object& request) {
    Poco::JSON::Object response;
    response.set("status", "success");
    response.set("message", "Profile updated successfully");
    return response;
}

Poco::JSON::Object handleListProfiles(const Poco::JSON::Object&) {
    Poco::JSON::Object response;
    Poco::JSON::Array profiles;
    
    // Przykładowe profile
    Poco::JSON::Object profile1;
    profile1.set("id", 1);
    profile1.set("name", "Default");
    profiles.add(profile1);
    
    Poco::JSON::Object profile2;
    profile2.set("id", 2);
    profile2.set("name", "Custom");
    profiles.add(profile2);
    
    response.set("profiles", profiles);
    return response;
}

}  // namespace profile
}  // namespace api 