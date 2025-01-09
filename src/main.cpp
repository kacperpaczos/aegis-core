#include "http_server.h"
#include "api/core_api.h"
#include "api/profile_api.h"
#include "api/faceent_api.h"
#include "api/camera_api.h"
#include "api/pam_api.h"
#include <iostream>

int main() {
    const int PORT = SERVER_PORT;
    const int MAX_THREADS = SERVER_THREADS;
    
    SimpleHttpServer server(PORT);
    
    // Rejestracja endpointów dla każdego modułu
    api::core::registerEndpoints(server);
    api::profile::registerEndpoints(server);
    api::faceent::registerEndpoints(server);
    api::camera::registerEndpoints(server);
    api::pam::registerEndpoints(server);

    std::cout << "Uruchamianie serwera na porcie " << PORT << "..." << std::endl;
    server.setThreadCount(MAX_THREADS);
    server.setQueueSize(SERVER_QUEUE_SIZE);
    
    server.start();
    std::cout << "Serwer uruchomiony. Naciśnij Enter aby zakończyć." << std::endl;
    std::cin.get();
    server.stop();

    return 0;
}