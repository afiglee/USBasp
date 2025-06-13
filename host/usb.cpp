#include "usb.h"
#include "log.h"
#include <iostream>

using std::string;
using std::ostream;
using std::endl;

libusb_context *USB::m_ctx = NULL;
bool USB::m_initialized = false;

int USB::usbOpenDevice(int vid, const string& usbvendor, int pid, const string& usbproduct, const std::string& port) {

  usb_dev_handle handle = NULL;
  int errorCode = LIBUSB_ERROR_NOT_FOUND;

  if(!m_initialized) {
    m_initialized = true;
    libusb_init(&m_ctx);
  }

  if (m_device) {
    usbCloseDevice();
    m_device = NULL;
  }

  libusb_device **dev_list;
  int dev_list_len = libusb_get_device_list(m_ctx, &dev_list);

  int j;
  int r;
  for(j = 0; j < dev_list_len; ++j) {
    libusb_device *dev = dev_list[j];
    struct libusb_device_descriptor descriptor;

    libusb_get_device_descriptor(dev, &descriptor);
    debug << "Found vid=0x" << std::hex << descriptor.idVendor << " pid=0x" << descriptor.idProduct << std::dec << std::endl;
    if(descriptor.idVendor == vid && descriptor.idProduct == pid) {
      char str[256];

      // We need to open the device in order to query strings
      r = libusb_open(dev, &handle);
      if(!handle) {
      //  cx->usb_access_error = 1;
        errorCode = LIBUSB_ERROR_ACCESS;
        warning << "cannot open USB device: vid=0x" << std::hex << descriptor.idVendor << " pid=0x" << descriptor.idProduct << std::dec << std::endl;
        continue;
      }
      errorCode = 0;
      // Do the names match? if vendorName not given ignore it (any vendor matches)
      r = libusb_get_string_descriptor_ascii(handle, descriptor.iManufacturer & 0xff,
        (unsigned char *) str, sizeof(str));
      if(r < 0) {
      //  cx->usb_access_error = 1;
        if(!usbvendor.empty()) {
          errorCode = LIBUSB_ERROR_IO;
          warning << "cannot query manufacturer for device: vid=0x" << std::hex << descriptor.idVendor << " pid=0x" << descriptor.idProduct << std::dec << " : ";
        }
      } else {
        info << "seen device from vendor >" << str << "<" << std::endl;
        if(!usbvendor.empty() && usbvendor != str) {
          errorCode = LIBUSB_ERROR_NOT_FOUND;
        }
      }
      // If productName not given ignore it (any product matches)
      r = libusb_get_string_descriptor_ascii(handle, descriptor.iProduct & 0xff,
        (unsigned char *) str, sizeof(str));
      if(r < 0) {
       // cx->usb_access_error = 1;
        if(!usbproduct.empty()) {
          errorCode = LIBUSB_ERROR_IO;
          warning << "cannot query product for device: vid=0x" << std::hex << descriptor.idVendor << " pid=0x" << descriptor.idProduct << std::dec << " : ";
        }
      } else {
        info << "seen product >" << str << "<" << std::endl;
        if(!usbproduct.empty() && usbproduct != str) {
          errorCode = LIBUSB_ERROR_NOT_FOUND;
        }
      }
      if(errorCode == 0) {
        if(port != "usb") {
          // -P option given
          libusb_get_string_descriptor_ascii(handle, descriptor.iSerialNumber,
            (unsigned char *) str, sizeof(str));
          //char bus_num[21];

          //sprintf(bus_num, "%d", libusb_get_bus_number(dev));
          //char dev_addr[21];

          //sprintf(dev_addr, "%d", libusb_get_device_address(dev));
          debug << "bus=" << libusb_get_bus_number(dev) << " device=" << libusb_get_device_address(dev) << " serial=" << str << std::endl;
          //if(!check_for_port_argument_match(port, bus_num, dev_addr, str))
          //  errorCode = LIBUSB_ERROR_NOT_FOUND;
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
    *m_device = handle;
  }
 
  return errorCode;
}


void USB::usbCloseDevice() {
  if (m_device) {
    libusb_close(*m_device);
    m_device = NULL;
  }
}

int USB::transfer(bool receive, const uint8_t function, const uint8_t send[], uint8_t *data, uint16_t dataLen) {
  
  uint8_t bmRequestType = LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE;
  if (receive) {
    bmRequestType |= 0x80;
  }
  uint16_t value = send[0] | (send[1] << 8);
  uint16_t index = send[2] | (send[3] << 8);
  int nbytes = libusb_control_transfer(*m_device, bmRequestType,
    function, value, index, data, dataLen, m_timeout);

  return nbytes;
}

#if 0
static int check_for_port_argument_match(const std::string& port, char *bus, char *device, char *serial_num) {

  if(port.str_starts(port, "usb:")) {
    port += usb_len + 1;
    char *colon_pointer = strchr(port, ':');

    if(colon_pointer) {
      // Value contains ':' character. Compare with bus/device.
      if(strncmp(port, bus, colon_pointer - port))
        return 0;
      port = colon_pointer + 1;
      return str_eq(port, device);
    }
    // Serial number case
    return *port && str_ends(serial_num, port);
  }
  // Invalid -P option.
  return 0;
}
#endif
