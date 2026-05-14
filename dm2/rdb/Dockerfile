from postgres:12

run apt-get update \
  && apt-get install -y \
    postgis \
    postgresql-12-postgis-3 \
  && apt-get clean \
  && rm -rf /var/lib/apt/lists/*

copy init.sh /docker-entrypoint-initdb.d/
