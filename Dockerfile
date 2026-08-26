FROM ubuntu:24.04 AS cppdev

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    curl \
    ca-certificates \
    gdb \
    vim \
    sudo \
    gosu \
    wget \
    && rm -rf /var/lib/apt/lists/*

COPY entrypoint.sh /usr/local/bin/entrypoint.sh

RUN chmod +x /usr/local/bin/entrypoint.sh

WORKDIR /workdir

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
CMD ["bash"]

FROM cppdev AS strictdoc_cppdev

RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    python3-venv \
    && rm -rf /var/lib/apt/lists/*

RUN python3 -m venv /opt/venv

ENV PATH="/opt/venv/bin:$PATH"

RUN pip install --no-cache-dir --upgrade pip \
    && pip install --no-cache-dir strictdoc

WORKDIR /workdir

FROM strictdoc_cppdev AS codex_strictdoc_cppdev

RUN apt-get update && apt-get install -y \
    nodejs \
    npm \
    && rm -rf /var/lib/apt/lists/*

RUN npm install -g @openai/codex

WORKDIR /workdir
