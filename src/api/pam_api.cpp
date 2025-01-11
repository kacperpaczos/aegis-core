#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include "api/pam_api.h"

namespace api {
namespace pam {

void registerEndpoints(SimpleHttpServer& server) {
    // server.addEndpoint("/api/v1/pam/authenticate", handleAuthenticate);
    // server.addEndpoint("/api/v1/pam/verify", handleVerify);
    // server.addEndpoint("/api/v1/pam/status", handleGetStatus);
    server.addEndpoint("/api/v1/pam/record", handleRecord);
}

Poco::JSON::Object handleRecord(const Poco::JSON::Object& request) {
    Poco::JSON::Object response;
    
    std::cerr << "handleRecord został wywołany" << std::endl;
    
    try {
        std::cerr << "Próba połączenia z serwerem Python na localhost:8080" << std::endl;
        
        // Przygotowanie żądania do serwera Python
        Poco::Net::HTTPClientSession session("localhost", 8080);
        Poco::Net::HTTPRequest pythonRequest(Poco::Net::HTTPRequest::HTTP_POST, "/record", Poco::Net::HTTPMessage::HTTP_1_1);
        pythonRequest.setContentType("application/json");
        pythonRequest.setKeepAlive(false);
        
        // Przygotowanie body
        std::string body = "{}";
        pythonRequest.setContentLength(body.length());
        
        std::cerr << "Wysyłanie żądania do serwera Python" << std::endl;
        
        // Wysłanie żądania
        std::ostream& requestStream = session.sendRequest(pythonRequest);
        requestStream << body;
        requestStream.flush();
        
        std::cerr << "Oczekiwanie na odpowiedź od serwera Python" << std::endl;
        
        // Odbieranie odpowiedzi
        Poco::Net::HTTPResponse pythonResponse;
        std::istream& responseStream = session.receiveResponse(pythonResponse);
        
        std::cerr << "Otrzymano odpowiedź od serwera Python: " << pythonResponse.getStatus() << std::endl;
        
        if (pythonResponse.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
            response.set("status", "success");
            response.set("message", "Recording started");
        } else {
            response.set("status", "error");
            response.set("message", "Failed to start recording");
        }
    } catch (const std::exception& e) {
        std::cerr << "Wystąpił błąd: " << e.what() << std::endl;
        response.set("status", "error");
        response.set("message", std::string("Error: ") + e.what());
    }
    
    return response;
}

}  // namespace pam
}  // namespace api 