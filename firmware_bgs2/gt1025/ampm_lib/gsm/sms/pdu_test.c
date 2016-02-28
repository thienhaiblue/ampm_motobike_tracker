// Copyright 2011 The Avalon Project Authors. All rights reserved.
// Use of this source code is governed by the Apache License 2.0
// that can be found in the LICENSE file.
#include "pdu.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

int test_check_buffer_size() {
	if (pdu_encode("001234567", "1234567", "hellohello", NULL, 0) >=0 ) {
		printf("SMS check buffer size failed!\n");
		return -1;
	}
	return 0;
}

int test_sms_encode() {
	const unsigned char expected_buffer[] = {
		0x07,  // Header length.
		0x91, 0x72, 0x83, 0x01, 0x00, 0x10, 0xF5,  // Service center.
		0x11, 0x00,  // Message type.
		0x0B, 0x91, 0x64, 0x07, 0x28, 0x15, 0x53, 0xF8,  // SMS phone number.
		0x00, 0x00, 0xB0, // PID, coding, validity.
		0x0A, 0xE8, 0x32, 0x9B, 0xFD, 0x46, 0x97, 0xD9, 0xEC, 0x37  // SMS text.
	};
	// SMS encoding example from: http://www.dreamfabric.com/sms/.
	unsigned char buffer[36];
	int buffer_length = pdu_encode("27381000015", "46708251358", "hellohello", buffer, sizeof(buffer));

	printf("Encoded SMS: ");
	int i = 0;
	for (; i < buffer_length; ++i)
		printf("%02X", buffer[i]);
	printf("\n");
	if (buffer_length != sizeof(expected_buffer)) {
		printf("SMS encoding buffer size failed!\n");
		return -1;
	}

	if (memcmp(expected_buffer, buffer, buffer_length)) {
		printf("SMS encoding failed!\n");
		return -1;
	}
	return 0;
}

int test_sms_decode() {
	const char expected_sms_phone[] = "27838890001";
	const char expected_sms_text[] = "hellohello";
	const unsigned char buffer[] = {
		0x07,  // Header length.
		0x91, 0x72, 0x83, 0x01, 0x00, 0x10, 0xF5,  // Service center.
		0x04, 0x0B,  // Message type.
		0xC8, 0x72, 0x38, 0x88, 0x09, 0x00, 0xF1,  // SMS sender.
		0x00, 0x00, // PID, coding,
		0x99, 0x30, 0x92, 0x51, 0x61, 0x95, 0x80,  // Timestamp.
		0x0A, 0xE8, 0x32, 0x9B, 0xFD, 0x46, 0x97, 0xD9, 0xEC, 0x37  // SMS text.
	};
	// SMS encoding example from: http://www.dreamfabric.com/sms/.
	time_t sms_time;
	char sms_phone[sizeof(expected_sms_phone)];
	char sms_text[sizeof(expected_sms_text)];

	int sms_text_length = pdu_decode(buffer, sizeof(buffer),
					&sms_time,
					sms_phone, sizeof(sms_phone),
					sms_text, sizeof(sms_text));
	printf("Decoded SMS: \"%s\"\n", sms_text);

	if (sms_text_length != strlen(expected_sms_text)) {
		printf("SMS decoding buffer size failed!\n");
		return -1;
	}

	if (memcmp(expected_sms_text, sms_text, sms_text_length)) {
		printf("SMS decoding failed!\n");
		return -1;
	}

	if (strcmp(sms_phone, expected_sms_phone)) {
		printf("SMS decoding phone number failed!\n");
		return -1;
	}

	return 0;
}


int main() {
	return test_check_buffer_size() || test_sms_encode() || test_sms_decode()
		|| printf("ALL TESTS PASSED.\n");
}
