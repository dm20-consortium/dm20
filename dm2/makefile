SUBDIRS := secLib csLib cs commonLib tupleLib dmClientLib is apl/dm2mes conf/is_func
.PHONY: all clean install uninstall $(SUBDIRS)

all:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir all || exit $$?; \
	done

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean || exit $$?; \
	done

install:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir install || exit $$?; \
	done
#カーネルチューニング
#	sysctl -w net.core.rmem_default=2129920000
#	sysctl -w net.core.rmem_max=2129920000
#	sysctl -w net.unix.max_dgram_qlen=2147000000

uninstall:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir uninstall || exit $$?; \
	done

$(SUBDIRS):
	$(MAKE) -C $@
	