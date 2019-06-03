#include <string.h>
#include <math.h>
#include "db.h"
#include "db_info.h"
#include "util/log.h"
#include "util/util_funcs.h"
#include "network/packet_result_code.h"
#include "util/vec2.h"
#include "network/friend.h"
#include "../network/server_funcs.h"

struct database *d_b;

struct database*
db(void)
{
	return d_b;
}

uint8_t
db_init(void)
{
	d_b = malloc(sizeof(struct database));

	MYSQL *con = mysql_init(NULL);

	if (con == NULL)
	{
		log_fatal("Couldn't connect to MYSQL database. %s", mysql_error(con));
		return 1;
	}

	if (mysql_real_connect(
				con, 
				DB_HOST, 
				DB_USER, 
				DB_PASSWORD, 
				DB_DB, 
				DB_PORT, 
				NULL, 
				CLIENT_FOUND_ROWS) == NULL)
	{
		log_fatal("MYSQL connection failed.");
		return 1;
	}
	
	d_b->con = con;
	bool reconnect = true;
	mysql_options(con, MYSQL_OPT_RECONNECT, &reconnect);

	log_info("Database initialized.");

	return 0;
}

void
db_destroy(void)
{
	mysql_close(d_b->con);
	free(d_b);
	
	log_info("Databased freed.");
}

bool
db_character_exists(const char *name)
{
	uint16_t buf_size = 1024;		
	char query[buf_size];
	
	uint8_t name_len = strlen(name)+1;

	char e_name[name_len*2+1];
	
	mysql_real_escape_string(d_b->con, e_name, name, name_len);

	snprintf(query, buf_size, "SELECT * FROM characters WHERE name = '%s'", e_name);

	if (mysql_query(d_b->con, query) != 0)
	{
		log_error("Failed to run query for checking if user exists. %s", mysql_error(d_b->con));
	}
	
	MYSQL_RES *res = mysql_store_result(d_b->con);
	
	if (res == NULL)
	{
		log_error("Failed to get stored result for checking if user exists. %s", mysql_error(d_b->con));
	}

	MYSQL_ROW row;
	row = mysql_fetch_row(res);
	
	mysql_free_result(res);

	if (row == NULL) return false;

	return true;
}

bool
db_account_exists(const char *username)
{
	uint16_t buf_size = 1024;		
	char query[buf_size];
	
	uint8_t username_len = strlen(username)+1;

	char e_username[username_len*2+1];
	
	mysql_real_escape_string(d_b->con, e_username, username, username_len - 1);

	snprintf(query, buf_size, "SELECT * FROM accounts WHERE username = '%s'", e_username);

	if (mysql_query(d_b->con, query) != 0)
	{
		log_error("Failed to run query for checking if user exists. %s", mysql_error(d_b->con));
	}
	
	MYSQL_RES *res = mysql_store_result(d_b->con);
	
	if (res == NULL)
	{
		log_error(
				"Failed to get stored result for checking if user exists. %s", 
				mysql_error(d_b->con));
	}

	MYSQL_ROW row;
	row = mysql_fetch_row(res);
	
	mysql_free_result(res);


	if (row == NULL) return false;

	return true;
}

uint32_t
db_get_account_id(char *username)
{
	if (!db_account_exists(username)) return -1;

	username = trim_white_space(username);

	uint8_t username_len = strlen(username)+1;

	char e_username[username_len*2+1];
	
	mysql_real_escape_string(d_b->con, e_username, username, username_len - 1);

	uint16_t buf_size = 1024;		
	char query[buf_size];

	snprintf(
			query, 
			buf_size, 
			"SELECT * FROM accounts WHERE username = '%s'", 
			e_username);
	
	if (mysql_query(d_b->con, query) != 0) {
		log_error(
				"Failed to run query for logging user in: %s. %s", 
				username, 
				mysql_error(d_b->con));
		return RESULT_ERROR_DATABASE;
	}
	
	MYSQL_RES *res = mysql_store_result(d_b->con);
	
	if (res == NULL)
	{
		log_error("Failed to get stored result for users account ID %s. %s", 
				e_username, 
				mysql_error(d_b->con));
		return RESULT_ERROR_DATABASE;
	}

	MYSQL_ROW row = mysql_fetch_row(res);

	mysql_free_result(res);
	
	uint32_t u_id = atoi(row[0]);

	return u_id;
	
}

