#include "usbasp.h"
#include "log.h"

#include <string.h>

#include <iostream>
using std::cout;
using std::endl;
using std::iostream;
using std::ostream;

// Wrapper for usb_control_msg call
int USBAsp::transmit(bool receive, enum USBASP_FUNC functionid,
                     const uint8_t *send, uint8_t *data, uint16_t datalen) {

  debug << "usbasp_transmit(" << USBAsp::get_funcname(functionid) << ", 0x"
        << send[0] << send[1] << send[2] << send[3] << endl;
  if (!receive && datalen > 0) {
    int i;

    debug << " => ";
    for (i = 0; i < datalen; i++)
      if (i) {
        debug << ",";
      }
    debug << "0x" << data[i];
    debug << endl;
  }

  int nbytes = transfer(receive, functionid, send, data, datalen);

  if (nbytes < 0) {
    error << "error to transfer to/from USB" << endl;
    return -1;
  }

  if (receive && nbytes > 0) {
    int i;

    debug << "<= ";
    for (i = 0; i < datalen; i++)
      if (i) {
        debug << ",";
      }
    debug << "0x" << data[i];
    debug << endl;
  }

  return nbytes;
}

// Interface prog
int USBAsp::open() {

  debug << "usbasp_open()\n";

  if (usbOpenDevice(m_config->usbvid, m_config->vendorName, m_config->usbpid,
                    m_config->productName) != 0) {
    error << "cannot find USB device with vid=0x" << std::hex
          << m_config->usbvid << " pid=0x" << std::hex << m_config->usbpid
          << " vendor='" << m_config->vendorName << "'"
          << " product='" << m_config->productName << "'" << std::dec
          << std::endl;
    return -1;
  }

  return 0;
}

void USBAsp::close() {
  debug << "usbasp_close()" << std::endl;

  if (m_device != NULL) {
    unsigned char temp[4];

    memset(temp, 0, sizeof(temp));
#if 0
    if(my.use_tpi) {
      usbasp_transmit(pgm, 1, USBASP_FUNC_TPI_DISCONNECT, temp, temp, sizeof(temp));
    } else {
      usbasp_transmit(pgm, 1, USBASP_FUNC_DISCONNECT, temp, temp, sizeof(temp));
    }
#endif
    usbCloseDevice();
  }
}

const char *USBAsp::get_funcname(const enum USBASP_FUNC &func_name) {
  switch (func_name) {
  case USBASP_FUNC_CONNECT:
    return "USBASP_FUNC_CONNECT";
  case USBASP_FUNC_DISCONNECT:
    return "USBASP_FUNC_DISCONNECT";
  case USBASP_FUNC_TRANSMIT:
    return "USBASP_FUNC_TRANSMIT";
  case USBASP_FUNC_READFLASH:
    return "USBASP_FUNC_READFLASH";
  case USBASP_FUNC_ENABLEPROG:
    return "USBASP_FUNC_ENABLEPROG";
  case USBASP_FUNC_WRITEFLASH:
    return "USBASP_FUNC_WRITEFLASH";
  case USBASP_FUNC_READEEPROM:
    return "USBASP_FUNC_READEEPROM";
  case USBASP_FUNC_WRITEEEPROM:
    return "USBASP_FUNC_WRITEEEPROM";
  case USBASP_FUNC_SETLONGADDRESS:
    return "USBASP_FUNC_SETLONGADDRESS";
  case USBASP_FUNC_SETISPSCK:
    return "USBASP_FUNC_SETISPSCK";
  case USBASP_FUNC_TPI_CONNECT:
    return "USBASP_FUNC_TPI_CONNECT";
  case USBASP_FUNC_TPI_DISCONNECT:
    return "USBASP_FUNC_TPI_DISCONNECT";
  case USBASP_FUNC_TPI_RAWREAD:
    return "USBASP_FUNC_TPI_RAWREAD";
  case USBASP_FUNC_TPI_RAWWRITE:
    return "USBASP_FUNC_TPI_RAWWRITE";
  case USBASP_FUNC_TPI_READBLOCK:
    return "USBASP_FUNC_TPI_READBLOCK";
  case USBASP_FUNC_TPI_WRITEBLOCK:
    return "USBASP_FUNC_TPI_WRITEBLOCK";
  case USBASP_FUNC_GETCAPABILITIES:
    return "USBASP_FUNC_GETCAPABILITIES";
  default:
    return "UNKNOWN";
  }
}
