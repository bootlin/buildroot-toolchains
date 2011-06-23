#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <getopt.h>

#include "libgpbr.h"

void usage(void)
{
	fprintf(stderr, "Usage: gpbr-tool [-o REGISTER] [-r | -w HEXVALUE]\n");
}

int main(int argc, char *argv[])
{
	int opt;
	int offset = 0;
	int doread = 0, dowrite = 0;
	unsigned int val;

	while ((opt = getopt(argc, argv, "o:rw:")) != -1) {
		switch (opt) {
		case 'o':
			offset = atoi(optarg);
			break;
		case 'r':
			doread = 1;
			break;
		case 'w':
			dowrite = 1;
			val = strtoul(optarg, NULL, 16);
			break;
		default:
			fprintf(stderr, "Invalid option %c", opt);
			usage();
			exit(1);
			break;
		}
	}

	if (! (doread ^ dowrite)) {
		fprintf(stderr, "Must select either read (-r) or write (-w)\n");
		usage();
		exit(1);
	}

	if (offset > 3) {
		fprintf(stderr, "Invalid register %d\n", offset);
		usage();
		exit(1);
	}

	if (doread) {
		int ret = gpbr_read(offset, & val);
		if (ret < 0) {
			fprintf(stderr, "Failure while reading GPBR\n");
			exit(1);
		}
		printf("GPBR[%d] = 0x%x\n", offset, val);
	} else {
		int ret = gpbr_write(offset, val);
		if (ret < 0) {
			fprintf(stderr, "Failure while writing GPBR\n");
			exit(1);
		}
	}

	return 0;
}
