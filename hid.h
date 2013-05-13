/*-
 * Copyright (c) 1999-2003 MAEKAWA Masahide <gehenna@daemon-systems.org>
 * Copyright (c) 1999-2000 UCHIYAMA Yasushi
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	$Id: hid.h,v 1.7 2003/01/11 14:46:36 maekawa Exp $
 */
	
/*
 * Class: Human Interface Devices (HID)
 */

/* Item Size */
#define	HID_ITEM_SIZE_0BYTE	0
#define	HID_ITEM_SIZE_1BYTE	1
#define	HID_ITEM_SIZE_2BYTE	2
#define	HID_ITEM_SIZE_4BYTE	3
#define	HID_ITEM_SIZE_GET(x)	((1 << (x)) >> 1)
#define	HID_ITEM_SIZE_MASK	0x3
#define	HID_ITEM_SIZE_SHIFT	0
#define	HID_ITEM_SIZE(cr)					\
	(((cr) >> HID_ITEM_SIZE_SHIFT) & HID_ITEM_SIZE_MASK)

/* Item Type */
#define	HID_ITEM_TYPE_MASK	0x3
#define	HID_ITEM_TYPE_SHIFT	2
#define	HID_ITEM_TYPE(cr)					\
	(((cr) >> HID_ITEM_TYPE_SHIFT) & HID_ITEM_TYPE_MASK)

/* Item Tag */
#define	HID_ITEM_TAG_LONGITEM	0xf
#define	HID_ITEM_TAG_MASK	0xf
#define	HID_ITEM_TAG_SHIFT	4
#define	HID_ITEM_TAG(cr)					\
	(((cr) >> HID_ITEM_TAG_SHIFT) & HID_ITEM_TAG_MASK)

void hid_device(usb_descriptor_t *, int);

void hid_main_item(int, uint8_t *, size_t);
void hid_global_item(int, uint8_t *, size_t);
void hid_local_item(int, uint8_t *, size_t);

const char *hid_get_item_name(uint8_t, uint8_t);
const char *hid_get_collection_name(uint8_t);
const char *hid_get_page_name(uint16_t);
const char *hid_get_usage_name(uint16_t, uint16_t);
uint32_t hid_get_usage_type(uint16_t, uint16_t);
