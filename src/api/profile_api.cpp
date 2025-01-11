#include "api/profile_api.h"
#include <Poco/JSON/Parser.h>
#include <Poco/File.h>
#include <Poco/Path.h>
#include <fstream>
#include <filesystem>

namespace api {
namespace profile {

namespace {
    const std::string PROFILES_DIR = "profiles";
    
    std::string getProfilePath(const std::string& name) {
        return Poco::Path(PROFILES_DIR, name + ".json").toString();
    }
}

Poco::JSON::Object ProfileData::toJson() const {
    Poco::JSON::Object json;
    json.set("id", id);
    json.set("name", name);
    if (pin) {
        json.set("pin", *pin);
    }
    json.set("avatar", avatar);
    json.set("background", background);
    
    Poco::JSON::Array photosArray;
    for (const auto& photo : photos) {
        photosArray.add(photo);
    }
    json.set("photos", photosArray);
    
    Poco::JSON::Array videosArray;
    for (const auto& video : videos) {
        videosArray.add(video);
    }
    json.set("videos", videosArray);
    
    json.set("createdAt", created_at);
    json.set("updatedAt", updated_at);
    
    return json;
}

ProfileData ProfileData::fromJson(const Poco::JSON::Object& json) {
    ProfileData profile;
    profile.id = json.getValue<std::string>("id");
    profile.name = json.getValue<std::string>("name");
    
    if (json.has("pin")) {
        profile.pin = json.getValue<std::string>("pin");
    }
    
    profile.avatar = json.getValue<std::string>("avatar");
    profile.background = json.getValue<std::string>("background");
    
    auto photosArray = json.getArray("photos");
    for (size_t i = 0; i < photosArray->size(); ++i) {
        profile.photos.push_back(photosArray->getElement<std::string>(i));
    }
    
    auto videosArray = json.getArray("videos");
    for (size_t i = 0; i < videosArray->size(); ++i) {
        profile.videos.push_back(videosArray->getElement<std::string>(i));
    }
    
    profile.created_at = json.getValue<std::string>("createdAt");
    profile.updated_at = json.getValue<std::string>("updatedAt");
    
    return profile;
}

bool saveProfile(const ProfileData& profile) {
    try {
        // Upewnij się, że katalog profiles istnieje
        Poco::File profilesDir(PROFILES_DIR);
        if (!profilesDir.exists()) {
            profilesDir.createDirectories();
        }

        // Zapisz profil do pliku JSON
        std::ofstream file(getProfilePath(profile.id));
        if (!file) return false;

        Poco::JSON::Object json = profile.toJson();
        json.stringify(file);
        return true;
    } catch (...) {
        return false;
    }
}

std::optional<ProfileData> loadProfile(const std::string& id) {
    try {
        std::ifstream file(getProfilePath(id));
        if (!file) return std::nullopt;

        std::string json_str((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(json_str);
        Poco::JSON::Object::Ptr json = result.extract<Poco::JSON::Object::Ptr>();

        return ProfileData::fromJson(*json);
    } catch (...) {
        return std::nullopt;
    }
}

void registerEndpoints(SimpleHttpServer& server) {
    server.addEndpoint("/api/v1/profile/sync", handleSyncProfiles);
    server.addEndpoint("/api/v1/profile/get-all", handleGetAllProfiles);
}

Poco::JSON::Object handleSyncProfiles(const Poco::JSON::Object& request) {
    Poco::JSON::Object response;
    
    try {
        if (!request.has("profiles")) {
            response.set("status", "error");
            response.set("message", "Brak wymaganego pola: profiles");
            return response;
        }

        auto profilesArray = request.getArray("profiles");
        std::vector<ProfileData> profiles;
        
        std::cerr << "Otrzymano żądanie synchronizacji " << profilesArray->size() << " profili" << std::endl;
        
        // Konwertuj każdy profil z JSON na obiekt ProfileData
        for (size_t i = 0; i < profilesArray->size(); ++i) {
            auto profileJson = profilesArray->getObject(i);
            auto profile = ProfileData::fromJson(*profileJson);
            std::cerr << "Przetwarzanie profilu: " << profile.name << " (ID: " << profile.id << ")" << std::endl;
            profiles.push_back(profile);
        }
        
        // Zapisz wszystkie profile
        for (const auto& profile : profiles) {
            if (saveProfile(profile)) {
                std::cerr << "Zapisano profil: " << profile.name << " (ID: " << profile.id << ")" << std::endl;
            } else {
                std::cerr << "Błąd podczas zapisu profilu: " << profile.name << " (ID: " << profile.id << ")" << std::endl;
            }
        }
        
        response.set("status", "success");
        response.set("message", "Profile zostały zsynchronizowane");
        
    } catch (const std::exception& e) {
        std::cerr << "Błąd podczas synchronizacji profili: " << e.what() << std::endl;
        response.set("status", "error");
        response.set("message", std::string("Błąd: ") + e.what());
    }
    
    return response;
}

Poco::JSON::Object handleGetAllProfiles(const Poco::JSON::Object&) {
    Poco::JSON::Object response;
    Poco::JSON::Array profilesArray;
    
    try {
        std::cerr << "Rozpoczęto pobieranie wszystkich profili" << std::endl;
        
        auto profiles = listAllProfiles();
        std::cerr << "Znaleziono " << profiles.size() << " profili" << std::endl;
        
        for (const auto& profile : profiles) {
            profilesArray.add(profile.toJson());
            std::cerr << "Dodano profil do odpowiedzi: " << profile.name << " (ID: " << profile.id << ")" << std::endl;
        }
        
        response.set("status", "success");
        response.set("profiles", profilesArray);
        std::cerr << "Zakończono przygotowywanie odpowiedzi z profilami" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Błąd podczas pobierania profili: " << e.what() << std::endl;
        response.set("status", "error");
        response.set("message", std::string("Błąd: ") + e.what());
    }
    
    return response;
}

std::vector<ProfileData> listAllProfiles() {
    std::vector<ProfileData> profiles;
    Poco::File dir(PROFILES_DIR);
    
    if (dir.exists() && dir.isDirectory()) {
        std::vector<std::string> files;
        dir.list(files);
        
        for (const auto& file : files) {
            if (Poco::Path(file).getExtension() == "json") {
                std::string id = Poco::Path(file).getBaseName();
                auto profile = loadProfile(id);
                if (profile) {
                    profiles.push_back(*profile);
                }
            }
        }
    }
    
    return profiles;
}

}  // namespace profile
}  // namespace api 