#include "usb.h"

#include <iostream>
using std::iostream;
using std::cout;
using std::endl;


enum LOG_LEVEL {
    FATAL,
    ERROR,
    WARNING,
    INFO,
    DEBUG
};
static enum LOG_LEVEL current_log_level = FATAL;

template <typename T>
class debug : public iostream {
public:
  debug() : iostream(nullptr) {
    rdbuf(nullptr);
  }
  friend debug& operator<< (debug &me, const T &value) {
    if (current_log_level <= DEBUG){
      (iostream&) me << value;
    }
    return me;
  }
};

template <typename T>
class info : public iostream {
public:
  info() : iostream(nullptr) {
    rdbuf(nullptr);
  }
  friend info& operator<< (info &me, const T &value) {
    if (current_log_level <= INFO){
      (iostream&) me << value;
    }
    return me;
  }
};


class _warning : public std::ostream {
public:
  _warning(std::ostream & os) : std::ostream(os.rdbuf()) {
  }
  template <typename T>
  friend _warning& operator<< (_warning &me, const T &value) {
    if (current_log_level <= WARNING){
      ((std::ostream&) me) << value;
    }
    return me;
  }
};

_warning warning(cout);

static libusb_context *ctx = NULL;
static bool my_USB_init = 0;

int usbOpenDevice(usb_dev_handle *device, int vendor,
  const char *vendorName, int product, const char *productName, const char *port) {
  usb_dev_handle handle = NULL;
  int errorCode = LIBUSB_ERROR_NOT_FOUND;
  if(!my_USB_init) {
    my_USB_init = 1;
    libusb_init(&ctx);
  }

  libusb_device **dev_list;
  int dev_list_len = libusb_get_device_list(ctx, &dev_list);

  int j;
  int r;
  for(j = 0; j < dev_list_len; ++j) {
    libusb_device *dev = dev_list[j];
    struct libusb_device_descriptor descriptor;

    libusb_get_device_descriptor(dev, &descriptor);
    if(descriptor.idVendor == vendor && descriptor.idProduct == product) {
      char string[256];

      // We need to open the device in order to query strings
      r = libusb_open(dev, &handle);
      if(!handle) {
        cx->usb_access_error = 1;
        errorCode = LIBUSB_ERROR_ACCESS;
        warning << "cannot open USB device: " << errstr(pgm, r) << std::endl;
        continue;
      }
      errorCode = 0;
      // Do the names match? if vendorName not given ignore it (any vendor matches)
      r = libusb_get_string_descriptor_ascii(handle, descriptor.iManufacturer & 0xff,
        (unsigned char *) string, sizeof(string));
      if(r < 0) {
        cx->usb_access_error = 1;
        if((vendorName != NULL) && (vendorName[0] != 0)) {
          errorCode = LIBUSB_ERROR_IO;
          warning << "cannot query manufacturer for device: " << errstr(pgm, r) << std::endl;
        }
      } else {
        info << "seen device from vendor >" << string << "<" << std::endl;
        if((vendorName != NULL) && (vendorName[0] != 0) && !str_eq(string, vendorName))
          errorCode = USB_ERROR_NOTFOUND;
      }
      // If productName not given ignore it (any product matches)
      r = libusb_get_string_descriptor_ascii(handle, descriptor.iProduct & 0xff,
        (unsigned char *) string, sizeof(string));
      if(r < 0) {
        cx->usb_access_error = 1;
        if((productName != NULL) && (productName[0] != 0)) {
          errorCode = LIBUSB_ERROR_IO;
          warning << "cannot query product for device: " << errstr(pgm, r) << std::endl;
        }
      } else {
        info << "seen product >" << string << "<" << std::endl;
        if((productName != NULL) && (productName[0] != 0) && !str_eq(string, productName))
          errorCode = USB_ERROR_NOTFOUND;
      }
      if(errorCode == 0) {
        if(!str_eq(port, "usb")) {
          // -P option given
          libusb_get_string_descriptor_ascii(handle, descriptor.iSerialNumber,
            (unsigned char *) string, sizeof(string));
          char bus_num[21];

          sprintf(bus_num, "%d", libusb_get_bus_number(dev));
          char dev_addr[21];

          sprintf(dev_addr, "%d", libusb_get_device_address(dev));
          if(!check_for_port_argument_match(port, bus_num, dev_addr, string))
            errorCode = LIBUSB_ERROR_NOTFOUND;
        }
      }
      if(errorCode == 0)
        break;
      libusb_close(handle);
      handle = NULL;
    }
  }
  libusb_free_device_list(dev_list, 1);


   if(handle != NULL) {
    errorCode = 0;
    *device = handle;
  }
 
  return errorCode;
}


