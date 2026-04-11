#include "formatter.hpp"

const sANSIFormatter gFORMATTER_ENABLED{
    .isEnabled = true,
    .CLEAR_TERMINAL = "\033[2J\033[H",
    .RESET = "\033[0m",
    .BOLD = "\033[1m",
    .UNDERLINE = "\033[4m",
    .RED_BACKGROUND = "\033[0;41m",
    .WHITE_BACKGROUND = "\033[0;47m",
    .WHITE_FOREGROUND = "\033[37;0m",
};
const sANSIFormatter gFORMATTER_DISABLED = {};

const sANSIFormatter *gFmt = &gFORMATTER_ENABLED;