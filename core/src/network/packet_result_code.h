#ifndef PACKET_RESULT_CODE__
#define PACKET_RESULT_CODE__

#include <inttypes.h>
#include "packet.h"

enum
packet_result
{
	RESULT_ERROR_DATABASE,
	RESULT_ERROR_UNKNOWN,

	RESULT_USER_REGISTER_SUCCESS,
	RESULT_USER_REGISTER_INVALID_EMAIL,
	RESULT_USER_REGISTER_USERNAME_TOO_LONG,
	RESULT_USER_REGISTER_USERNAME_EXISTS,
	RESULT_USER_REGISTER_PASSWORD_TOO_SHORT,
	RESULT_USER_REGISTER_PASSWORD_TOO_LONG,

	RESULT_USER_LOGIN_SUCCESS,
	RESULT_USER_LOGIN_INVALID_PASSWORD,
	RESULT_USER_LOGIN_USERNAME_DOESNT_EXIST,
	RESULT_USER_LOGIN_ACCOUNT_LOGGED_IN,
	RESULT_USER_LOGIN_BANNED,

};

char*
packet_result(enum packet_type type, uint16_t result);


#endif

