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
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with this package; if not, write to the Free Software Foundation, Inc., 51
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */
#ifndef USBHID_REPORTS_H_
#define USBHID_REPORTS_H_

#include "libc/types.h"
#include "libc/syscall.h"
#include "autoconf.h"

/*
 * Return the report descriptor length associated to the descriptor identifier given in
 * argument length is encoded in a 8 bits field in the HID descriptor. As a consequence,
 * its size can't be bigger than 255.
 */

/*@
  @ requires \separated( &usbhid_ctx, &usbotghs_ctx, &GHOST_num_ctx, ctx_list+(..), ((uint32_t*)(USB_BACKEND_MEMORY_BASE .. USB_BACKEND_MEMORY_END)));
  @ requires \valid(len);
  @ assigns *len;
  @ ensures \result== MBED_ERROR_INVPARAM || \result== MBED_ERROR_NONE || \result== MBED_ERROR_INVSTATE || \result== MBED_ERROR_NOSTORAGE;
  */
uint8_t usbhid_get_report_desc_len(uint8_t hid_handler, uint8_t index, __out uint8_t *len);

/*
 * forge report descriptor based on the upper stack item lists of the given report id.
 * The upper stack handle its own item list, but not the HID item encoding, which is
 * handled by this function.
 */

/*@
  @ requires \valid_read(buf + (0 .. 255));
  @ requires \separated(&report_oneindex,buf +(0 .. 255),bufsize,&usbhid_ctx, &usbotghs_ctx, &GHOST_num_ctx, ctx_list+(..), ((uint32_t*)(USB_BACKEND_MEMORY_BASE .. USB_BACKEND_MEMORY_END)));
  @ assigns *bufsize, buf[0..255] ;
  @ ensures  \result == MBED_ERROR_NONE || \result ==  MBED_ERROR_INVSTATE || \result == MBED_ERROR_INVPARAM ; 
  */
mbed_error_t usbhid_forge_report_descriptor(uint8_t hid_handler, uint8_t *buf, uint32_t *bufsize, uint8_t index);


/*
 * Return report length from report descriptor identifier. For a given report descriptor
 * identifier, all reports have the same size, based on two global tags: REPORT_SIZE and
 * REPORT_COUNT.
 */

/*@
  @ requires \separated(&usbhid_ctx, &usbotghs_ctx, &GHOST_num_ctx, ctx_list+(..), ((uint32_t*)(USB_BACKEND_MEMORY_BASE .. USB_BACKEND_MEMORY_END)));
  @ assigns \nothing;
  @ ensures 0<= \result <= 255 ;
  */
uint32_t usbhid_get_report_len(uint8_t hid_handler, usbhid_report_type_t type, uint8_t index);

/*
 * is report to send needs to be prefixed by its Report Identifier ?
 */

/*@
  @ requires \separated(&usbhid_ctx, &usbotghs_ctx, &GHOST_num_ctx, ctx_list+(..), ((uint32_t*)(USB_BACKEND_MEMORY_BASE .. USB_BACKEND_MEMORY_END)));
  @ assigns \nothing;
  @
  @ behavior uie_undeclared_iface:
  @   assumes hid_handler < usbhid_ctx.num_iface && hid_handler < MAX_USBHID_IFACES ;
  @   assumes usbhid_ctx.hid_ifaces[hid_handler].declared == \false;
  @   ensures \result == \false;
  @
  @ behavior report_null:
  @   assumes !usbhid_ctx.hid_ifaces[hid_handler].get_report_cb  ;
  @   assumes !(hid_handler < usbhid_ctx.num_iface && hid_handler < MAX_USBHID_IFACES) ;
  @   assumes !(usbhid_ctx.hid_ifaces[hid_handler].declared == \false);
  @   ensures \result == \false;
  @
  @ behavior not_found:
  @   assumes usbhid_ctx.hid_ifaces[hid_handler].get_report_cb  ;
  @   assumes !(hid_handler < usbhid_ctx.num_iface && hid_handler < MAX_USBHID_IFACES) ;
  @   assumes !(usbhid_ctx.hid_ifaces[hid_handler].declared == \false);
  @  assumes  !(\forall integer i; 0 <= i <= ONEINDEX_ITEMS_NUM ==> !(report_oneindex.items[i].type == USBHID_ITEM_TYPE_GLOBAL && report_oneindex.items[i].tag == USBHID_ITEM_GLOBAL_TAG_REPORT_ID) &&  \forall integer i; 0 <= i <= TWOINDEX_ITEMS_NUM ==> !(report_twoindex.items[i].type == USBHID_ITEM_TYPE_GLOBAL && report_twoindex.items[i].tag == USBHID_ITEM_GLOBAL_TAG_REPORT_ID)) ;
  @   ensures \result == \false;
  @
  @ behavior found:
  @   assumes usbhid_ctx.hid_ifaces[hid_handler].get_report_cb  ;
  @   assumes !(hid_handler < usbhid_ctx.num_iface && hid_handler < MAX_USBHID_IFACES) ;
  @   assumes !(usbhid_ctx.hid_ifaces[hid_handler].declared == \false);
  @  assumes !(\forall integer i; 0 <= i <= ONEINDEX_ITEMS_NUM ==> !(report_oneindex.items[i].type == USBHID_ITEM_TYPE_GLOBAL && report_oneindex.items[i].tag == USBHID_ITEM_GLOBAL_TAG_REPORT_ID) &&  \forall integer i; 0 <= i <= TWOINDEX_ITEMS_NUM ==> !(report_twoindex.items[i].type == USBHID_ITEM_TYPE_GLOBAL && report_twoindex.items[i].tag == USBHID_ITEM_GLOBAL_TAG_REPORT_ID));
  @   ensures \result == \true;
  @
  @ complete behaviors;
  @ disjoint behaviors;
*/
bool usbhid_report_needs_id(uint8_t hid_handler, uint8_t index);

/*
 * get back report identifier for report based on its index
 */

/*@
  @ requires \separated(&usbhid_ctx, &usbotghs_ctx, &GHOST_num_ctx, ctx_list+(..), ((uint32_t*)(USB_BACKEND_MEMORY_BASE .. USB_BACKEND_MEMORY_END)));
  @ assigns \nothing;
  @ ensures 0<= \result <= 255 ;
*/
uint8_t usbhid_report_get_id(uint8_t hid_handler, uint8_t index);

#endif/*!USBHID_REPORTS_H_*/
