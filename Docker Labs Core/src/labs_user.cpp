#include "labs_user.h"

Docker_Labs::User::User(std::string email)
	: email(email) {}

std::string Docker_Labs::User::Get_Email() {
	return email;
}