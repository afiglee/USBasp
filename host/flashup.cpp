#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "flashup.h"
#include "log.h"

using std::endl;
using std::stoi;
using std::string;

void usage(int exit_code) {
  std::cout << "TODO" << std::endl;
  exit(exit_code);
}

static int is_multimem(const std::string &mem) {
  return mem == "ALL" || mem == "all" || mem == "etc" || mem == "-";
}

FILEFMT fileio_format(char c) {
  switch (c) {
  case 'a':
    return FMT_AUTO;
  case 's':
    return FMT_SREC;
  case 'i':
    return FMT_IHEX;
  case 'I':
    return FMT_IHXC;
  case 'r':
    return FMT_RBIN;
  case 'e':
    return FMT_ELF;
  case 'm':
    return FMT_IMM;
  case 'R':
    return FMT_EEGG;
  case 'b':
    return FMT_BIN;
  case 'd':
    return FMT_DEC;
  case 'h':
    return FMT_HEX;
  case 'o':
    return FMT_OCT;
  default:
    return FMT_ERROR;
  }
}

/*
 * Parsing of [<memory>:<op>:<file>[:<fmt>] | <file>[:<fmt>]]
 *
 * As memory names don't contain colons and the r/w/v operation <op> is a
 * single character, check whether the first two colons sandwich one character.
 * If not, treat the argument as a filename (defaulting to flash write). This
 * allows colons in filenames other than those for enclosing <op> and
 * separating <fmt>, eg, C:/some/file.hex
 */
std::shared_ptr<UPDATE> parse_op(const char *inp) {
  // Assume -U <file>[:<fmt>] first
  std::shared_ptr<UPDATE> upd = std::make_shared<UPDATE>();

  // upd->op = DEVICE_WRITE;
  const char *fn = inp;

  // Check for <memory>:c: start in which case override defaults
  const char *fc = strchr(inp, ':');

  if (fc && fc[1] && fc[2] == ':') {
    if (!strchr("rwv", fc[1])) {
      error << "invalid I/O mode :" << fc[1] << ": in -U " << inp << std::endl;
      error << "I/O mode can be r, w or v for read, write or verify device"
            << std::endl;
      upd.reset();
      return upd;
    }

    upd->memstr = std::string(inp, fc - inp);
    upd->op = fc[1] == 'r'   ? DEVICE_READ
              : fc[1] == 'w' ? DEVICE_WRITE
                             : DEVICE_VERIFY;
    fn = fc + 3;
  }
  // Autodetect for file reads, and hex (multi-mem)/raw (single mem) for file
  // writes
  upd->format = upd->op != DEVICE_READ     ? FMT_AUTO
                : is_multimem(upd->memstr) ? FMT_IHXC
                                           : FMT_RBIN;

  // Filename: last char is format if the penultimate char is a colon
  size_t len = strlen(fn);

  if (!std::string(fn).starts_with("urboot:") && len > 2 &&
      fn[len - 2] == ':') { // Format specifier
    upd->format = fileio_format(fn[len - 1]);
    if (upd->format == FMT_ERROR) {
      error << "invalid format " << (fn[len - 1]);
      upd.reset();
      return upd;
    }
    len -= 2;
  }

  upd->filename = std::string(fn, len);

  return upd;
}

