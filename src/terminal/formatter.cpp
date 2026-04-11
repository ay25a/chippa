#include "formatter.hpp"

const sANSIFormatter gFORMATTER_ENABLED{
    .isEnabled = true,
    .CLEAR_TERMINAL = "\033[J",
    .RESET = "\033[0m",
    .BOLD = "\033[1m",
    .UNDERLINE = "\033[4m",
    .INVISIBLE = "\033[8m",
    .RED_BACKGROUND = "\033[0;41m",
};
const sANSIFormatter gFORMATTER_DISABLED = {};

const sANSIFormatter *gFmt = &gFORMATTER_ENABLED;