// Copyright 2012 The Avalon Project Authors. All rights reserved.
// Use of this source code is governed by the Apache License 2.0
// that can be found in the LICENSE file.

// sms tool for Iridium 9522B Modem (might actually work on general sms-capable modems)
// Honors mgetty/uucp-style port lockfiles.
// - Read destination phone number and message on stdin and send as sms
// - poll all smses stored on the phone and delete them
// - get status, timestamp and geo info
//
// NOTE that before you can use the 9522B to send smses you have to set the sms service number with +CSCA
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <termios.h>
#include <unistd.h>

#include "io2/lib/log.h"
#include "pdu.h"
#include "lockfile.h"

static const char* argv0;
static int debug = 0;

static void 
usage(void)
{
        fprintf(stderr,
                "usage: %s [options] send 4479xxxyyyy \"message ...\" \n"
                "       %s [options] recv > msg.txt \n"
                "       %s [options] status \n"
                "options:\n"
		"\t-D /dev/ttyS0 device to use\n"
                "\t-b 19200      baudrate\n"
                "\t-d            set debug mode\n"
                "\t-L /var/lock/LCK..$(basename dev) lockfile.\n"
                , argv0, argv0, argv0);
        exit(2);
}

static struct termios save_tio;
static int port = -1;
static const char* dev = "/dev/ttyS0";
static char* lckf = NULL;

static void
setserial(int baudrate)
{
        struct termios t;
        if (tcgetattr(port, &t) < 0) crash("tcgetattr(%s)", dev);

	memmove(&save_tio, &t, sizeof t);

	cfmakeraw(&t);
        t.c_cflag |= CRTSCTS | CREAD | CLOCAL;
	t.c_cc[VMIN] = 1;
	t.c_cc[VTIME] = 5;

        switch (baudrate) {
        case 0: break;
        case 4800: cfsetspeed(&t, B4800); break;
        case 9600: cfsetspeed(&t, B9600); break;
        case 19200: cfsetspeed(&t, B19200); break;
        case 38400: cfsetspeed(&t, B38400); break;
        case 57600: cfsetspeed(&t, B57600); break;
        case 115200: cfsetspeed(&t, B115200); break;
        default: crash("Unsupported baudrate: %d", baudrate);
        }

        tcflush(port, TCIOFLUSH);
        if (tcsetattr(port, TCSANOW, &t) < 0) crash("tcsetattr(%s)", dev);
}

static void
resetserial()
{
	// don't crash, b/c that calls exit, and this is in atexit.
	if (tcsetattr(port, TCSANOW, &save_tio) < 0)
		syslog(LOG_ERR, "failed tcsetattr(%s): %s", dev, strerror(errno));
        tcflush(port, TCIOFLUSH);
	close(port);
}

static void
unlock()
{
	if(!unlockfile(lckf))
		syslog(LOG_ERR, "someone stole our lock (%s)", lckf);
}

static void timeout() { crash("No response from modem."); }

static int
startswith(const char* pfx, const char* s) {
	while(*pfx)
		if (*pfx++ != *s++)
			return 0;
	return 1;
}

static int c2h(char c) {
	if (isdigit(c)) return c - '0';
	if (islower(c)) return 10 + c - 'a';
	if (isupper(c)) return 10 + c - 'A';
	return -1;
}

static const char* networkregstr[] = {
	"not registered",
	"registered, home network",
	"searching new operator",
	"registration denied",
	"unknown",
	"registered, roaming",
};


