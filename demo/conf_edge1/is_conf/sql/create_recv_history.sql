CREATE TABLE recv_history (
	ts			bigint NOT NULL, 
	protocol		varchar NOT NULL, 
	local_ip		varchar NOT NULL, 
	local_port		int NOT NULL, 
	foreign_ip		varchar NOT NULL, 
	foreign_port1		int NOT NULL, 
	foreign_port2		int NOT NULL, 
	socket_id1		int NOT NULL, 
	socket_id2		int, 
	socket_action		varchar, 
	socket_result		int, 
	socket_result2		int, 
	recv_data		text,
	memo			text		
);
