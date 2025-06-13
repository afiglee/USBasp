#ifndef _USBASP_H__
#define _USBASP_H__


#if defined(HAVE_LIBUSB_1_0)
    #if defined(HAVE_LIBUSB_1_0_LIBUSB_H)
        #include <libusb-1.0/libusb.h>
    #else
        #include <libusb.h>
    #endif
    typedef libusb_device_handle *usb_dev_handle;
#else
    #error "libusb support is required for this project. Please install libusb or libusb-1.0."    
#endif

#include "usb.h"
#include <string>

enum USBASP_FUNC : uint8_t {
    USBASP_FUNC_CONNECT =  1,
    USBASP_FUNC_DISCONNECT = 2,
    USBASP_FUNC_TRANSMIT = 3,
    USBASP_FUNC_READFLASH = 4,
    USBASP_FUNC_ENABLEPROG = 5,
    USBASP_FUNC_WRITEFLASH = 6,
    USBASP_FUNC_READEEPROM = 7,
    USBASP_FUNC_WRITEEEPROM = 8,
    USBASP_FUNC_SETLONGADDRESS = 9,
    USBASP_FUNC_SETISPSCK = 10,
    USBASP_FUNC_TPI_CONNECT = 11,
    USBASP_FUNC_TPI_DISCONNECT = 12,
    USBASP_FUNC_TPI_RAWREAD = 13,
    USBASP_FUNC_TPI_RAWWRITE = 14,
    USBASP_FUNC_TPI_READBLOCK  = 15,
    USBASP_FUNC_TPI_WRITEBLOCK = 16,
    USBASP_FUNC_GETCAPABILITIES = 127
};

struct Config {
    unsigned short usbpid;
    int usbvid;
    std::string vendorName;
    std::string productName;
    unsigned int timeout;
};

class USBAsp : protected USB {
    public:
    USBAsp(const std::shared_ptr<Config>& config) : 
        USB(config->timeout), m_config(config) {}
    int open(const std::string& port);
    void close();
    int transmit(bool receive, enum USBASP_FUNC functionid, const uint8_t *send, uint8_t *daya, uint16_t datalen);
protected:
    static const char *get_funcname(const enum USBASP_FUNC &func_name);
    std::shared_ptr<Config> m_config;
};
#endif