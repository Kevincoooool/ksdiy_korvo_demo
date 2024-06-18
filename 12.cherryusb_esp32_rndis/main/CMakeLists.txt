idf_component_register(SRCS 
    "main.c"
    "usbx_net.c"
    "cdc_acm_template.c"
    "cdc_rndis_template.c"

    "usb_cherry/addition/usb_cherry_platform.c"
    "usb_cherry/CherryUSB/class/cdc/usbd_cdc.c"
    "usb_cherry/CherryUSB/class/wireless/usbd_rndis.c"

    "usb_cherry/CherryUSB/class/msc/usbd_msc.c"
    "usb_cherry/CherryUSB/core/usbd_core.c"
    "usb_cherry/CherryUSB/port/dwc2/usb_dc_dwc2.c"

    INCLUDE_DIRS
    "."
    "usb_cherry/CherryUSB/class/wireless"
    "usb_cherry/CherryUSB/class/cdc"
    "usb_cherry/CherryUSB/class/msc"
    "usb_cherry/CherryUSB/common"
    "usb_cherry/CherryUSB/core"
    "usb_cherry/CherryUSB/port/dwc2"
)
