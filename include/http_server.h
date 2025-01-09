#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "config.h"
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/JSON/Object.h>
#include <functional>
#include <map>
#include <string>

using JsonHandler = std::function<Poco::JSON::Object(const Poco::JSON::Object&)>;

class SimpleHandler : public Poco::Net::HTTPRequestHandler {
public:
    SimpleHandler(const JsonHandler& handler) : _handler(handler) {}
    void handleRequest(Poco::Net::HTTPServerRequest& request, 
                      Poco::Net::HTTPServerResponse& response) override;
private:
    JsonHandler _handler;
};

class HandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
public:
    void addEndpoint(const std::string& path, JsonHandler handler);
    Poco::Net::HTTPRequestHandler* createRequestHandler(
        const Poco::Net::HTTPServerRequest& request) override;
private:
    std::map<std::string, JsonHandler> _handlers;
};

class SimpleHttpServer {
public:
    SimpleHttpServer(int port = SERVER_PORT);
    void addEndpoint(const std::string& path, JsonHandler handler);
    void start();
    void stop();
    void setMaxConnections(int max);
    void setQueueSize(int size);
    void setThreadCount(int threads);
    void setSSLEnabled(bool enabled);

private:
    int _port;
    int _maxConnections;
    int _queueSize;
    int _threadCount;
    bool _sslEnabled;
    Poco::Net::HTTPServer* _server;
    HandlerFactory* _factory;
};

#endif 