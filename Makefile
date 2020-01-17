CFLAGS = -g -Wall -fPIC
CFLAGS += -I.
LDFLAGS += -lrt

MOBJ= shmem.o test_write.o
SOBJ= shmem.o test_read.o

all: $(MOBJ) $(SOBJ)
	$(CC) $(CFLAGS) $(MOBJ) -o master $(LDFLAGS)
	$(CC) $(CFLAGS) $(SOBJ) -o slave $(LDFLAGS)
	rm -f *.o

clean:
	rm -f *.o master slave
