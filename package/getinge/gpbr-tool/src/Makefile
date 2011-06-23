
OBJS = gpbr-tool.o libgpbr.o

all: gpbr-tool

gpbr-tool: $(OBJS)
	$(CC) -o $@ $^

clean:
	$(RM) -f *.o gpbr-tool
