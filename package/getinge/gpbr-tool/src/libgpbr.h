#ifndef __LIBGPBR_H__
#define __LIBGPBR_H__

int gpbr_register_read(unsigned int reg, unsigned int *value);
int gpbr_register_write(unsigned int reg, unsigned int value);

#endif /* __LIBGPBR_H__ */
