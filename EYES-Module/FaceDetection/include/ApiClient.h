#pragma once
#include <string>

class ApiClient {
private:
    std::string serverIp;
    int port;
public:
    ApiClient(std::string ip, int p);
    // Hàm bắn tín hiệu lên Java
    bool sendUnlockRequest(const std::string& camName, int userId, double distance);
};