uint16_t
db_account_register(char *username, const char *password, char *email, uint32_t ip)
{	
	username = trim_white_space(username);
	email = trim_white_space(email);


	if (!is_valid_email(email) || strlen(email) > MAX_EMAIL_LEN)
		return RESULT_USER_REGISTER_INVALID_EMAIL;	

	if (strlen(username) > MAX_USERNAME_LEN)
		return RESULT_USER_REGISTER_USERNAME_TOO_LONG;

	if (strlen(password) > MAX_PASSWORD_LEN)
		return RESULT_USER_REGISTER_PASSWORD_TOO_LONG;

	if (strlen(password) < MIN_PASSWORD_LEN)
		return RESULT_USER_REGISTER_PASSWORD_TOO_SHORT;

	if (db_account_exists(username))
		return RESULT_USER_REGISTER_USERNAME_EXISTS;

	uint16_t buf_len = 1024;
	char query[buf_len];

	uint8_t username_len = strlen(username)+1;
	uint8_t password_len = strlen(password)+1;
	uint16_t email_len = strlen(email)+1;

	char e_username[username_len*2+1];
	char e_password[password_len*2+1];
	char e_email[email_len*2+1];

	mysql_real_escape_string(d_b->con, e_username, username, username_len - 1);
	mysql_real_escape_string(d_b->con, e_password, password, password_len - 1);
	mysql_real_escape_string(d_b->con, e_email, email, email_len - 1);

	snprintf(
			query, 
			buf_len,
			"INSERT INTO accounts(username, password, email, registration_ip) \
			VALUES ('%s', '%u', '%s', '%u')",
			e_username,
			hash_string(e_password),
			e_email,
			ip);

	if (mysql_query(d_b->con, query) != 0)
	{
		log_error("Register user failed for: %s. %s", username, mysql_error(d_b->con));
		return RESULT_ERROR_DATABASE;
	}
	
	log_trace("User registered: %s", username);

	return RESULT_USER_REGISTER_SUCCESS;
}

uint16_t
db_account_login(struct client *cl, char *username, const char *password)
{

	username = trim_white_space(username);

	if (!db_account_exists(username))
	{
		return RESULT_USER_LOGIN_USERNAME_DOESNT_EXIST;
	}
	
	uint16_t buf_size = 1024;		
	char query[buf_size];

	uint8_t username_len = strlen(username)+1;
	uint8_t password_len = strlen(password)+1;

	char e_username[username_len*2+1];
	char e_password[password_len*2+1];

	mysql_real_escape_string(d_b->con, e_username, username, username_len - 1);
	mysql_real_escape_string(d_b->con, e_password, password, password_len - 1);


	uint32_t h_password = hash_string(e_password);	

	snprintf(
			query, 
			buf_size, 
			"SELECT * FROM accounts WHERE username = '%s' AND password = '%u'", 
			e_username, 
			h_password);

	if (mysql_query(d_b->con, query) != 0) {
		log_error(
				"Failed to run query for logging user in: %s. %s", 
				username, 
				mysql_error(d_b->con));
		return RESULT_ERROR_DATABASE;
	}
	
	MYSQL_RES *res = mysql_store_result(d_b->con);
	
	if (res == NULL)
	{
		log_error("Failed to get stored result for checking if user %s exists. %s", 
				e_username, 
				mysql_error(d_b->con));
		return RESULT_ERROR_DATABASE;
	}

	MYSQL_ROW row = mysql_fetch_row(res);

	mysql_free_result(res);

	if (row == NULL)
	{
		log_error("User: %s tried logging in with invalid password.", username);
		return RESULT_USER_LOGIN_INVALID_PASSWORD;
	}	

	struct client_info c_info;

	c_info.id_account = atoi(row[0]);
	strcpy(c_info.username, row[1]);
	strcpy(c_info.email, row[2]);
	c_info.name_color.c = atoi(row[6]);
	c_info.logged_in = true;

	cl->info = c_info;

	log_trace("User logged in!: %u %s", c_info.id_account, c_info.username);

	return RESULT_USER_LOGIN_SUCCESS;
}

struct friend_list
db_account_friend_list_get(uint32_t db_id, int8_t *result)
{
	uint16_t buf_size = 1024;		
	char query[buf_size];

	snprintf(
			query, 
			buf_size, 
			"SELECT * FROM friends WHERE id_friend = '%u' or id_friend_other = '%u'", 
			db_id, 
			db_id);

	if (mysql_query(d_b->con, query) != 0) {
		log_error(
				"Failed to run query for user friends: %u. %s", 
				db_id, 
				mysql_error(d_b->con));
		*result = -1;
	}
	
	MYSQL_RES *res = mysql_store_result(d_b->con);
	
	if (res == NULL)
	{
		log_trace("Failed to get stored result for friends list:db_id: %u", db_id);
		*result = -1;
	}

	MYSQL_ROW row;


	struct friend_list list = friend_list_init();
	uint16_t count = 0;

	while((row = mysql_fetch_row(res)))
	{
		uint32_t id_friend = atoi(row[1]);
		uint32_t id_friend_other = atoi(row[2]);
		
		uint32_t id; 
		char username[MAX_USERNAME_LEN+1];

		if (id_friend == db_id)
		{
			id = id_friend_other;
			strcpy(username, row[4]);
		}
		else 
		{
			id = id_friend;
			strcpy(username, row[3]);
		}

		struct friend fr = friend_init(id, username, false);

		friend_list_add(&list, fr);

		log_trace("Got friend %s", username);

		count++;
	}

	if (row == NULL) *result = -1; // no friends :(

	mysql_free_result(res);

	*result = 1;
	
	list.friend_count = count;

	return list;

}


