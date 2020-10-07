#include "WebServer.h"

int main(int argc, char **argv) {
    Config::parseArgs(argc, argv);
    WebServer server(Config::webServerPort, Config::threadPoolNumWorkers,
                     Config::webRoot);
    server.eventLoop();
    return 0;
}
