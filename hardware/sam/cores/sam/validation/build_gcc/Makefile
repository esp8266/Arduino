SUBMAKE_OPTIONS=--no-builtin-rules --no-builtin-variables

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------

all: test

.PHONY: test
test:
	@echo --- Making test
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f test.mk

.PHONY: clean
clean:
	@echo --- Cleaning test
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f test.mk $@

.PHONY: debug
debug:
	@echo --- Debugging test
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f test.mk $@

