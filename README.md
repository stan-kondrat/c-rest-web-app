# C REST Web Application

**🚧 IN DEVELOPMENT 🚧**

## Overview

This project is being developed as a fun experiment in building a RESTful web application using C.

## Dependencies

- **HTTP Parser**: [picohttpparser](https://github.com/h2o/picohttpparser)
- **Sockets**: [libuv](https://libuv.org/)
- **PostgreSQL**: [libpq](https://www.postgresql.org/docs/current/libpq.html)
- **JSON Parsing**: [parson](https://github.com/kgabis/parson)

## Installation

### MacOS

```bash
brew install libuv libpq
```

### Ubuntu

```bash
apt install libpq-dev libuv1-dev
```

### Submodules

```bash
# Initialize the submodules (if not done already)
git submodule update --init --recursive
```

## Build and Run

```bash
make
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

---

Feel free to give it a star, share your ideas, or open an issue!
