#include "docker_labs/core/labs_user.h"
#include "docker_labs/core/cloudflare_hook.h"

using namespace Docker_Labs;

Labs_Core::User::User(std::string email)
    : email(email) {
}

Labs_Core::User_Seat::User_Seat(json user_json) :
    User(user_json.value("email","")), user_id(user_json.value("id", "")), name(user_json.value("name", "")),
    access_seat(user_json.value("access_seat", false)), gateway_seat(user_json.value("gateway_seat", false)),
    active_device_count(user_json.value("active_device_count", 0)), seat_uid(user_json.value("seat_uid", "")),
    uid(user_json.value("uid", "")), created_at(user_json.value("created_at", "")), updated_at(user_json.value("updated_at", "")),
    last_successful_login(user_json.value("last_successful_login", "")) {}

Labs_Core::User_Seat::User_Seat(std::string email, std::string user_id, std::string name, std::string id, bool access_seat,
    bool gateway_seat, int active_device_count, std::string seat_uid, std::string uid,
    std::string created_at, std::string updated_at,
    std::string last_successful_login)
    : User(email), user_id(user_id), name(name), id(id), access_seat(access_seat),
    gateway_seat(gateway_seat), active_device_count(active_device_count), seat_uid(seat_uid),
    uid(uid), created_at(created_at), updated_at(updated_at), last_successful_login(last_successful_login) {
}

std::string Labs_Core::User::Get_Email() {
    return email;
}

std::string Labs_Core::User_Seat::Get_ID() {
    return user_id;
}

std::string Labs_Core::User_Seat::Get_Name() {
    return name;
}

std::string Labs_Core::User_Seat::Get_UserID() {
    return id;
}

bool Labs_Core::User_Seat::Get_AccessSeat() {
    return access_seat;
}

bool Labs_Core::User_Seat::Get_GatewaySeat() {
    return gateway_seat;
}

int Labs_Core::User_Seat::Get_ActiveDeviceCount() {
    return active_device_count;
}

std::string Labs_Core::User_Seat::Get_SeatUID() {
    return seat_uid;
}

std::string Labs_Core::User_Seat::Get_UID() {
    return uid;
}

std::string Labs_Core::User_Seat::Get_CreatedAt() {
    return created_at;
}

std::string Labs_Core::User_Seat::Get_UpdatedAt() {
    return updated_at;
}

std::string Labs_Core::User_Seat::Get_LastSuccessfulLogin() {
    return last_successful_login;
}