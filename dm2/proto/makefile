SUBDIRS := api is

all:
	for d in $(SUBDIRS); do \
		$(MAKE) -C $$d; \
	done

install:
	for d in $(SUBDIRS); do \
		$(MAKE) -C $$d install; \
	done

clean:
	for d in $(SUBDIRS); do \
		$(MAKE) -C $$d clean; \
	done

uninstall:
	$(MAKE) -C is uninstall
	$(MAKE) -C api uninstall

.PHONY: all install uninstall clean