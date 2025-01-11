#include "api/profile_api.h"
#include <Poco/JSON/Parser.h>
#include <Poco/File.h>
#include <Poco/Path.h>
#include <fstream>
#include <filesystem>
#include <sys/stat.h>

namespace api {
namespace profile {

namespace {
    const std::string PROFILES_DIR = "./profiles";
    
    std::string getProfilePath(const std::string& name) {
        auto path = Poco::Path(PROFILES_DIR, name + ".json").toString();
        std::cerr << "Generowanie ścieżki dla profilu: " << path << std::endl;
        return path;
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
    
    std::cerr << "Parsowanie JSON na ProfileData..." << std::endl;
    
    try {
        // Wymagane pola
        profile.id = json.getValue<std::string>("id");
        profile.name = json.getValue<std::string>("name");
        
        // Opcjonalne pole PIN
        if (json.has("pin") && !json.isNull("pin")) {
            profile.pin = json.getValue<std::string>("pin");
        }
        
        // Pozostałe pola
        profile.avatar = json.getValue<std::string>("avatar");
        profile.background = json.getValue<std::string>("background");
        
        // Tablice
        if (json.has("photos")) {
            auto photosArray = json.getArray("photos");
            for (size_t i = 0; i < photosArray->size(); ++i) {
                profile.photos.push_back(photosArray->getElement<std::string>(i));
            }
        }
        
        if (json.has("videos")) {
            auto videosArray = json.getArray("videos");
            for (size_t i = 0; i < videosArray->size(); ++i) {
                profile.videos.push_back(videosArray->getElement<std::string>(i));
            }
        }
        
        // Daty - obsługa obu formatów nazw pól
        profile.created_at = json.has("createdAt") ? 
            json.getValue<std::string>("createdAt") : 
            json.getValue<std::string>("created_at");
            
        profile.updated_at = json.has("updatedAt") ? 
            json.getValue<std::string>("updatedAt") : 
            json.getValue<std::string>("updated_at");
        
        std::cerr << "Pomyślnie sparsowano profil: " << profile.name << " (ID: " << profile.id << ")" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Błąd podczas parsowania JSON na ProfileData: " << e.what() << std::endl;
        throw;
    }
    
    return profile;
}

bool saveProfile(const ProfileData& profile) {
    try {
        std::string baseDir = PROFILES_DIR;
        std::cerr << "Katalog bazowy: " << baseDir << std::endl;
        
        Poco::File profilesDir(baseDir);
        std::cerr << "Sprawdzanie katalogu: " << profilesDir.path() << std::endl;
        
        if (!profilesDir.exists()) {
            std::cerr << "Katalog nie istnieje, próba utworzenia: " << baseDir << std::endl;
            try {
                profilesDir.createDirectories();
                std::cerr << "Katalog utworzony pomyślnie" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Błąd podczas tworzenia katalogu: " << e.what() << std::endl;
                return false;
            }
            
            try {
                chmod(baseDir.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
                std::cerr << "Uprawnienia katalogu ustawione pomyślnie" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Błąd podczas ustawiania uprawnień katalogu: " << e.what() << std::endl;
                return false;
            }
        }
        
        std::string filePath = getProfilePath(profile.id);
        std::cerr << "Próba zapisu profilu do: " << filePath << std::endl;
        
        // Serializuj profil do JSON
        Poco::JSON::Object profileJson = profile.toJson();
        std::ostringstream oss;
        profileJson.stringify(oss);
        
        // Zapisz do pliku
        std::ofstream file(filePath, std::ios::out | std::ios::trunc);
        if (!file.is_open()) {
            std::cerr << "Nie można otworzyć pliku do zapisu: " << filePath << std::endl;
            return false;
        }
        
        file << oss.str();
        file.close();
        
        // Ustaw uprawnienia 644 dla pliku
        chmod(filePath.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        
        std::cerr << "Profil został pomyślnie zapisany: " << profile.name << " (ID: " << profile.id << ")" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Błąd podczas zapisu profilu: " << e.what() << std::endl;
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

        std::cerr << "Przetworzono " << profiles.size() << " profili" << std::endl;
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