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
    
    try {
        // Czytamy body requestu
        std::string body;
        std::istream& stream = request.stream();
        while (stream.good()) {
            char buffer[128];
            stream.read(buffer, sizeof(buffer));
            body.append(buffer, stream.gcount());
        }
        
        // Debugowanie
        std::cerr << "Received body: " << body << std::endl;
        
        Poco::JSON::Object requestJson;
        
        // Parsuj JSON tylko jeśli body nie jest puste
        if (!body.empty()) {
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse(body);
            Poco::JSON::Object::Ptr jsonPtr = result.extract<Poco::JSON::Object::Ptr>();
            requestJson = *jsonPtr;
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
}

void SimpleHttpServer::stop() {
    if (_server) {
        _server->stop();
        delete _server;
        _server = nullptr;
    }
} 