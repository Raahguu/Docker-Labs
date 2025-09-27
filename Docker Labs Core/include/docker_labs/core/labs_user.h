#pragma once
#include <string>
#include <chrono>
#include "json/json.hpp"

using json = nlohmann::json;

namespace Docker_Labs::Labs_Core {

    // -----------------------------------------------------------------------------
    // User
    // -----------------------------------------------------------------------------

    /// <summary>
    /// Basic representation of a Cloudflare user using their email.
    /// </summary>
    class User {
    public:
        /// <summary>
        /// Initializes a new instance of the <see cref="User"/> class with the specified email.
        /// </summary>
        /// <param name="email">The user's email address.</param>
        explicit User(std::string email);

        /// <summary>
        /// Gets the user's email address.
        /// </summary>
        /// <returns>The email address string.</returns>
        std::string Get_Email();

    protected:
        /// <summary>
        /// The user's email address.
        /// </summary>
        const std::string email;
    };

    // -----------------------------------------------------------------------------
    // User_Seat
    // -----------------------------------------------------------------------------

    /// <summary>
    /// Represents a detailed Cloudflare Access seat (extends User).
    /// Holds metadata for each user seat including permissions, account associations, timestamps, and device counts.
    /// </summary>
    class User_Seat : public User {
    public:
        /// <summary>
        /// Initializes a new instance of the <see cref="User_Seat"/> class from a JSON response.
        /// </summary>
        /// <param name="seat_json">JSON object containing seat data.</param>
        explicit User_Seat(json seat_json);

        /// <summary>
        /// Initializes a new instance of the <see cref="User_Seat"/> class with explicit values.
        /// </summary>
        /// <param name="email">The user's email address.</param>
        /// <param name="user_id">The user ID.</param>
        /// <param name="name">The user's name.</param>
        /// <param name="id">The seat ID.</param>
        /// <param name="access_seat">Whether the user has an Access seat.</param>
        /// <param name="gateway_seat">Whether the user has a Gateway seat.</param>
        /// <param name="active_device_count">Number of active devices.</param>
        /// <param name="seat_uid">The seat UID.</param>
        /// <param name="uid">The user UID.</param>
        /// <param name="created_at">Creation timestamp.</param>
        /// <param name="updated_at">Last update timestamp.</param>
        /// <param name="last_successful_login">Last successful login timestamp.</param>
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

        /// <summary>
        /// Gets the seat ID.
        /// </summary>
        /// <returns>The seat ID string.</returns>
        std::string Get_ID();

        /// <summary>
        /// Gets the user's name.
        /// </summary>
        /// <returns>The name string.</returns>
        std::string Get_Name();

        /// <summary>
        /// Gets the user ID.
        /// </summary>
        /// <returns>The user ID string.</returns>
        std::string Get_UserID();

        /// <summary>
        /// Gets whether the user has an Access seat.
        /// </summary>
        /// <returns>True if Access seat, otherwise false.</returns>
        bool Get_AccessSeat();

        /// <summary>
        /// Gets whether the user has a Gateway seat.
        /// </summary>
        /// <returns>True if Gateway seat, otherwise false.</returns>
        bool Get_GatewaySeat();

        /// <summary>
        /// Gets the number of active devices.
        /// </summary>
        /// <returns>Active device count.</returns>
        int Get_ActiveDeviceCount();

        /// <summary>
        /// Gets the seat UID.
        /// </summary>
        /// <returns>The seat UID string.</returns>
        std::string Get_SeatUID();

        /// <summary>
        /// Gets the user UID.
        /// </summary>
        /// <returns>The user UID string.</returns>
        std::string Get_UID();

        /// <summary>
        /// Gets the creation timestamp.
        /// </summary>
        /// <returns>The creation timestamp string.</returns>
        std::string Get_CreatedAt();

        /// <summary>
        /// Gets the last update timestamp.
        /// </summary>
        /// <returns>The last update timestamp string.</returns>
        std::string Get_UpdatedAt();

        /// <summary>
        /// Gets the last successful login timestamp.
        /// </summary>
        /// <returns>The last successful login timestamp string.</returns>
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

