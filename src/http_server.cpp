#include "http_server.h"
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/JSON/Parser.h>
#include <Poco/DateTime.h>
#include <Poco/DateTimeFormatter.h>
#include <iostream>
#include <iomanip>
#include <sstream>

void SimpleHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                                Poco::Net::HTTPServerResponse& response) {
    auto timestamp = std::time(nullptr);
    
    // Dodaj nagłówki CORS
    response.add("Access-Control-Allow-Origin", "http://localhost:1420");
    response.add("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response.add("Access-Control-Allow-Headers", "Content-Type");
    response.add("Access-Control-Allow-Credentials", "true");
    
    // Logowanie requestu
    std::cerr << "[" << timestamp << "] " 
              << request.getMethod() << " " 
              << request.getURI() << std::endl;
    
    // Obsługa preflight request
    if (request.getMethod() == "OPTIONS") {
        response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_OK);
        response.send();
        return;
    }
    
    try {
        std::string body;
        Poco::JSON::Object requestJson;
        
        // Sprawdź czy jest Content-Length
        if (request.hasContentLength()) {
            // Czytamy body requestu tylko jeśli jest Content-Length
            std::istream& stream = request.stream();
            auto contentLength = request.getContentLength();
            
            if (contentLength > 0) {
                body.reserve(contentLength);
                while (stream.good() && body.length() < contentLength) {
                    char buffer[128];
                    stream.read(buffer, std::min(sizeof(buffer), contentLength - body.length()));
                    body.append(buffer, stream.gcount());
                }
                
                // Debugowanie
                std::cerr << "Received body: " << body << std::endl;
                
                // Parsuj JSON tylko jeśli body nie jest puste
                if (!body.empty()) {
                    Poco::JSON::Parser parser;
                    Poco::Dynamic::Var result = parser.parse(body);
                    Poco::JSON::Object::Ptr jsonPtr = result.extract<Poco::JSON::Object::Ptr>();
                    requestJson = *jsonPtr;
                }
            }
        }
        
        // Wywołanie handlera
        Poco::JSON::Object responseJson = _handler(requestJson);
        
        // Ustawienie nagłówków odpowiedzi
        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        response.setContentType("application/json");
        
        // Przygotowanie i wysłanie odpowiedzi
        std::ostringstream responseStream;
        responseJson.stringify(responseStream);
        std::string responseStr = responseStream.str();
        
        // Logowanie odpowiedzi
        std::cerr << "[" << timestamp << "] Response: " << responseStr << std::endl;
        
        std::ostream& out = response.send();
        out << responseStr;
        out.flush();

    } catch (const std::exception& e) {
        std::string errorJson = "{\"error\": \"" + std::string(e.what()) + "\"}";
        std::cerr << "[" << timestamp << "] Błąd: " << e.what() << std::endl;

        response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        response.send() << errorJson;
    }
}

void HandlerFactory::addEndpoint(const std::string& path, JsonHandler handler) {
    _handlers[path] = handler;
}

Poco::Net::HTTPRequestHandler* HandlerFactory::createRequestHandler(
    const Poco::Net::HTTPServerRequest& request) {
    auto it = _handlers.find(request.getURI());
    if (it != _handlers.end()) {
        return new SimpleHandler(it->second);
    }
    return nullptr;
}

SimpleHttpServer::SimpleHttpServer(int port) : 
    _port(port),
    _maxConnections(SERVER_MAX_CONNECTIONS),
    _queueSize(SERVER_QUEUE_SIZE),
    _threadCount(SERVER_THREADS),
    _sslEnabled(SERVER_SSL_ENABLED),
    _server(nullptr) {
    _factory = new HandlerFactory();
}

void SimpleHttpServer::addEndpoint(const std::string& path, JsonHandler handler) {
    _factory->addEndpoint(path, handler);
}

void SimpleHttpServer::setMaxConnections(int max) {
    _maxConnections = max;
}

void SimpleHttpServer::setQueueSize(int size) {
    _queueSize = size;
}

void SimpleHttpServer::setThreadCount(int threads) {
    _threadCount = threads;
}

void SimpleHttpServer::setSSLEnabled(bool enabled) {
    _sslEnabled = enabled;
}

void SimpleHttpServer::start() {
    Poco::Net::ServerSocket socket(_port);
    Poco::Net::HTTPServerParams* params = new Poco::Net::HTTPServerParams;
    
    params->setMaxQueued(_queueSize);
    params->setMaxThreads(_threadCount);
    
    _server = new Poco::Net::HTTPServer(_factory, socket, params);
    _server->start();
    
    std::cerr << "Server started on port " << _port << std::endl;
}

void SimpleHttpServer::stop() {
    if (_server) {
        _server->stop();
        delete _server;
        _server = nullptr;
        std::cerr << "Server stopped" << std::endl;
    }
}