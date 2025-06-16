#ifndef _USB_H__
#define _USB_H__

#if defined(HAVE_LIBUSB_1_0)
#if defined(HAVE_LIBUSB_1_0_LIBUSB_H)
#include <libusb-1.0/libusb.h>
#else
#include <libusb.h>
#endif
typedef libusb_device_handle *usb_dev_handle;
#else
#error                                                                         \
    "libusb support is required for this project. Please install libusb or libusb-1.0."
#endif

#include <memory>
#include <string>

class USB {

public:
  USB(unsigned int timeout) : m_timeout(timeout) {}
  int usbOpenDevice(int vid, const std::string &usbvendor, int pid,
                    const std::string &usbproduct);
  void usbCloseDevice();
  int transfer(bool receive, const uint8_t function, const uint8_t send[],
               uint8_t *data, uint16_t dataLen);

protected:
  static libusb_context *m_ctx;
  static bool m_initialized;
  usb_dev_handle *m_device;
  unsigned int m_timeout;
};

#endif