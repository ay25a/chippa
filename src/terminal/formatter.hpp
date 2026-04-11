#pragma once

struct sANSIFormatter {
  bool isEnabled = false;

  // Essentials
  const char *CLEAR_TERMINAL = "";
  const char *RESET = "";

  // Font style
  const char *BOLD = "";
  const char *UNDERLINE = "";
  const char *INVISIBLE = "";

  // Colors
  const char *RED_BACKGROUND;
};

extern const sANSIFormatter gFORMATTER_ENABLED;
extern const sANSIFormatter gFORMATTER_DISABLED;
extern const sANSIFormatter * gFmt;

inline void EnableFormatter(){ gFmt = &gFORMATTER_ENABLED; }
inline void DisableFormatter(){ gFmt = &gFORMATTER_DISABLED; }