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
    Poco::DateTime now;
    std::string timestamp = Poco::DateTimeFormatter::format(now, "%Y-%m-%d %H:%M:%S");
    
    std::cout << "\n[" << timestamp << "] "
              << "Połączenie z " << request.clientAddress().toString()
              << " - " << request.getMethod()
              << " " << request.getURI() << std::endl;

    try {
        response.setContentType("application/json");
        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);

        // Dla GET tworzymy pusty obiekt JSON jako wejście
        Poco::JSON::Object inputJson;
        
        // Wywołanie handlera i uzyskanie odpowiedzi
        Poco::JSON::Object responseJson = _handler(inputJson);

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