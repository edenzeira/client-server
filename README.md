# 🚨 SPL Assignment 3 – STOMP Emergency Messaging System

This project implements an emergency communication platform using the STOMP protocol over TCP.  
It includes a **Java-based server** and a **C++ multithreaded client**, and was developed as part of the SPL 251 course at Ben-Gurion University.

---

## 🧭 Overview

- Users can subscribe to emergency channels (e.g., `fire_dept`, `police`)
- They can send reports to these channels and receive live updates
- The system is built on a **text-based messaging protocol (STOMP 1.2)**
- Implements both **Thread-Per-Client** and **Reactor** server models

---

## 🧠 Components

### 🖥 Server (Java)
- Supports two modes:
  - `tpc` – thread-per-client
  - `reactor` – event-based model
- Implements:
  - `Connections<T>` – manages all connected clients
  - `ConnectionHandler<T>` – sends STOMP messages
  - `StompMessagingProtocol<T>` – processes client frames

### 💻 Client (C++)
- Multithreaded:
  - One thread for keyboard input
  - One thread for receiving messages
- Supports commands like:
  - `login`, `join`, `exit`, `report`, `summary`, `logout`
- Translates each command to STOMP frames (`CONNECT`, `SUBSCRIBE`, `SEND`, etc.)

---

## ⚙️ How to Run

### Server (Java)
```bash
mvn compile
mvn exec:java -Dexec.mainClass="bgu.spl.net.impl.stomp.StompServer" -Dexec.args="7777 tpc"
```

### Client (C++)
```bash
cd client
make
./bin/StompEMIClient
```

---

## 📝 Sample Commands (Client)
```bash
login 127.0.0.1:7777 eden secret
join fire_dept
report data/events1_partial.json
summary fire_dept eden output.txt
logout
```

---

## 📂 Project Structure

```
server/
├── src/
├── pom.xml
└── run instructions via Maven

client/
├── src/
├── include/
├── bin/
└── Makefile
```

---

## 🧪 Test Data

Test reports (in JSON format) are placed under `data/` and include emergency events like:
- `Grand Theft Auto` in `police`
- `Fire` in `fire_dept`

Each report includes:
- City, time, event name, description
- Flags: `active`, `forces_arrival_at_scene`

---

## ✅ Status

✔️ Full STOMP protocol support  
✔️ Java server supports TPC and Reactor  
✔️ C++ client supports all required commands  
✔️ Multithreading and synchronization working  
✔️ Cross-communication verified  

---