int main(int argc, char *argv[]) {

  uint8_t uflags = UF_AUTO_ERASE | UF_VERIFY;
  int baudrate; // TODO default baudrate?
  double bitclock;
  int ispdelay;
  int showversion = 0;
  int erase = 0;
  int explicit_e = 0;
  int ovsigck = 0;
  int calibrate = 0;
  int verbose = 0;
  int firmware = 0;
  int deviceid = 0;

  std::string port, partdesc;

  std::vector<std::shared_ptr<UPDATE>> updates;

  // Process command line arguments
  struct option longopts[] = {{"help", no_argument, NULL, '?'},
                              {"baud", required_argument, NULL, 'b'},
                              {"bitclock", required_argument, NULL, 'B'},
                              {"noerase", no_argument, NULL, 'D'},
                              {"erase", no_argument, NULL, 'e'},
                              {"firmware-version", no_argument, NULL, 'f'},
                              {"test-memory", no_argument, NULL, 'n'},
                              {"osccal", no_argument, NULL, 'O'},
                              {"part", required_argument, NULL, 'p'},
                              {"port", required_argument, NULL, 'P'},
                              {"memory", required_argument, NULL, 'U'},
                              {"verbose", no_argument, NULL, 'v'},
                              {"noverify-memory", no_argument, NULL, 'V'},
                              {"version", no_argument, &showversion, 0},
                              {NULL, 0, NULL, 0}};

  int option_idx = 0;
  char ch;

  while ((ch = getopt_long(argc, argv, "?b:B:DdefFi:np:OP:U:vV",
                           longopts, &option_idx)) != -1) {
    switch (ch) {
    case 'b':                       // Override default programmer baud rate
      baudrate = std::stoi(optarg); // throws std::invalid_argument
                                    // throws std::out_of_range
      break;

    case 'B': {

      // Specify bit clock period
      char *end;
      bitclock = std::strtod(optarg, &end);
      if ((end == optarg) || bitclock <= 0.0) {
        error << "invalid bit clock period " << optarg << std::endl;
        exit(1);
      }
      while (*end && isascii(*end & 0xff) && isspace(*end & 0xff))
        end++;
      if (*end == 0 || !strcasecmp(end, "us")) // us is optional and the default
        ;
      else if (strcasecmp(end, "m") || strcasecmp(end, "mhz"))
        bitclock = 1 / bitclock;
      else if (strcasecmp(end, "k") || strcasecmp(end, "khz"))
        bitclock = 1e3 / bitclock;
      else if (strcasecmp(end, "hz"))
        bitclock = 1e6 / bitclock;
      else {
        error << "invalid bit clock unit " << end << std::endl;
        exit(1);
      }
    } break;

    case 'i':                       // Specify isp clock delay
      ispdelay = std::stoi(optarg); // throws std::invalid_argument
                                    // throws std::out_of_range
      if (ispdelay == 0) {
        error << "invalid isp clock delay '" << optarg << "' specified"
              << std::endl;
        exit(1);
      }
      break;

    case 'D': // Disable auto-erase
      uflags &= ~UF_AUTO_ERASE;
      break;
    case 'd':
      deviceid = 1; //Report detected chip
      break;

    case 'e': // Perform a chip erase
      erase = 1;
      explicit_e = 1;
      uflags &= ~UF_AUTO_ERASE;
      break;

    case 'F': // Override invalid signature check
      ovsigck = 1;
      break;

    case 'f': // Report USBasp firmware version 
      firmware = 1;
      break;

    case 'n':
      uflags |= UF_NOWRITE;
      break;

    case 'O': // Perform RC oscillator calibration
      calibrate = 1;
      break;

    case 'p': // Specify AVR part
      partdesc = optarg;
      break;

    case 'P':
      port = optarg;
      break;

    case 'U': {
      std::shared_ptr<UPDATE> upd = parse_op(optarg);
      if (!upd) {
        error << "unable to parse update operation " << optarg << endl;
        usage(1);
      }
      updates.emplace_back(upd);
    } break;

    case 'v':
      verbose++;
      break;

    case 'V':
      uflags &= ~UF_VERIFY;
      break;

    case 0:
      if (longopts[option_idx].flag)
        *longopts[option_idx].flag = 1;
      break;

    case '?': // Help
      usage(0);
      break;

    default:
      error << "invalid option - " << ch << std::endl << std::endl;
      usage(1);
      break;
    }
  }

  if (showversion) {
    std::cout << "Version " << __DATE__ << std::endl
              << "This program inspired by avrdude https://github.com/avrdudes/avrdude/" << std::endl
              << "and may be found at https://github.com/afiglee/USBasp" << std::endl
              << std::endl;
    exit(0);
  }

  return 0;
}