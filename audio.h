/*-
 * Copyright (c) 1999-2003 MAEKAWA Masahide <gehenna@daemon-systems.org>
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
 *	$Id: audio.h,v 1.12 2003/01/11 14:46:36 maekawa Exp $
 */
	
/*
 * Class: Audio
 */
void audio_interface(usb_descriptor_t *, uint16_t);
void audio_endpoint(usb_descriptor_t *, uint16_t);

/*
 * SubClass: Audio Control
 */
void audio_control_interface(usb_descriptor_t *);

/*
 * SubClass: Audio Streaming
 */
void audio_streaming_interface(usb_descriptor_t *);
void audio_streaming_endpoint(usb_descriptor_t *);

/*
 * SubClass: MIDI Streaming
 */
#define	USB_MIDI_MS_HEADER	0x01
#define	USB_MIDI_IN_JACK	0x02
#define	USB_MIDI_OUT_JACK	0x03
#define	USB_MIDI_ELEMENT	0x04

#define	USB_MIDI_EMBEDDED	0x01
#define	USB_MIDI_EXTERNAL	0x02

void audio_midi_interface(usb_descriptor_t *);
void audio_midi_endpoint(usb_descriptor_t *);
