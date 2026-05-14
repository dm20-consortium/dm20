#!/bin/bash

set -ex

psql -v ON_ERROR_STOP=1 --username "${POSTGRES_USER:-postgres}" <<EOSQL
CREATE USER dm2 WITH PASSWORD '${RDB_DM2_PASS:-dm2rdbsamplepassword}' CREATEDB;
CREATE DATABASE dm2_db OWNER dm2;
CREATE DATABASE dm2_is_db OWNER dm2;
\connect dm2_db postgres
create extension postgis;
create extension postgis_topology;
alter user dm2 set search_path=dm2,public,topology;
\connect dm2_is_db postgres
CREATE TABLE user_login (user_name  varchar NOT NULL, hash   text NOT NULL, PRIMARY KEY(user_name));
CREATE TABLE login_history (user_name varchar NOT NULL, ip varchar, pid int, session_key text, invalid boolean, time bigint);
INSERT INTO user_login VALUES('${RDB_USER_NAME:-dm2sampleuser}', MD5('${RDB_USER_PASS:-dm2samplepassword}'));
ALTER TABLE user_login OWNER TO dm2;
ALTER TABLE login_history OWNER TO dm2;
EOSQL
