#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
//erevum em?
volatile sig_atomic_t reply_received = 0;

void spionen_handler(int sig, siginfo_t* info, void* context) {
    if (sig == SIGUSR1 && info != nullptr) {
        kill(info->si_pid, SIGUSR2);
    }
}

void sturmbann_handler(int sig, siginfo_t* info, void* context) {
    if (sig == SIGUSR2) {
        reply_received = 1;
    }
}

void run_spionen() {
    struct sigaction sa;
    sa.sa_sigaction = spionen_handler;
    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, nullptr);

    std::cout << "[SPIONEN] Ready. Waiting for SIGUSR1 to reply with SIGUSR2..." << std::endl;
    while (true) {
        pause();  
    }
}

void run_sturmbann() {
    struct sigaction sa;
    sa.sa_sigaction = sturmbann_handler;
    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR2, &sa, nullptr);

    std::vector<pid_t> candidates;
    std::ifstream file("/tmp/reichspost.txt");
    if (!file.is_open()) {
        std::cerr << "[STURMBANNFUHRER] Error: cannot open /tmp/reichspost.txt" << std::endl;
        exit(1);
    }
    pid_t pid_val;
    while (file >> pid_val) {
        candidates.push_back(pid_val);
    }
    file.close();

    std::cout << "[STURMBANNFUHRER] Training started. Scanning " << candidates.size()
              << " pseudo-PIDs to identify the spionen..." << std::endl;

    for (auto candidate : candidates) {
        reply_received = 0;  
        if (kill(candidate, SIGUSR1) == 0) {
            usleep(500000);  

            if (reply_received) {
                std::cout << "[STURMBANNFUHRER] Spionen identified: PID " << candidate << std::endl;
                if (kill(candidate, SIGILL) == 0) {
                    std::cout << "[STURMBANNFUHRER] Spionen successfully vernichtet with SIGILL!" << std::endl;
                } else {
                    std::cerr << "[STURMBANNFUHRER] Failed to send SIGILL" << std::endl;
                }
                return; 
            }
        }
    }

    std::cerr << "[STURMBANNFUHRER] No spionen found. Training failed." << std::endl;
    exit(1);
}

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<pid_t> dis(10000, 999999);

    std::cout << "=== Gestapo Signal Training Mission Started ===" << std::endl;

    std::vector<pid_t> fake_pids;
    for (int i = 0; i < 8; ++i) {
        fake_pids.push_back(dis(gen));
    }

    pid_t spionen_pid = fork();
    if (spionen_pid == 0) {
        run_spionen();  
        return 0;
    } else if (spionen_pid > 0) {
        std::cout << "[MAIN] Forked real spionen with PID: " << spionen_pid << std::endl;
    } else {
        std::cerr << "[MAIN] Failed to fork spionen" << std::endl;
        return 1;
    }

    std::vector<pid_t> all_pids = fake_pids;
    all_pids.push_back(spionen_pid);

    std::shuffle(all_pids.begin(), all_pids.end(), gen);

    std::ofstream outfile("/tmp/reichspost.txt");
    if (!outfile.is_open()) {
        std::cerr << "[MAIN] Failed to create /tmp/reichspost.txt" << std::endl;
        return 1;
    }
    for (size_t i = 0; i < all_pids.size(); ++i) {
        outfile << all_pids[i];
        if (i < all_pids.size() - 1) outfile << " ";
    }
    outfile << std::endl;
    outfile.close();

    std::cout << "[MAIN] Created /tmp/reichspost.txt with shuffled pseudo-PIDs + real spionen PID" << std::endl;

    pid_t sturmbann_pid = fork();
    if (sturmbann_pid == 0) {
        run_sturmbann(); 
        return 0;
    } else if (sturmbann_pid > 0) {
        std::cout << "[MAIN] Forked Sturmbannfuhrer with PID: " << sturmbann_pid << std::endl;
    } else {
        std::cerr << "[MAIN] Failed to fork Sturmbannfuhrer" << std::endl;
        return 1;
    }

    waitpid(spionen_pid, nullptr, 0);
    waitpid(sturmbann_pid, nullptr, 0);

    std::cout << "=== Mission Complete: Sturmbannfuhrer training finished and deployed ===" << std::endl;
    std::cout << "You may now run the Sturmbannfuhrer binary in future operations." << std::endl;

    return 0;
}
