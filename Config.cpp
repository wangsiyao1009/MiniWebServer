#include "Config.h"

Config::Config() = default;

Config::~Config() = default;

const std::string Config::webRoot = "./www";
unsigned short Config::threadPoolNumWorkers = 5;
uint16_t Config::webServerPort = 8086;

void Config::parseArgs(int argc, char **argv) {
    static struct option longOptions[] = {
            {"port",      required_argument, nullptr, 'p'},
            {"numWorker", required_argument, nullptr, 'n'}
    };
    int optionIndex = 0;
    int opt;
    while ((opt = getopt_long(argc, argv, "p:n:", longOptions, &optionIndex)) != -1) {
        switch (opt) {
            case 'p':
                webServerPort = static_cast<uint16_t>(strtol(optarg, nullptr, 10));
                break;
            case 'n':
                threadPoolNumWorkers = static_cast<unsigned short>(strtol(optarg, nullptr, 10));
                break;
            default:
                break;
        }
    }
}