int main(int argc, char* argv[]) {

	int ch;
        int baudrate = 19200;
	int retries = -1;

        argv0 = strrchr(argv[0], '/');
        if (argv0) ++argv0; else argv0 = argv[0];
	
	while ((ch = getopt(argc, argv, "b:dD:L:hr:")) != -1){
		switch (ch) {
		case 'b': baudrate = atoi(optarg); break;
		case 'd': ++debug; break;
		case 'D': dev = optarg; break;
		case 'L': lckf = optarg; break;
		case 'r': retries = atoi(optarg); break;
		case 'h': 
		default:
			usage();
		}
	}
	
        argv += optind; argc -= optind;

        if (argc < 1) usage();
	if (!strcmp("send", argv[0])) {
		if(argc < 3) usage();
		if(strlen(argv[2]) > 160) crash("sms message too long: '%s'", argv[2]);
	} else if (strcmp("recv", argv[0])) {
		if(argc != 1) usage();
	} else if (strcmp("status", argv[0])) {
		if(argc != 1) usage();
	} else usage();

        if (signal(SIGBUS,  fault)   == SIG_ERR) crash("signal(SIGBUS)");
        if (signal(SIGSEGV, fault)   == SIG_ERR) crash("signal(SIGSEGV)");
	if (signal(SIGALRM, timeout) == SIG_ERR) crash("signal(SIGSALRM)");

        openlog(argv0, (debug?LOG_PERROR:0), LOG_UUCP);
	if(!debug) setlogmask(LOG_UPTO(LOG_NOTICE));

	char cmdstr[100];
	char pdustr[2*SMS_MAX_PDU_LENGTH+4];

	if (!strcmp("send", argv[0])) {

		syslog(LOG_INFO, "sending sms to +%s: \"%s\"", argv[1], argv[2]);
		
		unsigned char pdu[SMS_MAX_PDU_LENGTH];
		int pdu_len = pdu_encode("", argv[1], argv[2], pdu, sizeof pdu);
		if (pdu_len < 0) crash("error encoding to PDU: %s \"%s\"", argv[1], argv[2]);

		const int pdu_len_except_smsc = pdu_len - 1 - pdu[0];
		snprintf(cmdstr, sizeof cmdstr, "AT+CMGS=%d\r\n", pdu_len_except_smsc);

		int i;
		for (i = 0; i < pdu_len; ++i) sprintf(pdustr+2*i, "%02X", pdu[i]);
		syslog(LOG_DEBUG, "pdu: %s", pdustr);
		sprintf(pdustr+2*i, "%c\r\n", 0x1A);   // End PDU mode with Ctrl-Z.
	}

	// Go get the lock

	if (!lckf) {
		const char *base = strrchr(dev, '/');
		if(!base) base = dev; else ++base;
		asprintf(&lckf, "/var/lock/LCK..%s", base);
		syslog(LOG_DEBUG, "lockfile: %s", lckf);
	}

	while(retries-- && !lockfile(lckf)) {
		syslog(LOG_INFO, "Failed to lock file %s: %s", lckf, strerror(errno));
		sleep(10);
	}

	if (!retries) crash("Failed to obtain lock (%s)", lckf);

	atexit(unlock);
	syslog(LOG_DEBUG, "Got lockfile %s", lckf);

	// open the port

	port = open(dev, O_RDWR|O_NONBLOCK|O_NOCTTY);
	if (port < 0) crash("open(%s)", dev);
	setserial(baudrate);
	atexit(resetserial);

	syslog(LOG_DEBUG, "opened port %s", dev);

	close(port);
	port = open(dev, O_RDWR|O_NOCTTY);
	if (port < 0) crash("reopen(%s)", dev);
	syslog(LOG_DEBUG, "reopened port %s", dev);

	FILE* pf = fdopen(port, "w");
	FILE* pfi = fdopen(port, "r");
	if (!pf || ! pfi) crash("huh?");
	if(setvbuf(pf, NULL, _IOLBF, 0))
		syslog(LOG_WARNING, "failed to make serial port linebuffered");

	char buf[1024];
	if (!strcmp("send", argv[0])) {

		fputs(cmdstr, pf); //fflush(pf);
		sleep(1);
		fputs(pdustr, pf); //fflush(pf);
	
		syslog(LOG_DEBUG, "wrote commands");
	
		alarm(30);
		errno = 0;		

		while(fgets(buf, sizeof buf, pfi)) {
			syslog(LOG_DEBUG, "modem: '%s'", buf);
			if(startswith("+CMGS:", buf)) {
				syslog(LOG_NOTICE, "sms sent sucessfully: %s", buf + 7);
				return 0;
			} else if(startswith("+CMS ERROR:", buf)) {
				crash("sms not sent, code: %s", buf + 11);
			} else if(startswith("ERROR", buf)) {
				crash("sms not sent, command error");
			} else if(startswith("OK", buf)) {
				return 0;
			}
		}
		crash("reading port");

	}

	if (!strcmp("recv", argv[0])) {
		fputs("AT+CMGL=4\r\n", pf);
		alarm(10);
		int idx[1024];
		int count  = 0;
		while(fgets(buf, sizeof buf, pfi)) {
			syslog(LOG_DEBUG, "modem: '%s'", buf);
			if(startswith("OK", buf)) break;
			if(startswith("+CMGL:", buf)) {
				if(sscanf(buf+7, "%d,", idx+count) != 1) {
					syslog(LOG_ERR, "unparsable CMGL response: %s", buf+7);
					continue;
				}
				if(!fgets(buf, sizeof buf, pfi))
					crash("reading pdu %d", count);
				++count;
				unsigned char pdu[SMS_MAX_PDU_LENGTH];
				int l = strlen(buf);
				int i;
				for(i = 0; i < l; i+=2)
					pdu[i/2] = 16*c2h(buf[i]) + c2h(buf[i+1]);
				time_t sms_time;
				char pstr[40];
				char txt[161];
				int sms_len = pdu_decode(pdu, l/2, &sms_time, pstr, sizeof pstr, txt, sizeof txt);
				if (sms_len <= 0) {
					syslog(LOG_ERR, "error decoding pdu %d: %s", count-1, buf);
					continue;
				} 
				
				// gsm 7 bit charset should be translated to utf8, but
				// we only care about 0 -> '@'
				for(i = 0; i < sms_len; ++i)
					if(txt[i] == 0) txt[i] = '@';

				printf("%s %s\n", pstr, txt);
			}
		}
		alarm(0);
		int i;
		for(i = 0; i < count; ++i) {
			fprintf(pf, "AT+CMGD=%d\r\n", idx[i]);
			alarm(10);
			while(fgets(buf, sizeof buf, pfi)) {
				syslog(LOG_DEBUG, "modem: '%s'", buf);
				if(startswith("OK", buf)) {
					syslog(LOG_INFO, "Deleted message %d", idx[i]);
					break;
				}
				if(startswith("+CMS ERROR:", buf)) {
					syslog(LOG_ERR, "Error deleting message %d: %s", idx[i], buf+12);
					break;
				}
			}
		}

		alarm(0);
	}

	if (!strcmp("status", argv[0])) {
		
		fputs("AT+CREG?\r\n", pf);
		alarm(10);
		while(fgets(buf, sizeof buf, pfi)) {
			syslog(LOG_DEBUG, "modem: '%s'", buf);
			if(startswith("+CREG:", buf)) {
				int dum, st;
				if(sscanf(buf+7, "%d,%d", &dum, &st)!=2 || st < 0 || st > 5) {
					syslog(LOG_ERR, "unparsable CREG response: %s", buf+7);
					continue;
				}
				syslog(LOG_NOTICE, "Network registration: %s", networkregstr[st]);
				//printf("Network registration: %s", networkregstr[st]);
				break;
			}
		}

		fputs("AT+CSQ\r\n", pf);
		alarm(10);
		while(fgets(buf, sizeof buf, pfi)) {
			syslog(LOG_DEBUG, "modem: '%s'", buf);
			if(startswith("+CSQ:", buf)) {
				syslog(LOG_NOTICE, "Signal quality: %d%%", atoi(buf+5)*20);
				//printf("Signal quality: %d%%", atoi(buf+5)*20);
				break;
			}
		}

		fputs("AT-MSGEO\r\n", pf);
		alarm(10);
		while(fgets(buf, sizeof buf, pfi)) {
			syslog(LOG_DEBUG, "modem: '%s'", buf);
			if(startswith("-MSGEO:", buf)) {
				int x, y, z, t;
				if (sscanf(buf+7, "%d,%d,%d,%x", &x, &y, &z, &t) != 4) {
					syslog(LOG_ERR, "Geo report unparseable: %s", buf + 7);
					break;
				}

				// Iridium system time epoch: March 8, 2007, 03:50:21.00 GMT. 
				// (Note: the original Iridium system time epoch was June 1, 
				// 1996, 00:00:11 GMT, and was reset to the new epoch in January, 2008).
				enum { IRIDIUM_SYSTEM_TIME_SHIFT_S = 1173325821 };
				double lat = atan2(z, sqrt(x*x + y*y)) * (180.0 / M_PI);
				double lng = atan2(y, x) * (180.0 / M_PI);
				int64_t ts_ms = t * 90LL + (IRIDIUM_SYSTEM_TIME_SHIFT_S * 1000LL);
				time_t ts_s = ts_ms / 1000;
				char tbuf[200];
				strftime(tbuf, sizeof tbuf, "%a %F %T", gmtime(&ts_s));
				syslog(LOG_NOTICE, "Iridium time: %s", tbuf);
				syslog(LOG_NOTICE, "Geo report: iridium_timestamp_ms:%lld lat_deg:%.7lf lon_deg:%.7lf", ts_ms, lat, lng);
				printf("avalon %.7lf %.7lf  %s", lat, lng, tbuf);
				break;
			}
		}
		alarm(0);
	}


	return 0;
}
