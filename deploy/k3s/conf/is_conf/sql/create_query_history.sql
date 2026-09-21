CREATE TABLE query_history (
	ts				bigint NOT NULL, 
	action			varchar NOT NULL, 
	pid				varchar NOT NULL,
	mng_id			int, 
	socket_id		int, 
	destination		varchar NOT NULL,
	query			text,	
	action_user		varchar NOT NULL, 	
	memo			text		
);
