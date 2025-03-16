import subprocess
import yaml
import os
import signal
import sys

config_file_path = "../config.yaml"

# Load the config file
with open(config_file_path, "r") as config_file:
    config = yaml.safe_load(config_file)

backends = config.get("backends", [])

# List to keep track of the processes
backend_processes = []

def start_backend(ip, port):
    backend_command = [
        "python3", "../backend/backend.py", 
        str(port)
    ]
    # Start the backend server as a subprocess and store the process object
    process = subprocess.Popen(backend_command)
    backend_processes.append(process)
    print(f"Started backend server on {ip}:{port}")
    
def stop_backends():
    """Stop all running backend processes."""
    for process in backend_processes:
        try:
            process.terminate()  # Try to terminate the process gracefully
            process.wait()  # Wait for the process to terminate
            print(f"Stopped backend server on {process.args[-1]}")
        except Exception as e:
            print(f"Error stopping process {process.args[-1]}: {e}")

# Starting backends
for backend in backends:
    ip = backend["ip"]
    port = backend["port"]
    start_backend(ip, port)

# A simple signal handler for stopping the servers gracefully
def signal_handler(sig, frame):
    print("\nStopping all backends...")
    stop_backends()
    sys.exit(0)

# Register the signal handler to catch Ctrl+C (SIGINT)
signal.signal(signal.SIGINT, signal_handler)

# Wait indefinitely (press Ctrl+C to stop)
print("Press Ctrl+C to stop the backends...")
signal.pause()
