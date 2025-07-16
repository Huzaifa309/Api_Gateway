# 🚀 API Gateway using Drogon & Aeron

This project is a multi-threaded API Gateway built in **C++** using the **Drogon HTTP framework** and **Aeron messaging system**. It demonstrates how to receive JSON HTTP requests, process them in the background using message queues, and optionally forward/receive them via Aeron.

---

## 🧩 Architecture Overview

### Threads:
- **T1** – Receives HTTP POST `/api/data` (via Drogon), logs and pushes to `ReceiverQueue`.
- **T2** – Pulls JSON from `ReceiverQueue`, and pushes to Aeron (currently echoes).
- **T3** – Receives data from Aeron subscription, pushes result to `ResponseQueue`.
- **T4** – Dispatches final response from `ResponseQueue` back to the original HTTP client.

---

## 📦 Features

✅ Drogon HTTP server  
✅ Multi-threaded processing using `moodycamel::ConcurrentQueue`  
✅ Basic Aeron publication/subscription  
✅ JSON parsing and logging  
✅ Graceful Aeron initialization  
✅ Placeholder for future SBE encoding/decoding

---

## 🔧 Dependencies

- C++17
- [Drogon](https://github.com/drogonframework/drogon)
- [Aeron](https://github.com/real-logic/aeron)
- [nlohmann/json](https://github.com/nlohmann/json)
- [concurrentqueue](https://github.com/cameron314/concurrentqueue)

---

## 🛠️ Building the Project

### 1. Clone the Repository
```bash
git clone https://github.com/YOUR_USERNAME/api_gateway.git
cd api_gateway


2.	Build Aeron (if not already)
bash
Copy
Edit
git submodule update --init --recursive
cd external/aeron
./gradlew cpp:build

3. Build the API Gateway
bash
Copy
Edit
cd ../..  # back to root of project
mkdir build && cd build
cmake ..
make -j$(nproc)

4. Run
bash
Copy
Edit
./api_gateway


📁 Project Structure
bash
Copy
Edit
.
├── CMakeLists.txt
├── README.md
├── external/
│   ├── aeron/                # Aeron C++ client
│   └── concurrentqueue/      # moodycamel queue
├── src/
│   ├── main.cpp
│   ├── Logger.*              # Thread-safe logger
│   ├── RequestHandler.*      # T1 - Drogon HTTP handler
│   ├── QueueManager.*        # Shared queues
│   ├── JsonToSbeSender.*     # T2 - Placeholder SBE sender
│   ├── AeronReceiver.*       # T3 - Aeron subscriber
│   ├── ResponseDispatcher.*  # T4 - Sends response

