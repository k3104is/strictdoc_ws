#!/usr/bin/env bash
set -e

USER_NAME=dev
GROUP_NAME=dev

HOST_UID=${HOST_UID:-1000}
HOST_GID=${HOST_GID:-1000}

# Group作成
if ! getent group "${HOST_GID}" >/dev/null 2>&1; then
    groupadd -g "${HOST_GID}" "${GROUP_NAME}"
else
    GROUP_NAME=$(getent group "${HOST_GID}" | cut -d: -f1)
fi

# User作成
if ! getent passwd "${HOST_UID}" >/dev/null 2>&1; then
    useradd \
        -m \
        -u "${HOST_UID}" \
        -g "${HOST_GID}" \
        -s /bin/bash \
        "${USER_NAME}"
else
    USER_NAME=$(getent passwd "${HOST_UID}" | cut -d: -f1)
fi

USER_HOME=$(getent passwd "${HOST_UID}" | cut -d: -f6)

# sudo権限を付与
usermod -aG sudo "${USER_NAME}"

echo "${USER_NAME} ALL=(ALL) NOPASSWD:ALL" \
    > "/etc/sudoers.d/${USER_NAME}"

chmod 0440 "/etc/sudoers.d/${USER_NAME}"

export HOME="${USER_HOME}"

exec gosu "${USER_NAME}" "$@"