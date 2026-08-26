COMPOSE_DIR:="./strictdoc/compose.yaml"
ENV_VAR:="HOST_UID=$(id -u) HOST_GID=$(id -g)"

hello:
  echo "hello"

echo:
  echo {{COMPOSE_DIR}}

init:
  git clone https://github.com/strictdoc-project/strictdoc.git
  ln -s $(pwd)/compose.yaml ./strictdoc

up:
  {{ENV_VAR}} docker compose -f {{COMPOSE_DIR}} up -d

run:
  {{ENV_VAR}} docker compose -f {{COMPOSE_DIR}} run --rm strictdoc /bin/bash

exec:
  {{ENV_VAR}} docker compose -f {{COMPOSE_DIR}} exec strictdoc /bin/bash

down:
  {{ENV_VAR}} docker compose -f {{COMPOSE_DIR}} down

rm:
  {{ENV_VAR}} docker compose -f {{COMPOSE_DIR}} down --rmi all --volumes --remove-orphans