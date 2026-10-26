#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "ran/Simulator.hpp"

namespace {

void printUsage(const char* prog) {
    std::cout << "Usage: " << prog << " [options]\n"
              << "  --ues N         number of UEs (default 10)\n"
              << "  --rbs N         resource blocks in the grid (default 50)\n"
              << "  --tti N         number of TTIs to simulate (default 1000)\n"
              << "  --seed N        RNG seed (default 42)\n"
              << "  --scheduler S   rr | maxci | pf (default pf)\n"
              << "  --help          show this help\n";
}

/// Parse the value following a flag; throws on missing/invalid input.
long parseLong(std::string_view flag, int& i, int argc, char** argv) {
    if (++i >= argc) {
        throw std::invalid_argument("missing value for " + std::string(flag));
    }
    return std::stol(argv[i]);
}

}  // namespace

int main(int argc, char** argv) {
    ran::SimConfig cfg;
    try {
        for (int i = 1; i < argc; ++i) {
            const std::string_view arg = argv[i];
            if (arg == "--help" || arg == "-h") {
                printUsage(argv[0]);
                return EXIT_SUCCESS;
            } else if (arg == "--ues") {
                cfg.numUes = static_cast<int>(parseLong(arg, i, argc, argv));
            } else if (arg == "--rbs") {
                cfg.resourceBlocks = static_cast<int>(parseLong(arg, i, argc, argv));
            } else if (arg == "--tti") {
                cfg.numTtis = static_cast<ran::Tti>(parseLong(arg, i, argc, argv));
            } else if (arg == "--seed") {
                cfg.seed = static_cast<unsigned>(parseLong(arg, i, argc, argv));
            } else if (arg == "--scheduler") {
                if (++i >= argc) {
                    throw std::invalid_argument("missing value for --scheduler");
                }
                cfg.scheduler = ran::mac::parseSchedulerType(argv[i]);
            } else {
                throw std::invalid_argument("unknown argument: " + std::string(arg));
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << "\n\n";
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    ran::Simulator sim(cfg);
    const ran::SimResult r = sim.run();

    auto scheduler = ran::mac::makeScheduler(cfg.scheduler);
    std::cout << "Scheduler: " << scheduler->name() << " | UEs: " << cfg.numUes
              << " | RBs: " << cfg.resourceBlocks << " | TTIs: " << cfg.numTtis << "\n";
    std::cout << "------------------------------------------------------------\n";
    std::cout.setf(std::ios::fixed);
    std::cout.precision(2);
    std::cout << "Cell throughput : " << r.cellThroughputMbps << " Mbps\n";
    std::cout << "Jain's fairness : " << r.jainsFairness << "\n";
    std::cout << "HARQ retx rate  : " << (r.harqRetxRate * 100.0) << " %\n";
    std::cout << "Per-UE throughput (Mbps):";
    for (const double t : r.perUeThroughputMbps) {
        std::cout << ' ' << t;
    }
    std::cout << '\n';
    return EXIT_SUCCESS;
}
