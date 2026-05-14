CREATE TABLE login_history (
	user_name		varchar NOT NULL,
	ip				varchar,
	pid				int,
	session_key		text,
	invalid			boolean,
	time			bigint
);