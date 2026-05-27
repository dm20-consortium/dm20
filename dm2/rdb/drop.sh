#!/bin/bash

set -ex

psql -v ON_ERROR_STOP=1 --username "${POSTGRES_USER:-postgres}" <<EOSQL
DROP DATABASE dm2_is_db;
DROP DATABASE dm2_db;
DROP USER dm2;
EOSQL
