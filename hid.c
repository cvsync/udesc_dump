/*-
 * Copyright (c) 1999-2011 MAEKAWA Masahide <gehenna@daemon-systems.org>
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
 *	$Id: hid.c,v 1.11 2011/04/26 01:29:01 maekawa Exp $
 */

#include <sys/ioctl.h>

#include <stdio.h>
#include <stdlib.h>

#include <err.h>
#include <unistd.h>

#include <dev/usb/usb.h>
#include <dev/usb/usbhid.h>

#include "main.h"
#include "hid.h"
#include "hiddefs.h"
#include "hiddefs_data.h"

static void hid_report(uint8_t *, size_t);

extern int fd, level;

/*
 * Class: Human Interface Devices (HID)
 */
void
hid_device(usb_descriptor_t *desc, int interface)
{
	usb_hid_descriptor_t *hdesc = (usb_hid_descriptor_t *)desc;
	int n, i;

	printf("\t\tHID Descriptor:\n\t\t  bLength           %d\n"
	       "\t\t  bDescriptorType   %02x\n\t\t  bcdHID            %04x\n"
	       "\t\t  bCountryCode      %02x\n\t\t  bNumDescriptors   %d\n",
	       hdesc->bLength, hdesc->bDescriptorType, UGETW(hdesc->bcdHID),
	       hdesc->bCountryCode, hdesc->bNumDescriptors);

	n = hdesc->bNumDescriptors;

	for (i = 0 ; i < n ; i++) {
		uint8_t *buffer;
		size_t length;
		int j;

		printf("\t\t  bDescriptorType   %02x\n"
		       "\t\t  wDescriptorLength %d\n\n",
		       hdesc->descrs[i].bDescriptorType,
		       UGETW(hdesc->descrs[i].wDescriptorLength));

		length = UGETW(hdesc->descrs[i].wDescriptorLength);
		if ((buffer = (uint8_t *)malloc(length)) == NULL)
			err(1, "hid_device");

		if (usb_get_descriptor(interface,
				       hdesc->descrs[i].bDescriptorType, i,
				       interface, buffer, length) == 0) {
			if (hdesc->descrs[i].bDescriptorType == UDESC_REPORT) {
				printf("\t\tReport Desctiptor (ID: %d) {\n", i);
				hid_report(buffer, length);
				printf("\t\t}\n");
			} else {
				printf("\t\t");
				for (j = 0 ; (size_t)j < length ; j++)
					printf(" %02x", buffer[j]);
				printf("\n");
			}
		}

		free(buffer);
	}

	printf("\n");
}

void
hid_report(uint8_t *buffer, size_t bufsize)
{
	int bSize, bType, bTag, i = 0;
	uint8_t *end, item;
	const char *name;

	for (end = buffer + bufsize ; buffer < end ; /* do nothing */) {
		item = *buffer++;
		bSize = HID_ITEM_SIZE_GET(HID_ITEM_SIZE(item));
		bType = HID_ITEM_TYPE(item);
		bTag  = HID_ITEM_TAG(item);

		if (bTag == HID_ITEM_TAG_LONGITEM) {
			fprintf(stderr, "WARNING: long item (reserved for "
					"future use.\n");
			bSize = *buffer++;
			bTag = *buffer++;
		}

		if ((name = hid_get_item_name(bType, bTag)) != NULL) {
			if (bType == HID_ITEM_TYPE_MAIN &&
			    bTag == HID_MAIN_ITEM_ENDCOLLECTION)
				level--;
			iprintf("%s", name);
		} else {
			iprintf("Unknown Item(%01x, %01x): ", bType, bTag);
			for (i = 0 ; i < bSize ; i++)
				printf("%02x ", buffer[i]);
		}

		switch (bType) {
		case HID_ITEM_TYPE_MAIN:
			hid_main_item(bTag, buffer, bSize);
			break;
		case HID_ITEM_TYPE_GLOBAL:
			hid_global_item(bTag, buffer, bSize);
			break;
		case HID_ITEM_TYPE_LOCAL:
			hid_local_item(bTag, buffer, bSize);
			break;
		}

		buffer += bSize; /* next item */
	}
}

const char *
hid_get_item_name(uint8_t type, uint8_t tag)
{
	struct hid_item *hi;

	for (hi = hid_items ; hi->name != NULL ; hi++) {
		if (hi->type == type && hi->tag == tag)
			break;
	}

	return (hi->name);
}

const char *
hid_get_collection_name(uint8_t type)
{
	struct hid_collection *hc;

	if (type >= HID_COLLECTION_VENDOR_START)
		return ("Vendor-defined");

	for (hc = hid_collections ; hc->name != NULL ; hc++) {
		if (hc->type == type)
			break;
	}

	return (hc->name);
}

const char *
hid_get_page_name(uint16_t page)
{
	struct hid_usage_page *hp;

	if (page >= HID_USAGE_PAGE_VENDOR_START)
		return ("Vendor-defined");

	for (hp = hid_usage_pages ; hp->name != NULL ; hp++) {
		if (hp->page == page)
			break;
	}

	return (hp->name);
}

const char *
hid_get_usage_name(uint16_t page, uint16_t usage)
{
	struct hid_usage *hu;

	for (hu = hid_usages ; hu->name != NULL ; hu++) {
		if (hu->page == page && hu->usage == usage)
			break;
	}

	return (hu->name);
}

uint32_t
hid_get_usage_type(uint16_t page, uint16_t usage)
{
	struct hid_usage *hu;

	for (hu = hid_usages ; hu->name != NULL ; hu++) {
		if (hu->page == page && hu->usage == usage)
			break;
	}

	return (hu->type);
}
