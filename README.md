# C REST Web Application

**🚧 IN DEVELOPMENT 🚧**

## Overview

This project is being developed as a fun experiment in building a RESTful web application using C.

## Features

- 🚀 **Fast** (compare?)
- ✨ **lightweight** (compare?)
- Simple **Router** with middleware 
- **PostgreSQL** client
- **Asynchronous** I/O with libuv
- **Feature-Based** logging
- **Tests** with cmocka
- Benchmarks, Docker scratch image, gihub actions ..
 

## Dependencies

- **HTTP Parser**: [picohttpparser](https://github.com/h2o/picohttpparser)
- **Sockets**: [libuv](https://libuv.org/)
- **PostgreSQL**: [libpq](https://www.postgresql.org/docs/current/libpq.html)
- **JSON Parsing**: [parson](https://github.com/kgabis/parson)
- **Unit Testing** : [cmocka](https://cmocka.org)

## Installation

```sh
# MacOS
brew install libuv libpq cmocka  

# Ubuntu
apt install pkg-config libpq-dev libuv1-dev libcmocka-dev

# Windows Support?
# Most likely not, I don’t have a Windows machine, but technically it’s possible.

```


### Submodules

```bash
# Initialize the submodules (if not done already)
git submodule update --init --recursive
```

## Build and Run

```sh
make
make clean
make test
make run
```

## Benchmarking

To benchmark the server, you can use tools like **wrk** and **ab**:

### Install Tools

```bash
# MacOS
brew install wrk ab
```

### Run Benchmark

```bash
wrk -t4 -c100 -d30s http://127.0.0.1:8080
ab -n 10000 -c 100 http://127.0.0.1:8080/
```

## Developemnt 

```sh
# Generate `compile_commands.json`
make .vscode/compile_commands.json

# Format code
make format

# Run tests in docker
make test-docker
make test-docker-linux-amd64
make test-docker-linux-arm64
```


## Contributions

Feel free to give it a star, share your ideas, or open an issue! But please, no pull requests, as this is an educational project and will likely be archived once it's completed.
