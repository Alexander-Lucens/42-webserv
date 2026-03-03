FROM debian:bookworm-slim AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential clang cmake git valgrind gdb \
    curl wget telnet netcat-openbsd net-tools \
    python3 php-cgi procps lsof vim clang-format \
    clang-tidy cppcheck siege nano \
  && rm -rf /var/lib/apt/lists/*

RUN useradd -m -u 1000 -s /bin/bash student
WORKDIR /home/student/project
RUN chown -R student:student /home/student/project

COPY . .
RUN chown -R student:student /home/student/project


FROM debian:bookworm-slim AS runner

RUN apt-get update && apt-get install -y \
    python3 \
    php-cgi \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /home/student/project/webserv /app/webserv
COPY --from=builder /home/student/project/www /app/www

CMD ["./webserv", "www/nginx.conf"]