SUBMAKE_OPTIONS=--no-builtin-rules --no-builtin-variables

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------

all: prod_test_due

.PHONY: prod_test_due
prod_test_due:
	@echo --- Making prod_test_due
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f prod_test_due.mk

.PHONY: clean
clean:
	@echo --- Cleaning prod_test_due
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f prod_test_due.mk $@

.PHONY: debug
debug:
	@echo --- Debugging prod_test_due
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f prod_test_due.mk $@

