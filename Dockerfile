FROM debian:bookworm-slim

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    cmake \
    git \
    valgrind \
    gdb \
    curl \
    wget \
    telnet \
    netcat-openbsd \
    siege \
    net-tools \
    python3 \
    php-cgi \
    procps \
    lsof \
    vim \
    nano \
    && rm -rf /var/lib/apt/lists/*

RUN useradd -m -u 1000 -s /bin/bash student

WORKDIR /home/student/project

RUN chown -R student:student /home/student/project

USER student

CMD ["bash"]