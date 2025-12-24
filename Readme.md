# C++ Download Sorter Daemon

A robust, background service (daemon) written in C++ that automates file organization. It monitors a source directory (e.g., `~/Downloads`) and instantly moves files into categorized subfolders based on their extensions.

This project demonstrates advanced Unix programming concepts, specifically the **Double Fork Technique** for creating a true, detached daemon process.

---

## 🚀 Features

* **Automated Organization:** Automatically sorts files into `Images`, `Videos`, `Documents`, `Music`, and `Archives`.
* **True Daemonization:**
    * Detaches from the controlling terminal (no `TTY`).
    * Runs in its own independent Session and Process Group.
    * **Double Fork** implementation prevents the process from ever re-acquiring a terminal.
* **Resource Safety:**
    * Changes working directory to root (`/`) to avoid locking file systems.
    * Sets file mode creation mask (`umask 0`) for permission flexibility.
    * Closes standard file descriptors (`stdin`, `stdout`, `stderr`) to prevent I/O hanging.
* **System Logging:** specific daemon events and errors are logged to `syslog` (viewable via `journalctl`).

---

## 🛠️ Technical Implementation: The Double Fork

This daemon uses the "Double Fork" strategy to ensure robust detachment from the user session.

1.  **First Fork:** The parent process forks and exits. The child continues, detached from the terminal prompt but still inside the original session.
2.  **`setsid()`:** The child creates a new Session and becomes the **Session Leader**.
3.  **Second Fork:** The Session Leader forks a grandchild and exits.
4.  **Result:** The grandchild becomes the active daemon. Since it is **not** a Session Leader, the kernel prevents it from ever acquiring a controlling terminal, even if it opens a specific device file.

---

## 📋 Prerequisites

* **Operating System:** Linux / Unix-like OS
* **Compiler:** `g++` (GCC) or `clang`
* **Standard:** C++17 or newer (required for `<filesystem>`)

---

## ⚙️ Installation & Build

1.  **Clone the repository** (if applicable) or create your directory.

2.  **Compile the source code:**
    Use the following command to build the daemon. We use C++17 for the `std::filesystem` library.

    ```bash
    g++ -std=c++17 -o sorter_daemon main.cpp
    ```

    *Note: If you are on an older version of GCC (version < 9), you may need to link the library manually:*
    ```bash
    g++ -std=c++17 -o sorter_daemon main.cpp -lstdc++fs
    ```

---

## 🔧 Configuration

By default, the daemon is configured in `main.cpp`. You can modify the following constants in the source code before compiling:

* **`WATCH_DIR`**: The directory to monitor (default: `~/Downloads`).
* **`SLEEP_INTERVAL`**: How often the daemon checks for new files (in seconds).

### Default Extension Mapping
| Category | Extensions |
| :--- | :--- |
| **Images** | `.jpg`, `.jpeg`, `.png`, `.gif`, `.bmp` |
| **Videos** | `.mp4`, `.mkv`, `.avi`, `.mov` |
| **Documents** | `.pdf`, `.doc`, `.docx`, `.txt`, `.xlsx` |
| **Archives** | `.zip`, `.tar`, `.gz`, `.7z` |

---

## 🖥️ Usage

### 1. Start the Daemon
Run the executable. It will immediately return control to your terminal as the process forks into the background.

```bash
./sorter_daemon