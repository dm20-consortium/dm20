.PHONY: all

SUBDIRS := $(shell find . -maxdepth 1 -type d ! -name '.')

all:
	@for dir in $(SUBDIRS); do \
		if [ -f "$$dir/makefile" ] || [ -f "$$dir/Makefile" ]; then \
			$(MAKE) -C $$dir; \
		fi; \
	done

clean:
	@for dir in $(SUBDIRS); do \
		if [ -f "$$dir/makefile" ] || [ -f "$$dir/Makefile" ]; then \
			$(MAKE) -C $$dir clean; \
		fi; \
	done

install:
	@:
uninstall:
	@:
