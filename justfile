# COMPOSE_DIR:="./strictdoc/compose.yaml"
COMPOSE_DIR:="./compose.yaml"
ENV_VAR:="HOST_UID=$(id -u) HOST_GID=$(id -g)"
SVC:="codex_strictdoc_cppdev"

hello:
  echo "hello"

echo:
  echo {{COMPOSE_DIR}}

init:
  git clone https://github.com/strictdoc-project/strictdoc.git
  ln -s $(pwd)/compose.yaml ./strictdoc

up:
  {{ENV_VAR}} docker compose -f {{COMPOSE_DIR}} up -d {{SVC}}

run:
  {{ENV_VAR}} docker compose -f {{COMPOSE_DIR}} run --rm {{SVC}} /bin/bash

exec:
  {{ENV_VAR}} docker compose -f {{COMPOSE_DIR}} exec {{SVC}} /bin/bash

down:
  {{ENV_VAR}} docker compose -f {{COMPOSE_DIR}} down

rm:
  {{ENV_VAR}} docker compose -f {{COMPOSE_DIR}} down --rmi all --volumes --remove-orphans