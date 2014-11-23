CC      = gcc
CCFLAGS = -I/var/lib/gems/2.0.0/gems/trema-0.4.7/src/lib/ -I/var/lib/gems/2.0.0/gems/trema-0.4.7/vendor/openflow-1.0.0/include/openflow/
LDFLAGS = -L/var/lib/gems/2.0.0/gems/trema-0.4.7/objects/lib/ -ltrema -lsqlite3 -lpthread -lrt -g
CPPFLAGS= -DCONFIG_TCP

ifeq ($(TYPE),mptcp)
	CPPFLAGS=-DCONFIG_MPTCP
endif
ifeq ($(TYPE),tcp)
	CPPFLAGS=-DCONFIG_TCP
endif

all: controller

controller: controller.o
	$(CC) -o $@ $^ $(LDFLAGS)

controller.o: controller.c 
	$(CC) -c $(CCFLAGS) $< $(CPPFLAGS)

.PHONY: clean

clean:
	-rm *.o controller
