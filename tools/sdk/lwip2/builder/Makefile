
ROOT	= .
include $(ROOT)/Makefile.common

####################################
#### list of functions going to IRAM
# esp
IRAM	= glue2esp_err esp2glue_err pbuf_wrapper_get pbuf_wrapper_release glue2esp_linkoutput ethernet_input pbuf_alloc pbuf_free pbuf_ref
# git
IRAM	+= glue2git_err git2glue_err new_linkoutput esp2glue_pbuf_freed esp2glue_alloc_for_recv esp2glue_ethernet_input
# lwip2
IRAM	+= sys_timeout_LWIP2
# sntp
IRAM	+= sntp_time_inc sntp_get_current_timestamp
####################################

$(LWIP_LIB): all

all:
	make -f Makefile.glue-esp	ROOT=$(ROOT) $@
	make -f Makefile.glue		ROOT=$(ROOT) $@
	make -f Makefile.lwip2		ROOT=$(ROOT) $@

section-rename: $(LWIP_LIB_RELEASE)
	@# according to eagle.app.v6.common.ld:
	@# putting symbols into .gnu.linkonce.literal.* instead of (default:).text.*
	@# will eventually move them in iram: section .text.* instead of .irom0.text.*
	@# check this with xtensa-lx106-elf-objdump -t <elfresult> | grep <symbol>
	@for i in $(IRAM); do \
		echo "---- moving $$i in IRAM"; \
		$(OC) --rename-section .text.$$i=.gnu.linkonce.literal.$$i $(LWIP_LIB_RELEASE); \
	done

section-show:
	@for i in $(IRAM); do \
		echo  $$i \\t\\t `$(OD) -t $(LWIP_LIB_RELEASE) | grep " F .* $${i}$$"`; \
	done; true

copy:
	cp $(LWIP_LIB) $(LWIP_LIB_RELEASE)
	rm -rf ../include
	cp -a lwip2-src/src/include ..
	cp -a glue-lwip2/lwip2-git-hash.h glue-lwip2/arch ../include
	(echo "/* this will be overwritten from ../builder/glue-lwip2/lwip2/lwipopts.h */"; cat glue-lwip2/lwipopts.h) > ../include/lwipopts.h
	echo "warning: this directory is re/over/written from ../builder/ files upon lwip2 build" > ../include/README.md

rawinstall: $(LWIP_LIB) copy
install: rawinstall section-rename

clean:
	rm -rf build $(LWIP_LIB)
