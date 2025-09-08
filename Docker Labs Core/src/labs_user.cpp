#include "labs_user.h"

using namespace Docker_Labs;

Labs_Core::User::User(std::string email)
	: email(email) {}

std::string Labs_Core::User::Get_Email() {
	return email;
}