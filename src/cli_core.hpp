/// @file cli_core.hpp
/// @brief Defines the main command line elements to be used in the UI
#pragma once

#include "utilities.hpp"
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

// **************************************
// Formatter
// **************************************

/// @struct Fmt
/// @brief an ANSI text unicode storage
///
/// @note Not compile-time constant to enable the user to disable it if it
/// doesn't behave correctly (on very old terminals only). Therefore,
/// `Fmt::Get` method is to be used instead of direct access.
struct Fmt {
  static bool Enabled; // < Can be set to false to disable

  inline static const char *Get(const char *code) {
    return Enabled ? code : "";
  }

  static constexpr const char *CLEAR_TERMINAL = "\033[2J\033[H";
  static constexpr const char *RESET = "\033[0m";
  static constexpr const char *BOLD = "\033[1m";
  static constexpr const char *UNDERLINE = "\033[4m";
  static constexpr const char *RED_BACKGROUND = "\033[0;41m";
  static constexpr const char *GREEN_BACKGROUND = "\033[0;42m";
  static constexpr const char *YELLOW_BACKGROUND = "\033[0;43m";
};

// **************************************
// Ouput-Only Elements
// **************************************

/// @brief Prints bold and underlined `content`
#define cli_header(content)                                                    \
  std::cout << '\n'                                                            \
            << Fmt::Get(Fmt::BOLD) << Fmt::Get(Fmt::UNDERLINE) << content      \
            << Fmt::Get(Fmt::RESET) << "\n\n"

/// @brief Prints bold `content`
#define cli_subheader(content)                                                 \
  std::cout << '\n'                                                            \
            << Fmt::Get(Fmt::BOLD) << content << Fmt::Get(Fmt::RESET) << '\n'

/// @brief Prints normal `content` (used for consistancy)
#define cli_text(content) std::cout << content << "\n"

/// @brief Prints bold `name` with normal `value`
/// @example 'name: value'
#define cli_field(name, value)                                                 \
  std::cout << Fmt::Get(Fmt::BOLD) << name << ": " << Fmt::Get(Fmt::RESET)     \
            << value << '\n'

/// @brief Prints red and bold `content` with '[Error] ' prefix and wait for
/// 'Enter' key to be pressed
/// @example '[Error] message'
#define cli_error(content)                                                     \
  do {                                                                         \
    std::cout << Fmt::Get(Fmt::RED_BACKGROUND) << Fmt::Get(Fmt::BOLD)          \
              << "[Error] " << content << Fmt::Get(Fmt::RESET) << '\n';        \
    cli_press_enter();                                                         \
  } while (0)

/// @brief Prints green and bold `content` with '[Success] ' prefix and wait for
/// 'Enter' key to be pressed
/// @example '[Success] message'
#define cli_success(content)                                                   \
  do {                                                                         \
    std::cout << Fmt::Get(Fmt::GREEN_BACKGROUND) << Fmt::Get(Fmt::BOLD)        \
              << "[Success] " << content << Fmt::Get(Fmt::RESET) << '\n';      \
    cli_press_enter();                                                         \
  } while (0)

/// @brief Prints yellow and bold `content` with '[Warning] ' prefix
/// @example '[Warning] message'
#define cli_warning(content)                                                   \
  std::cout << Fmt::Get(Fmt::YELLOW_BACKGROUND) << Fmt::Get(Fmt::BOLD)         \
            << "[Warning] " << content << Fmt::Get(Fmt::RESET) << '\n'

/// @brief Repeats `ch` a `length` amount of time
/// @param length The amount of repeations
/// @param ch a Character to repeat
inline void cli_separator(size_t length, char ch = '=') {
  std::cout << std::string(length, ch) << '\n';
}

/// @brief Clears the terminal
inline void cli_clear() {
  if (Fmt::Enabled)
    std::cout << Fmt::Get(Fmt::CLEAR_TERMINAL);
  else
    std::cout << std::string(50, '\n');
}

