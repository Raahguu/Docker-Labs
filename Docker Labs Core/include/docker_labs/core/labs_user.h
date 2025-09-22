#pragma once
#include <string>
#include <chrono>
#include "json/json.hpp"

using json = nlohmann::json;
namespace Docker_Labs::Labs_Core {

    class User {
    public:
        User(std::string email);
        std::string Get_Email();
    protected:
        const std::string email;
    };

    class User_Seat : public User {
    public:
        User_Seat(json seat_json);

        User_Seat(std::string email, std::string user_id, std::string name, std::string id, bool access_seat,
            bool gateway_seat, int active_device_count, std::string seat_uid, std::string uid,
            std::string created_at, std::string updated_at,
            std::string last_successful_login);

        std::string Get_ID();

        std::string Get_Name();

        std::string Get_UserID();

        bool Get_AccessSeat();

        bool Get_GatewaySeat();

        int Get_ActiveDeviceCount();

        std::string Get_SeatUID();

        std::string Get_UID();

        std::string Get_CreatedAt();

        std::string Get_UpdatedAt();

        std::string Get_LastSuccessfulLogin();

    private:
        std::string user_id;
        std::string name;
        std::string id;
        bool access_seat;
        bool gateway_seat;
        int active_device_count;
        std::string seat_uid;
        std::string uid;
        std::string created_at;
        std::string updated_at;
        std::string last_successful_login;
    };
}