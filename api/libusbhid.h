/*
 *
 * Copyright 2019 The wookey project team <wookey@ssi.gouv.fr>
 *   - Ryad     Benadjila
 *   - Arnauld  Michelizza
 *   - Mathieu  Renard
 *   - Philippe Thierry
 *   - Philippe Trebuchet
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * the Free Software Foundation; either version 3 of the License, or (at
 * ur option) any later version.
 *
 * This package is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this package; if not, write to the Free Software Foundation, Inc., 51
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */
#ifndef LIBUSBHID_H_
#define LIBUSBHID_H_

#include "libc/types.h"
#include "libc/syscall.h"
#include "libusbctrl.h"
#include "autoconf.h"

/******************
 * Triggers various standard HID requests to upper stack
 */
#define USB_CLASS_RQST_GET_REPORT           0x01
#define USB_CLASS_RQST_GET_IDLE             0x02
#define USB_CLASS_RQST_GET_PROTOCOL         0x03
#define USB_CLASS_RQST_SET_REPORT           0x09
#define USB_CLASS_RQST_SET_IDLE             0x0A
#define USB_CLASS_RQST_SET_PROTOCOL         0x0B

/*
 * USB HID class is defined here:
 * https://www.usb.org/sites/default/files/documents/hid1_11.pdf
 */

typedef enum {
    USBHID_SUBCLASS_NONE = 0,
    USBHID_SUBCLASS_BOOT_IFACE = 1
} usbhid_subclass_t;

typedef enum {
    USBHID_PROTOCOL_NONE     = 0,
    USBHID_PROTOCOL_KEYBOARD = 1,
    USBHID_PROTOCOL_MOUSE    = 2
} usbhid_protocol_t;


/**********
 * About item tagging and typing
 * This enumerates permate to upper layers to generate their collections without taking into
 * account the way items are encoded in the HID stack.
 * Collection are passed to the HID layer, which handle the correct encoding based on each
 * successive item tag, type, length and data.
 */
/* TODO: to complete... */
typedef enum {
    USBHID_ITEM_TYPE_MAIN = 0,
    USBHID_ITEM_TYPE_GLOBAL = 1,
    USBHID_ITEM_TYPE_LOCAL = 2,
    USBHID_ITEM_TYPE_RESERVED = 3
} usbhid_item_type_t;


typedef enum {
    /* global item, typed GLOBAL : XXXX 01 NN where XXXX is
     * global item tag, and NN the item size */
    USBHID_ITEM_GLOBAL_TAG_USAGE_PAGE   = 0x0,
    USBHID_ITEM_GLOBAL_TAG_LOGICAL_MIN  = 0x1,
    USBHID_ITEM_GLOBAL_TAG_LOGICAL_MAX  = 0x2,
    USBHID_ITEM_GLOBAL_TAG_PHYSICAL_MIN = 0x3,
    USBHID_ITEM_GLOBAL_TAG_PHYSICAL_MAX = 0x4,
    USBHID_ITEM_GLOBAL_TAG_UNIT_EXPONENT = 0x5,
    USBHID_ITEM_GLOBAL_TAG_UNIT          = 0x6,
    USBHID_ITEM_GLOBAL_TAG_REPORT_SIZE   = 0x7,
    USBHID_ITEM_GLOBAL_TAG_REPORT_ID     = 0x8,
    USBHID_ITEM_GLOBAL_TAG_REPORT_COUNT  = 0x9,
    USBHID_ITEM_GLOBAL_TAG_PUSH          = 0xa,
    USBHID_ITEM_GLOBAL_TAG_POP           = 0xb,
    /* other are reserved */
} usbhid_item_tag_global_t;


typedef enum {
    /* main item, typed MAIN : XXXX 00 NN where XXXX is
     * main item tag, and NN the item size */
    USBHID_ITEM_MAIN_TAG_INPUT = 0x8,
    USBHID_ITEM_MAIN_TAG_OUTPUT = 0x9,
    USBHID_ITEM_MAIN_TAG_FEATURE = 0xb,
    USBHID_ITEM_MAIN_TAG_COLLECTION = 0xa,
    USBHID_ITEM_MAIN_TAG_END_COLLECTION = 0xc
    /* other are reserved */
} usbhid_item_tag_main_t;


typedef enum {
    /* local item, typed MAIN : XXXX 10 NN where XXXX is
     * local item tag, and NN the item size */
    USBHID_ITEM_LOCAL_TAG_USAGE = 0x0,
    USBHID_ITEM_LOCAL_TAG_USAGE_MIN = 0x1,
    USBHID_ITEM_LOCAL_TAG_USAGE_MAX = 0x2,
    USBHID_ITEM_LOCAL_TAG_DESIGNATOR_IDX = 0x3,
    USBHID_ITEM_LOCAL_TAG_DESIGNATOR_MIN = 0x4,
    USBHID_ITEM_LOCAL_TAG_DESIGNATOR_MAX = 0x5,
    USBHID_ITEM_LOCAL_TAG_STRING_IDX     = 0x7,
    USBHID_ITEM_LOCAL_TAG_STRING_MIN     = 0x8,
    USBHID_ITEM_LOCAL_TAG_STRING_MAX     = 0x9,
    USBHID_ITEM_LOCAL_TAG_DELIMITER      = 0xa,
    /* other are reserved */
} usbhid_item_tag_local_t;

