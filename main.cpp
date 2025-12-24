#include "Sorter.hpp"
#include <unistd.h>
#include <sys/stat.h>
#include <csignal>
#include <atomic>
#include <cstdlib>    // Required for getenv()
#include <filesystem> // Required for creating log directories

namespace fs = std::filesystem;

// --- Global Flags & Signal Handling ---
volatile std::atomic<bool> is_running(true);

void handle_signal(int signal) {
    if (signal == SIGTERM) is_running = false;
}

// ... (Keep your daemonize function here) ...
void daemonize() {
    // ... same as before ...
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);
    if (setsid() < 0) exit(EXIT_FAILURE);
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);
    umask(0);
    chdir("/");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main() {
    // 1. Daemonize first
    daemonize();

    // 2. Setup Signal Handler
    signal(SIGTERM, handle_signal);

    // 3. BEST PRACTICE: Dynamically find paths
    // This ensures it works even if you change your username later
    const char* homeEnv = std::getenv("HOME");
    if (!homeEnv) return 1; // Should never happen on a normal Mac

    std::string homeDir(homeEnv);
    
    // Path 1: Source (Your Downloads)
    std::string downloadPath = homeDir + "/Downloads";

    // Path 2: Logs (Standard macOS User Log Location)
    // Location: /Users/harshdalwadi/Library/Logs/DownloadSorter/
    std::string logDir = homeDir + "/Library/Logs/DownloadSorter";
    std::string logFilePath = logDir + "/download_sorter.log";

    // 4. Create the Log Directory if it doesn't exist
    // If we don't do this, the logger will fail to open the file
    if (!fs::exists(logDir)) {
        try {
            fs::create_directories(logDir);
        } catch (...) {
            return 1; // Exit if we can't create the log folder
        }
    }

    // 5. Initialize Sorter
    Sorter sorter(downloadPath, logFilePath);

    // 6. The Loop
    while (is_running) {
        sorter.run_once();
        sleep(60); 
    }

    return 0;
}