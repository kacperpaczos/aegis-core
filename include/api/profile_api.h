#pragma once
#include "../http_server.h"
#include <Poco/JSON/Object.h>
#include <optional>
#include <vector>
#include <string>

namespace api {
namespace profile {

struct ProfileData {
    std::string id;
    std::string name;
    std::optional<std::string> pin;
    std::string avatar;
    std::string background;
    std::vector<std::string> photos;
    std::vector<std::string> videos;
    std::string created_at;
    std::string updated_at;
    
    Poco::JSON::Object toJson() const;
    static ProfileData fromJson(const Poco::JSON::Object& json);
};

void registerEndpoints(SimpleHttpServer& server);

// Handlery endpointów
Poco::JSON::Object handleSyncProfiles(const Poco::JSON::Object& request);
Poco::JSON::Object handleGetAllProfiles(const Poco::JSON::Object& request);

// Funkcje pomocnicze
bool saveProfile(const ProfileData& profile);
std::optional<ProfileData> loadProfile(const std::string& id);
std::vector<ProfileData> listAllProfiles();

}  // namespace profile
}  // namespace api 