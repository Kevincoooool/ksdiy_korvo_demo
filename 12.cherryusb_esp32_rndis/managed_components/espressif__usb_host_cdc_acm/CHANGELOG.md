## 2.0.3

- Added `cdc_acm_host_cdc_desc_get()` function that enables users to get CDC functional descriptors of the device
- Fixed closing of a CDC device from multiple threads
- Fixed reaction on TX transfer timeout (https://github.com/espressif/esp-protocols/issues/514)

## 2.0.2

- Return an error if the selected interface does not have IN and OUT bulk endpoints

## 2.0.1

- Added support for USB "triple null" devices, which use USB interface association descriptors, but have Device Class, Device Subclass, and Device Protocol all set to 0x00, instead of 0xEF, 0x02, and 0x01 respectively. USB Standard reference: https://www.usb.org/defined-class-codes, Base Class 00h (Device) section.

## 2.0.0

- New function `cdc_acm_host_register_new_dev_callback`. This allows you to get New Device notifications even if you use the default driver.
- Receive buffer has configurable size. This is useful if you expect data transfers larger then Maximum Packet Size.
- Receive buffer has 'append' function. In the Data Received callback you can signal that you wait for more data and the current data were not yet processed. In this case, the CDC driver appends new data to the already received data. This is especially useful if the upper layer messages consist of multiple USB transfers and you don't want to waste more RAM and CPU copying the data around.

## 1.0.4

- C++ methods are now virtual to allow derived classes to override them.

## 1.0.0

- Initial version
