
all: install

%: builder
	make -C builder -f Makefile.arduino $@

builder:
	git submodule update --init --recursive builder
