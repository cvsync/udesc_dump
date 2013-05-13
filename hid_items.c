/*-
 * Copyright (c) 1999-2003 MAEKAWA Masahide <gehenna@daemon-systems.org>
 * Copyright (c) 2000 UCHIYAMA Yasushi
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
 *	$Id: hid_items.c,v 1.12 2003/07/11 07:56:04 maekawa Exp $
 */

#include <stdio.h>

#include <dev/usb/usb.h>

#include "main.h"
#include "hid.h"
#include "hiddefs.h"

uint32_t usage_page = 0;
int level = 3, relative;

static void hid_main_bits(int, uint8_t *, size_t);

/*
 * Main Item
 */
void
hid_main_item(int tag, uint8_t *buf, size_t bufsize)
{
	const char *name;

	switch (tag) {
	case HID_MAIN_ITEM_INPUT:
		printf(": ");
		hid_main_bits(tag, buf, bufsize);
		break;
	case HID_MAIN_ITEM_OUTPUT:
		printf(": ");
		hid_main_bits(tag, buf, bufsize);
		break;
	case HID_MAIN_ITEM_COLLECTION:
		if ((name = hid_get_collection_name(*buf)) != NULL)
			printf("(%s)", name);
		else
			printf("(Reserved: %02x)", *buf);

		if (bufsize > 1)
			printf(" - Illegal bSize %lu", (unsigned long)bufsize);
		printf("\n");
		level++;
		break;
	case HID_MAIN_ITEM_FEATURE:
		printf(": ");
		hid_main_bits(tag, buf, bufsize);
		break;
	case HID_MAIN_ITEM_ENDCOLLECTION:
		printf("\n");
		break;
	}
}

static void
hid_main_bits(int tag, uint8_t *buf, size_t bufsize)
{
	if (bufsize == 0) {
		printf(" {\n");

		level++;

		iprintf("Data\n");
		iprintf("Array\n");
		iprintf("Absolute\n");
		iprintf("No Wrap\n");
		iprintf("Linear\n");
		iprintf("Preferred State\n");
		iprintf("No Null position\n");

		level--;

		relative = 0;

		iprintf("\n");

		return;
	}

	if (BIT(*buf, 0, 1)) {
		printf("(Constant)\n");
		relative = 0;
		return;
	} else {
		bprintf(buf, bufsize);
		level++;
		printf(" {\n");
		iprintf("Data\n");
	}

	iprintf("%s\n", BIT(*buf, 1, 1) ? "Variable" : "Array");
	iprintf("%s\n", BIT(*buf, 2, 1) ? "Relative" : "Absolute");
	iprintf("%s\n", BIT(*buf, 3, 1) ? "Wrap" : "No Wrap");
	iprintf("%s\n", BIT(*buf, 4, 1) ? "Non Linear" : "Linear");
	iprintf("%s\n", BIT(*buf, 5, 1) ? "No Preferred" : "Preferred State");
	iprintf("%s\n", BIT(*buf, 6, 1) ? "Null State" : "No Null position");

	if (tag != 8)
		iprintf("%s\n", BIT(*buf, 7, 1) ? "Volatile" : "Non Volatile");

	if (bufsize > 1) {
		buf++;
		iprintf("%s\n", BIT(*buf, 0, 1) ?
			"Buffered Bytes" : "Bit Field");
	}

	level--;

	relative = BIT(buf[0], 1, 3);

	iprintf("}\n");
}

/*
 * Global Item
 */
void
hid_global_item(int tag, uint8_t *buffer, size_t bufsize)
{
	const char *name;
	uint32_t val = 0;
	int i;

	switch (bufsize) {
	case 1:
		val = *buffer;
		break;
	case 2:
		val = UGETW(buffer);
		break;
	case 4:
		val = UGETDW(buffer);
		break;
	default:
		break;
	}

	switch (tag) {
	case HID_GLOBAL_ITEM_USAGE_PAGE:
		for (usage_page = 0, i = 0 ; (size_t)i < bufsize ; i++)
			usage_page |= (uint32_t)(buffer[i] << (8 * i));

		printf("(");

		if ((name = hid_get_page_name(usage_page)) != NULL)
			printf("%s", name);
		else
			printf("%d", usage_page);

		printf(")\n");
		return;
	}

	if (bufsize > 0) {
		if (relative)
			printf(": %d", (int32_t)val);
		else
			printf(": %u", (uint32_t)val);
	}

	printf("\n");
}

/*
 * Local Item
 */
void
hid_local_item(int tag, uint8_t *buffer, size_t bufsize)
{
	uint32_t val;
	uint16_t usage;
	const char *name;
	int i;

	switch (tag) {
	case HID_LOCAL_ITEM_USAGE:
		printf("(");
		if (bufsize > 2) {
			printf(" - Illegal bSize %lu", (unsigned long)bufsize);
			break;
		}
		for (usage = 0, i = 0 ; (size_t)i < bufsize ; i++)
			usage |= (uint16_t)(buffer[i] << (8 * i));
		if ((name = hid_get_usage_name(usage_page, usage)) != NULL)
			printf("%s", name);
		else
			printf("Reserved:0x%x", usage);
		printf(")\n");
		return;
	}

	if (bufsize > 0) {
		for (val = 0, i = 0 ; (size_t)i < bufsize ; i++)
			val |= (buffer[i] << (8 * i));
		printf("(%x)", val);
	}

	printf("\n");
}
