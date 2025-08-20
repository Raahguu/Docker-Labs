#include "labs_user.h"

Docker_Labs::User::User(std::string email, std::string cf_uid)
	: email(email), cf_uid(cf_uid) {}

std::string Docker_Labs::User::Get_Email() {
	return email;
}

std::string Docker_Labs::User::Get_CF_UID() {
	return cf_uid;
}