typedef enum {
    USBHID_INPUT_TYPE_CONSTANT = 0x01,
    USBHID_INPUT_TYPE_VARIABLE = 0x01 << 1,
    USBHID_INPUT_TYPE_COOR_ABS = 0x01 << 2,
    USBHID_INPUT_TYPE_MINMAXWR = 0x01 << 3,
    USBHID_INPUT_TYPE_PHYSREL  = 0x01 << 4,
    USBHID_INPUT_TYPE_PREFSTATE= 0x01 << 5,
    USBHID_INPUT_TYPE_NONULL   = 0x01 << 6,
} usbhid_input_data_type_t;

/*
 * A collection is an ordered set of items.
 * Each item is defined by its type, tag, size (0, 1 or 2 bytes) and data
 */
typedef struct {
    uint8_t type;
    uint8_t tag;
    uint8_t size;
    uint8_t data1;
    uint8_t data2;
} usbhid_item_info_t;

typedef struct {
    uint32_t num_items;
    uint32_t report_id;
    usbhid_item_info_t items[];
} usbhid_report_infos_t;


/* This is the reports that are sent to the host asynchronously on the
 * IN interrupt pipe */
typedef struct {
    uint8_t id;
    uint8_t data[1];
} usbhid_report_t;

/*********************************************************************
 * Upper layer-defined functions
 */

/*
 * A HID stack handle collections of items (at least one)
 * Collections are a homogeneous set of items handling a given content
 * For this, the upper stack must define two functions compatible with
 * the following API.
 * Again, we use linker timer resolution instead of callbacks for security
 * reasons, as those API are not hotpluggable ones.
 */

/*
 * The upper stack handles collection(s) of item. These collections are
 * identified by their index, and transmitted to the host through the
 * Get_Report request. This function returns the corresponding collection
 * pointer, of usbhid_item_info_t type.
 */
usbhid_report_infos_t *usbhid_get_report(uint8_t hid_handler,
                                         uint8_t index);

/*************************************************
 * USB HID stack API
 */

/*
 * Declaring a new USB HID interface. This HID interface is associated to a given
 * HID device type. This interface is composed of a:
 * EP0 IN & OUT control plane
 * EPx IN INTERRUPT plane
 *
 * The upper stack declare the HID specific properties:
 * - subclass
 * - protocol
 * - descriptors number (including one report descriptor and potential physical descriptors)
 * - IN EP polling period
 *
 * the USB HID stack return a handler to interact with this interface
 */
mbed_error_t usbhid_declare(uint32_t          usbxdci_handler,
                            usbhid_subclass_t hid_subclass,
                            usbhid_protocol_t hid_protocol,
                            uint8_t           num_descriptor,
                            uint8_t           poll_time,
                            uint8_t          *hid_handler);



/*
 * sending an HID report. report index is the index of the
 * report in the report descriptor, starting with 0
 */
mbed_error_t usbhid_send_report(uint8_t hid_handler,
                                uint8_t *report,
                                uint8_t report_index);


/*
 * USB HID getter (get back the current HID states information
 */

/*
 * get back requested values from standard HID requests. These functions return the
 * values of the host requested:
 * IDLE (Set_Idle command)
 * PROTOCOL (Set_Procotol command)
 */
uint16_t usbhid_get_requested_idle(uint8_t hid_handler, uint8_t index);

uint16_t usbhid_get_requested_protocol(uint8_t hid_handler);

bool     usbhid_silence_requested(uint8_t hid_handler, uint8_t index);

/***********************************************************
 * triggers
 *
 * On some HID specific events (received requests or transmition complete,
 * the upper stack may wish to receive event acknowledgement. They can
 * react to this events the way they want, using the upper API or just by
 * doing nothing.
 * There is two types of triggers:
 * - transmition done trigger (when HID data has been sent asynchronously
 *   on the IN interrupt EP)
 * - request received trigger (when the host has requested a HID specific information,
 *   handled at HID stack level). These requests may impact the upper stack which can,
 *   in consequence, react in the trigger.
 */
void usbhid_report_sent_trigger(uint8_t hid_handler, uint8_t index);

/*
 * This trigger is called for each of the above HID requests, when received.
 * This function must be defined by the upper stack in order to be triggered.
 * A weak symbol is defined if no trigger is used.
 */
mbed_error_t usbhid_request_trigger(uint8_t hid_handler, uint8_t hid_req);



#endif/*!LIBUSBHID*/
