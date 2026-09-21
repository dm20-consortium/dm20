CREATE TABLE user_login (
	user_name		varchar NOT NULL, 
	hash			text NOT NULL, 
	PRIMARY KEY(user_name)
);