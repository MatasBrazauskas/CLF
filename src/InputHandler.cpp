#include "InputHandler.hpp"

#include <argp.h>
#include <string>

Inputs::Inputs(): fileName{}, n{}, epsilon{0.00001f}, delta{0.001f} {}

static char doc[] = "V1";
static char args_doc[] = "[ARG1...]";

static argp_option options[] = {
    {"input", 'f', "FILE", 0, "Input file path", 0},
    {"number", 'n', "N", 0, "Number of requested top statistics information", 0},
    {"epsilon", 'e', "E", 0, "Epsilon value for the count min sketch", 0},
    {"delta", 'd', "D", 0, "Delta value for the count min sketch", 0},
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
        case 'e':
            if (arg) {
                input->epsilon = std::stof(arg);
            }
            break;
        case 'd':
            if (arg) {
                input->delta = std::stof(arg);
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

    if (inputs.fileName.empty() or inputs.n <= 0) {
        return std::nullopt;
    }

    if (inputs.epsilon <= 0.0f or inputs.epsilon >= 1.0f or inputs.delta <= 0.0f or inputs.delta >= 1.0f) {
        return std::nullopt;
    }

    return inputs;
}
