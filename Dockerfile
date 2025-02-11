FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    build-essential \
    make \
    libuv1-dev \
    libpq-dev \
    libcmocka-dev \
    && rm -rf /var/lib/apt/lists/*

COPY . /app
WORKDIR /app

CMD ["make", "clean", "test"]
