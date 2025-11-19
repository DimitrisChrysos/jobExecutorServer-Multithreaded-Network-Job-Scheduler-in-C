# 🌐 jobExecutorServer — Multithreaded Network Job Scheduler in C

A system-level multithreaded job scheduler in C using sockets, condition variables, and producer-consumer synchronization. It supports job submission, polling, concurrency control, and graceful termination via a client-server architecture.

---

## 🧾 Author Info

- **Name**: Δημήτριος Χρυσός  
- **Student ID**: 1115202100275  

---

### Note:
- Check report.txt and hw2-spring-2024.pdf for a more detailed explanation.

---

## 🧩 Project Summary

This project includes:
- A multithreaded **jobExecutorServer** that listens for TCP connections from clients and manages job execution.
- A **jobCommander** client that submits jobs and commands to the server.
- A thread pool of workers consuming jobs from a synchronized buffer.
- Full support for commands like `issueJob`, `poll`, `setConcurrency`, `stop`, and `exit`.

---

## 🛠 Compilation & Execution

### Build
```bash
make
```

### Run the Server
```bash
./bin/jobExecutorServer [portnum] [bufferSize] [threadPoolSize]
```

### Run the Client
```bash
./bin/jobCommander [serverName] [portNum] [jobCommand]
```

> ℹ️ See `Makefile` comments for example execution scripts.

---

## 🗃️ File Structure

- `src/jobExecutorServer.c`: Manages sockets, thread pool, and server logic.
- `src/jobCommander.c`: Parses arguments, connects to server, and prints results.
- `src/queue.c/h`: Thread-safe job buffer based on custom Queue & Triplet structs.
- `src/ServerCommands.c/h`: Core logic for all server commands:
  - `issueJob`, `stop_job`, `poll`, `exit_server`, `setConcurrency`
  - `execute_job()` for workers
  - `call_commands()` for controllers
- `Makefile`: Automates build and clean process.

---

## 🧵 Threading Design

- **Main thread**: Accepts incoming connections.
- **Controller threads**: Spawned per client, parse commands, push jobs to buffer.
- **Worker threads**: Constantly wait on condition variable; pick jobs if buffer is non-empty and concurrency level allows.

All shared resources (buffer, concurrency) are protected by mutexes and condition variables.

---

## 🧪 Testing & Notes

- Configure `tests/server_host.sh` with `SERVER_HOST` and `PORT`.
- Run tests via:
  ```bash
  bash tests/[script.sh]
  ```
- You can also run individual commands manually.

---

## ✅ Highlights

- Fully modular and separated codebase.
- Dynamic concurrency adjustment (`setConcurrency`).
- Synchronized socket-based communication.
- Clean output handling using `dup2()` and temporary `.output` files.

---

Course: **Systems Programming (Κ24 — Προγραμματισμός Συστήματος)**  
Assignment: **Εργασία 2 — Εαρινό Εξάμηνο 2024**
