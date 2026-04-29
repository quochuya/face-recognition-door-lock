#include "ApiClient.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <iostream>

ApiClient::ApiClient(std::string ip, int p) : serverIp(ip), port(p) {}

bool ApiClient::sendUnlockRequest(const std::string &camName, int userId, double distance)
{
    httplib::Client cli(serverIp, port);
    cli.set_connection_timeout(3, 0);
    cli.set_read_timeout(5, 0);
    nlohmann::json j;
    j["cam_id"] = camName;
    j["user_id"] = userId;
    j["sai_so"] = distance;

    auto res = cli.Post("/api/detect", j.dump(), "application/json");

    if (res)
    {
        std::cout << "[DEBUG API] Ket noi thong suot! Java tra ve ma: " << res->status << std::endl;

        if (res->status == 200)
        {
            std::cout << "-> [SUCCESS] Hop le, mo cua!" << std::endl;
            return true;
        }
        else if (res->status == 403 || res->status == 401)
        {
            std::cout << "-> [DENIED] Java tu choi vi ID nay khong co trong Web." << std::endl;
            return false;
        }
        else
        {
            std::cout << "-> [WARNING] Loi logic khac: " << res->status << std::endl;
            return false;
        }
    }
    else
    {
        auto err = res.error();
        std::cout << "[API FATAL] Mat ket noi hoan toan! Ly do: " << httplib::to_string(err) << std::endl;
        return false;
    }
}