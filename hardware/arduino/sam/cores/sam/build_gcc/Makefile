ifeq ("$(VARIANTS)", "")
VARIANTS = arduino_due_x
#VARIANTS = sam3u_ek sam3s_ek sam3x_ek arduino_due_u
endif

SUBMAKE_OPTIONS=--no-builtin-rules --no-builtin-variables

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------

all: $(VARIANTS)

.PHONY: sam3s_ek 
sam3s_ek:
	@echo --- Making sam3s_ek
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_sam3s_ek.mk
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_sam3s_ek.mk

.PHONY: sam3u_ek
sam3u_ek:
	@echo --- Making sam3u_ek
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_sam3u_ek.mk
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_sam3u_ek.mk

.PHONY: sam3x_ek
sam3x_ek:
	@echo --- Making sam3x_ek
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_sam3x_ek.mk
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_sam3x_ek.mk

.PHONY: arduino_due_u
arduino_due_u:
	@echo --- Making arduino_due_u
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_arduino_due_u.mk
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_arduino_due_u.mk

.PHONY: arduino_due_x
arduino_due_x:
	@echo --- Making arduino_due_x
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_arduino_due_x.mk
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_arduino_due_x.mk

.PHONY: clean
clean:
	@echo --- Cleaning sam3s_ek
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_sam3s_ek.mk $@
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_sam3s_ek.mk $@

	@echo --- Cleaning sam3u_ek
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_sam3u_ek.mk $@
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_sam3u_ek.mk $@

	@echo --- Cleaning sam3x_ek
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_sam3x_ek.mk $@
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_sam3x_ek.mk $@

	@echo --- Cleaning arduino_due_u
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_arduino_due_u.mk $@
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_arduino_due_u.mk $@

	@echo --- Cleaning arduino_due_x
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_arduino_due_x.mk $@
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_arduino_due_x.mk $@