// **************************************
// Input-Output Elements
// **************************************

/// @brief Waits for the user to press 'Enter'
inline void cli_press_enter() {
  std::cout << Fmt::Get(Fmt::BOLD) << "Press any key " << Fmt::Get(Fmt::RESET);
  std::string dummy;
  std::getline(std::cin, dummy);
}

/// @brief Prints prompt then takes input from the user
/// @param prompt The prompt to print before taking input
inline std::string cli_input(const std::string &prompt) {
  std::cout << prompt;
  std::string value;
  std::getline(std::cin, value);
  return value;
}

/// @brief Prints a confirmation message
/// @param prompt the prompt to print before taking input
/// @return true if the user inputs 'y'/'yes', and false if user inputs 'n'/'no'
inline bool cli_boolean(const std::string &prompt) {
  std::string str;
  // Loop until a valid yes/no response is given
  for (;;) {
    str = cli_input(prompt + " (y/n) ");
    // Check for negative responses
    if (str == "n" || str == "no")
      return false;

    // Check for affirmative responses
    else if (str == "y" || str == "yes")
      return true;

    cli_error("Invalid option ('yes'/'y' to confirm, 'no'/'n' to deny)");
  }
}

/// @brief Displays a numbered menu of options and prompts for user selection
/// @param items Vector of strings representing the menu options
/// @return The index of the selected option (0-based)
inline size_t cli_menu(const std::vector<std::string> &items) {
  cli_separator(20);
  for (size_t i = 0; i < items.size(); ++i)
    std::cout << "(" << i << ") " << items[i] << "\n";

  cli_separator(10);

  std::string input;
  // Loop until a valid choice is entered
  for (;;) {
    std::cout << "(0 - " << items.size() - 1 << ") > ";
    std::getline(std::cin, input);

    int choice = -1;
    // Attempt to convert input to integer
    if (is_integer(input))
      choice = std::stoi(input);

    // Validate choice is within range
    if (choice < 0 || choice > (int)(items.size() - 1))
      cli_error("Invalid choice!");
    else
      return choice;
  }
}

/// @brief Prints a single row of a table with specified column widths
/// @param widths Vector of widths for each column
/// @param row Vector of strings for each cell in the row
inline void 
cli_table_row(const std::vector<size_t> &widths, const std::vector<std::string> &row) {
  // Print each cell with left alignment and 2-space padding
  for (size_t i = 0; i < row.size(); ++i)
    std::cout << std::left << std::setw(widths[i] + 2) << row[i];

  std::cout << '\n';
}

/// @brief Prints a formatted table with headers and data rows
/// @param cols Vector of column header strings
/// @param rows Vector of rows, where each row is a vector of cell strings
/// @note Automatically calculates column widths based on content
inline void 
cli_table(const std::vector<std::string> &cols, const std::vector<std::vector<std::string>> &rows) {
  std::vector<size_t> widths(cols.size(), 0);
  // Initialize widths with header lengths
  for (size_t i = 0; i < cols.size(); ++i)
    widths[i] = cols[i].size();

  // Update widths to accommodate the longest content in each column
  for (const auto &row : rows)
    for (size_t i = 0; i < row.size(); ++i)
      widths[i] = std::max(widths[i], row[i].size());

  // Calculate total table width including padding
  size_t total = 0;
  for (size_t w : widths)
    total += w + 2;
  cli_separator(total, '-');

  // Print header row in bold
  std::cout << Fmt::Get(Fmt::BOLD);
  cli_table_row(widths, cols);
  std::cout << Fmt::Get(Fmt::RESET);
  cli_separator(total, '-');

  // Print data rows
  for (const auto &r : rows)
    cli_table_row(widths, r);
  cli_separator(total, '-');
}

inline std::string 
cli_valid_input(const std::string &prompt, std::function<std::string(const std::string &)> validate) {
  for (;;) {
    std::string in = cli_input(prompt);

    std::string error = validate(in);
    if (error.empty())
      return in;

    cli_error(error);
  }
}
