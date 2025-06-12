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
    #error "libusb support is required for this project. Please install libusb or libusb-1.0."    
#endif


int usbOpenDevice(usb_dev_handle *device, int vendor,
  const char *vendorName, int product, const char *productName, const char *port);


#endif