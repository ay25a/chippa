#pragma once

#include "formatter.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <functional>

// Stateless CLI Elements
#ifdef _WIN32
#define cli_clear() system("cls")
#else
#define cli_clear() system("clear")
#endif

#define cli_header(content) std::cout << '\n' << gFmt.BOLD << gFmt.UNDERLINE << content << gFmt.RESET << "\n\n"
#define cli_subheader(content) std::cout << '\n' << gFmt.BOLD << content << gFmt.RESET << '\n'
#define cli_field(name, value) std::cout << gFmt.BOLD << name << ": " << gFmt.RESET << value << '\n'
inline void cli_separator(size_t length, char ch = '=') { 
  std::cout << std::string(length, ch) << '\n';
}

#define cli_error(content) \
  std::cout << gFmt.RED_BACKGROUND << gFmt.BOLD << "[Error] " << content << gFmt.RESET << '\n'

extern void cli_table(const std::vector<std::string> &names, const std::vector<std::vector<std::string>> &values);

// Stateful CLI Elements
extern void cli_confirm();
extern bool cli_boolean(std::string_view prompt);
extern void cli_input(std::string_view prompt, std::string& value);
extern unsigned int cli_menu(const std::vector<std::string> &items);

// Helpers
extern bool StringToInt(std::string_view str, int& out);

template <class _Tp>
_Tp cli_input_valid(const std::string& prompt, std::function<std::string(std::string_view, _Tp&)> v) {
    std::string input;

  while (true) {
    cli_input(prompt, input);

    _Tp value{};
    std::string error = v(input, value);
    if (error.empty())
      return value;

    cli_error(error);
  }
}
