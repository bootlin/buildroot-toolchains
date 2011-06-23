#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define AT91_GPBR_ADDR      0xFFFFFD60
#define AT91_GPBR_SIZE              16
#define AT91_REGISTER_COUNT          4

static int _gpbr_do(unsigned int reg, int iswrite, unsigned int *value)
{
	int fd;
	char *startaddr;
	volatile unsigned int *regaddr;
	unsigned int pagesize;
	unsigned int pageaddr, pageoffset;

	fd = open("/dev/mem", O_RDWR);
	if (fd < 0)
		return -1;

	pagesize   = getpagesize();
	pageaddr   = (AT91_GPBR_ADDR & ~(pagesize - 1));
	pageoffset = (AT91_GPBR_ADDR &  (pagesize - 1));

	startaddr = mmap(NULL, pagesize, (PROT_READ | PROT_WRITE),
		    MAP_SHARED, fd, pageaddr);
	if (startaddr == MAP_FAILED) {
		close (fd);
		return -1;
	}

	regaddr = (unsigned int*) (startaddr + pageoffset);
	regaddr += reg;

	if (iswrite)
		*regaddr = *value;
	else
		*value = *regaddr;

	munmap(startaddr, pagesize);
	close(fd);

	return 0;
}

int gpbr_read(unsigned int reg, unsigned int *value)
{
	if (reg >= AT91_REGISTER_COUNT)
		return -1;

	return _gpbr_do(reg, 0, value);
}

int gpbr_write(unsigned int reg, unsigned int value)
{
	if (reg >= AT91_REGISTER_COUNT)
		return -1;

	return _gpbr_do(reg, 1, & value);
}
