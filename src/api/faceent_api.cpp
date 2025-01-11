#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <sstream>
#include "api/faceent_api.h"

namespace api {
namespace faceent {

void registerEndpoints(SimpleHttpServer& server) {
    // server.addEndpoint("/api/v1/face/detect", handleDetect);
    server.addEndpoint("/api/v1/face/recognize", handleRecognize);
    // server.addEndpoint("/api/v1/face/enroll", handleEnroll);
    server.addEndpoint("/api/v1/face/train", handleTrain);
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

Poco::JSON::Object handleTrain(const Poco::JSON::Object& request) {
    Poco::JSON::Object response;
    
    // Dodaj debugowanie
    std::ostringstream debugStr;
    request.stringify(debugStr);
    std::cerr << "Received request: " << debugStr.str() << std::endl;
    
    if (request.has("video_path")) {
        std::string videoPath = request.getValue<std::string>("video_path");
        
        // Dodaj debugowanie
        std::cerr << "Video path: " << videoPath << std::endl;
        
        // Przygotowanie żądania do serwera Pythona
        Poco::Net::HTTPClientSession session("localhost", 8081);
        Poco::Net::HTTPRequest pythonRequest(Poco::Net::HTTPRequest::HTTP_POST, "/train");
        pythonRequest.setContentType("application/json");
        
        // Tworzenie body żądania
        Poco::JSON::Object requestBody;
        requestBody.set("video_path", videoPath);
        std::stringstream ss;
        requestBody.stringify(ss);
        
        // Wysłanie żądania
        pythonRequest.setContentLength(ss.str().length());
        std::ostream& requestStream = session.sendRequest(pythonRequest);
        requestStream << ss.str();
        
        // Odbieranie odpowiedzi
        Poco::Net::HTTPResponse pythonResponse;
        std::istream& responseStream = session.receiveResponse(pythonResponse);
        
        if (pythonResponse.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
            response.set("status", "success");
            response.set("message", "Training request sent successfully");
        } else {
            response.set("status", "error");
            response.set("message", "Failed to send training request");
        }
    } else {
        std::cerr << "video_path field not found in request" << std::endl;
        response.set("error", "No video_path provided");
    }
    
    return response;
}

}  // namespace faceent
}  // namespace api 