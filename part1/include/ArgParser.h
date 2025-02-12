#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <string>
#include <iostream>
#include <cstdlib> // for std::atoi and std::atof

struct CommandLineOptions {
    std::string filename;
    int timeout_ms;
    std::string getFilenameExtension() {
        size_t last_slash = filename.find_last_of("/\\");
        size_t last_dot = filename.find_last_of(".");

        std::string result;
        if (last_slash == std::string::npos) {
            result = filename;
        } else {
            result = filename.substr(last_slash + 1);
        }

        if (last_dot != std::string::npos && last_dot > last_slash) {
            result = filename.substr(last_slash + 1, last_dot-last_slash-1);
        }
        return result;
    }
};
CommandLineOptions parse_arguments(int argc, char* argv[]) {
    CommandLineOptions options;
    options.timeout_ms = -1;


    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-f" || arg == "--filename") {
            if (i + 1 < argc) {
                options.filename = argv[++i];
            } else {
                std::cerr << "Error: No filename provided after " << arg << std::endl;
                exit(1);
            }
        } else if (arg == "-t" || arg == "--timeout-ms") {
            if (i + 1 < argc) {
                options.timeout_ms = std::atoi(argv[++i]);
            } else {
                std::cerr << "Error: No timeout provided after " << arg << std::endl;
                exit(1);
            }
        } else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [options]\n"
                      << "Options:\n"
                      << "  -f, --filename [FILE]                Name of the input TSP instance file\n"
                      << "  -t, --timeout-ms [TIMEOUT]           Timeout expressed in milliseconds\n";
            exit(0);
        }
    }

    return options;
}


#endif // ARG_PARSER_H
