# EchelonLB


[![C++ Version](https://img.shields.io/badge/C++-17%2B-brightgreen.svg)](https://en.cppreference.com/w/cpp/compiler_support)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

**EchelonLB** is a high-performance load balancer written in **C++**, designed to efficiently distribute traffic across multiple backend servers. 

---

## Features

- **Load Balancing Algorithm**:
  - Round Robin: Distributes requests evenly across all backend servers.
- **Simple Configuration**: Easy-to-use YAML configuration file.
- **High Performance**: Built with C++ for maximum efficiency.

---

## Installation

### Prerequisites
- **C++ Compiler** (supporting C++17 or higher)
- **CMake** (for building the C++ component)

### Steps
1. Clone the repository:
   ```bash
   git clone https://github.com/YBDYY/EchelonLB.git
   cd EchelonLB
2. Build the load balancer:
    ```bash
    mkdir build 
    cd build
    cmake ..
    make
3. Run the load balancer:
    ```bash
    ./tcp_proxy
## Usage

### Configuration
EchelonLB uses a YAML configuration file (`config.yaml`) located in the root directory of the project. Here's an example of its structure:

```yaml
server:
  listen_port: 8080

backends:
  - ip: "127.0.0.1"
    port: 9000
  - ip: "127.0.0.1"
    port: 9001
  - ip: "127.0.0.1"
    port: 9002
```


## Backend Management

### Starting Multiple Backends
The `scripts/start_backends.py` Python script is provided to start multiple backend servers simultaneously. This is useful for testing and development.

#### Usage
1. Navigate to the `scripts/` directory:
   ```bash
   cd scripts/
2. Run the script:
    ```bash
    python3 start_backends.py
### Customization
You can modify the `start_backends.py` and `config.yaml` to start backend servers on different ports or with different configurations as needed.

## Roadmap

Planned features for the future:
- **Health Checks**: Monitor backend servers for availability.
- **Weighted Round Robin**: Assign weights to servers based on capacity.
- **Least Connections**: Route traffic to the server with the fewest active connections.
- **TLS Support**: Secure communication with HTTPS.
- **Metrics Collection**: Gather and display performance metrics (e.g., request rate, latency).
- **Dynamic Configuration**: Allow configuration changes without restarting the load balancer.
