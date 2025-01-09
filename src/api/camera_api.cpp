#include "api/camera_api.h"
#include <Poco/JSON/Array.h>

namespace api {
namespace camera {

void registerEndpoints(SimpleHttpServer& server) {
    server.addEndpoint("/api/v1/camera/stream", handleGetStream);
    server.addEndpoint("/api/v1/camera/config", handleSetConfig);
    server.addEndpoint("/api/v1/camera/devices", handleGetDevices);
}

Poco::JSON::Object handleGetStream(const Poco::JSON::Object& request) {
    Poco::JSON::Object response;
    
    if (request.has("device_id")) {
        response.set("stream_url", "rtsp://localhost:8554/stream1");
        response.set("format", "h264");
        response.set("resolution", "1280x720");
    } else {
        response.set("error", "No device_id provided");
    }
    
    return response;
}

Poco::JSON::Object handleSetConfig(const Poco::JSON::Object& request) {
    Poco::JSON::Object response;
    
    if (request.has("device_id") && request.has("config")) {
        response.set("status", "success");
        response.set("message", "Camera configuration updated");
    } else {
        response.set("error", "Invalid configuration parameters");
    }
    
    return response;
}

Poco::JSON::Object handleGetDevices(const Poco::JSON::Object&) {
    Poco::JSON::Object response;
    Poco::JSON::Array devices;
    
    Poco::JSON::Object device1;
    device1.set("id", "cam1");
    device1.set("name", "USB Camera");
    device1.set("status", "active");
    devices.add(device1);
    
    Poco::JSON::Object device2;
    device2.set("id", "cam2");
    device2.set("name", "IP Camera");
    device2.set("status", "inactive");
    devices.add(device2);
    
    response.set("devices", devices);
    return response;
}

}  // namespace camera
}  // namespace api 