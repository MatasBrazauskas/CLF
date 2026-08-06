#include "InputHandler.hpp"

#include <argp.h>
#include <string>

Inputs::Inputs(): fileName{}, n{} {}

static char doc[] = "V1";
static char args_doc[] = "[ARG1...]";

static argp_option options[] = {
    {"input", 'f', "FILE", 0, "Input file path", 0},
    {"number", 'n', "N", 0, "Number of requested top statistics information", 0},
    {"reserve", 'r', "RESERVE", 0, "Reserve the hash map space", 0},
    {"maps", 'm', "MAPS", 0, "Amount of hash maps used as buckets", 0},
    {0, 0, 0, 0, 0, 0}
};

static error_t parse_opt(int key, char *arg, argp_state *state) {
    Inputs *input = static_cast<Inputs *>(state->input);

    switch (key) {
        case 'f':
            if (arg) {
                input->fileName = arg;
            }
            break;
        case 'n':
            if (arg) {
                input->n = std::stoi(arg);
            }
            break;
        case 'r':
            if (arg) {
                input->reserveCount = std::stoi(arg);
            }
            break;
        case 'm':
            if (arg) {
                input->maps = std::stoi(arg);
            }
            break;
        case ARGP_KEY_END:
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static argp argp = {options, parse_opt, args_doc, doc};

std::optional<Inputs> validateInputs(const int argc, char **argv) {
    Inputs inputs{};

    if (argp_parse(&argp, argc, argv, 0, nullptr, &inputs) != 0) {
        return std::nullopt;
    }

    if (inputs.fileName.empty() or inputs.n == 0) {
        return std::nullopt;
    }

    if (inputs.reserveCount <= 0) {
        inputs.reserveCount = 5000;
    }

    return inputs;
}