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
    
    std::cerr << "Otrzymano żądanie trenowania" << std::endl;
    
    if (request.has("video_path")) {
        std::string videoPath = request.getValue<std::string>("video_path");
        std::cerr << "Ścieżka do wideo: " << videoPath << std::endl;
        
        try {
            // Zmiana portu z 8081 na 8080
            Poco::Net::HTTPClientSession session("localhost", 8080);
            Poco::Net::HTTPRequest pythonRequest(Poco::Net::HTTPRequest::HTTP_POST, "/train");
            pythonRequest.setContentType("application/json");
            
            // Przygotowanie body żądania
            Poco::JSON::Object requestBody;
            requestBody.set("video_path", videoPath);
            std::stringstream ss;
            requestBody.stringify(ss);
            
            std::cerr << "Wysyłanie żądania do serwera Python: " << ss.str() << std::endl;
            
            pythonRequest.setContentLength(ss.str().length());
            std::ostream& requestStream = session.sendRequest(pythonRequest);
            requestStream << ss.str();
            requestStream.flush();
            
            Poco::Net::HTTPResponse pythonResponse;
            std::istream& responseStream = session.receiveResponse(pythonResponse);
            
            std::cerr << "Otrzymano odpowiedź od serwera Python: " << pythonResponse.getStatus() << std::endl;
            
            if (pythonResponse.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
                response.set("status", "success");
                response.set("message", "Rozpoczęto trenowanie modelu");
            } else {
                response.set("status", "error");
                response.set("message", "Błąd podczas rozpoczynania trenowania");
            }
        } catch (const std::exception& e) {
            std::cerr << "Błąd podczas komunikacji z serwerem Python: " << e.what() << std::endl;
            response.set("status", "error");
            response.set("message", std::string("Błąd: ") + e.what());
        }
    } else {
        std::cerr << "Brak wymaganego pola video_path" << std::endl;
        response.set("status", "error");
        response.set("message", "Brak wymaganego pola: video_path");
    }
    
    return response;
}

}  // namespace faceent
}  // namespace api 