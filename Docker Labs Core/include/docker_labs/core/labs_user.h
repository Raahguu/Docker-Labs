// -----------------------------------------------------------------------------
// @file labs_user.h
// @brief Declares the User and User_Seat classes for representing Cloudflare users.
//
// The `User` class represents a basic user with an email identity.
// The `User_Seat` class extends `User` and includes additional metadata and
// seat-specific properties (used for managing Cloudflare Access seats).
//
// Dependencies:
// - nlohmann::json for parsing Cloudflare JSON responses.
//
// Namespace:
// - Docker_Labs::Labs_Core
// -----------------------------------------------------------------------------

#pragma once
#include <string>
#include <chrono>
#include "json/json.hpp"

using json = nlohmann::json;

namespace Docker_Labs::Labs_Core {

    // -----------------------------------------------------------------------------
    // @class User
    // @brief Basic representation of a Cloudflare user using their email.
    // -----------------------------------------------------------------------------
    class User {
    public:
        explicit User(std::string email);
        std::string Get_Email();

    protected:
        const std::string email;
    };

    // -----------------------------------------------------------------------------
    // @class User_Seat
    // @brief Represents a detailed Cloudflare Access seat (extends User).
    //
    // This class holds metadata for each user seat including permissions,
    // account associations, timestamps, and device counts.
    // -----------------------------------------------------------------------------
    class User_Seat : public User {
    public:
        // Constructor from JSON response
        explicit User_Seat(json seat_json);

        // Manual constructor (used for mock/testing or direct population)
        User_Seat(
            std::string email,
            std::string user_id,
            std::string name,
            std::string id,
            bool access_seat,
            bool gateway_seat,
            int active_device_count,
            std::string seat_uid,
            std::string uid,
            std::string created_at,
            std::string updated_at,
            std::string last_successful_login
        );

        // Getters for metadata
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

} // namespace Docker_Labs::Labs_Core
