#include <errno.h>
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>

static int lp_fd_out;
static int lp_fd_in;


#define DEFAULT_OUT "/dev/linkport0"
#define DEFAULT_IN "/dev/linkport1"

#define BUF_LEN		16

static void usage(int status)
{
	fprintf(status ? stderr : stdout,
		"Usage: lp_test [options]\n"
		"\n"
		"Options:\n"
	);
	exit(status);
}

int main(int argc, char *argv[])
{
	int c;
	unsigned char *buffer_out;
	unsigned char *buffer_in;
	int i;
	int ret = 0;

	while ((c = getopt(argc, argv, "s:h")) != EOF)
		switch (c) {
		case 's':
			break;
		case 'h':
			usage(0);
			break;
		default:
			break;
		}

	lp_fd_out = open(DEFAULT_OUT, O_RDWR, 0);
	if (lp_fd_out < 0)
		perror("failed to open");

	lp_fd_in = open(DEFAULT_IN, O_RDWR, 0);
	if (lp_fd_in < 0)
		perror("failed to open");

	if ((buffer_out = malloc(BUF_LEN)) == NULL)
		perror("malloc() failed");

	if (buffer_out)
		memset(buffer_out, 0, BUF_LEN);

	if ((buffer_in = malloc(BUF_LEN)) == NULL)
		perror("malloc() failed");

	if (buffer_in)
		memset(buffer_in, 0, BUF_LEN);

	for (i = 0; i < BUF_LEN; i++)
		buffer_out[i] = i % 0xff;

	if (write(lp_fd_out, buffer_out, BUF_LEN) < 0) {
		perror("failed");
	}

	if (read(lp_fd_in, buffer_in, BUF_LEN) < 0) {
		perror("failed");
	}

	for (i = 0; i < BUF_LEN; i++) {
		if (buffer_in[i] != buffer_out[i]) {
			printf("linkport test failed %x - %x\n", buffer_in[i], buffer_out[i]);
			ret = -1;
			break;
		}
	}

	sleep(2);
	close(lp_fd_out);
	close(lp_fd_in);
	free(buffer_out);
	free(buffer_in);

	if (!ret)
		printf("linkport test passed\n");
	return ret;
}
