#include "api/faceent_api.h"

namespace api {
namespace faceent {

void registerEndpoints(SimpleHttpServer& server) {
    server.addEndpoint("/api/v1/face/detect", handleDetect);
    server.addEndpoint("/api/v1/face/recognize", handleRecognize);
    server.addEndpoint("/api/v1/face/enroll", handleEnroll);
}

Poco::JSON::Object handleDetect(const Poco::JSON::Object& request) {
    Poco::JSON::Object response;
    
    if (request.has("image")) {
        Poco::JSON::Array faces;
        Poco::JSON::Object face;
        face.set("confidence", 0.95);
        face.set("x", 100);
        face.set("y", 100);
        face.set("width", 200);
        face.set("height", 200);
        faces.add(face);
        
        response.set("faces", faces);
    } else {
        response.set("error", "No image provided");
    }
    
    return response;
}

Poco::JSON::Object handleRecognize(const Poco::JSON::Object& request) {
    Poco::JSON::Object response;
    
    if (request.has("face_id")) {
        response.set("person_id", "123");
        response.set("confidence", 0.98);
        response.set("name", "John Doe");
    } else {
        response.set("error", "No face_id provided");
    }
    
    return response;
}

Poco::JSON::Object handleEnroll(const Poco::JSON::Object& request) {
    Poco::JSON::Object response;
    
    if (request.has("image") && request.has("person_id")) {
        response.set("status", "success");
        response.set("face_id", "face_123");
        response.set("quality_score", 0.95);
    } else {
        response.set("error", "Missing required parameters");
    }
    
    return response;
}

}  // namespace faceent
}  // namespace api 