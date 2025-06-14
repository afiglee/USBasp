#pragma once
#include <stdint.h>
#include <string>

enum updateflags : uint8_t {
  UF_NONE = 0,
  UF_NOWRITE = 1,
  UF_AUTO_ERASE = 2,
  UF_VERIFY = 4,
  UF_NOHEADING = 8,
};

enum device_op : uint8_t { DEVICE_READ, DEVICE_WRITE, DEVICE_VERIFY };

typedef enum mem_format : int8_t {
  FMT_ERROR = -1,
  FMT_AUTO,
  FMT_SREC,
  FMT_IHEX,
  FMT_RBIN,
  FMT_IMM,
  FMT_EEGG,
  FMT_HEX,
  FMT_DEC,
  FMT_OCT,
  FMT_BIN,
  FMT_ELF,
  FMT_IHXC,
} FILEFMT;

typedef struct update {
  std::string memstr;   // Memory name for -U
  device_op op;         // Symbolic memory operation DEVICE_... for -U
  std::string filename; // Filename for -U, can be -
  FILEFMT format;       // File format FMT_...
  update() : op{DEVICE_WRITE}, format{FMT_AUTO} {}
} UPDATE;
