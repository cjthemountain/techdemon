# Configuration section

AR := ar
CC := gcc
CFLAGS := -g -O2 -Icaptouch/mgc3130/api/include
MKDIR := mkdir -p

APPS :=  techdemon
FRAMEWORKS :=  framework_dyn 

BUILDDIR := build

# Configuration of the individual products

framework_dyn_SRC_FILES := core.c flash.c fw_version.c rtc.c stream.c \
                       io/cdcbridge_linux.c io/serial.c \
                       dynamic/depr_stream.c dynamic/dynamic.c
framework_dyn_SRC_PATH  := ./captouch/mgc3130/api/src
framework_dyn_BUILDDIR  := $(BUILDDIR)/framework/dynamic
framework_dyn_FILENAME  := libgestic.so
framework_dyn_CFLAGS    := -fpic -DGESTIC_API_EXPORT -DGESTIC_API_DYNAMIC
framework_dyn_LDFLAGS   := -shared

techdemon_SRC_FILES := techdemon.c
techdemon_SRC_PATH := techdemon
techdemon_BUILDDIR  := $(BUILDDIR)/techdemon
techdemon_FILENAME  := techdemon
techdemon_CFLAGS    := -DGESTIC_API_DYNAMIC
techdemon_LDFLAGS   := -L$(BUILDDIR)/bin -lgestic -Wl,-rpath,\$$ORIGIN

.PHONY: all framework apps clean

all: framework apps

framework: $(FRAMEWORKS)

apps: $(APPS)

# Macro for creating object files and their dependency information
define make-object
$3: $2
	$(MKDIR) $$(dir $$@)
	$(CC) $(CFLAGS) $($1_CFLAGS) -MM -MF $$(patsubst %.o,%.d,$$@) $$<
	$(CC) $(CFLAGS) $($1_CFLAGS) -c -o $$@ $$<
endef

# Macro for creating whole products
define make-product
$1_SOURCES := $$(patsubst %,$$($1_SRC_PATH)/%,$$($1_SRC_FILES))
$1_OBJECTS := $$(patsubst %.c,$$($1_BUILDDIR)/%.o,$$($1_SRC_FILES))
$1_DEPENDS := $$(patsubst %.c,$$($1_BUILDDIR)/%.d,$$($1_SRC_FILES))
CLEAN += $$($1_DEPENDS) $$($1_OBJECTS) $$($1_BUILDDIR)/$$($1_FILENAME)
DEPENDS += $$($1_DEPENDS)
$$(foreach src,$$($1_SRC_FILES),$$(eval $$(call make-object,$1,$$(patsubst %,$$($1_SRC_PATH)/%,$$(src)),$$(patsubst %.c,$$($1_BUILDDIR)/%.o,$$(src)))))
$1: $(BUILDDIR)/bin/$$($1_FILENAME)
ifeq ($$(suffix $$($1_FILENAME)),.a)
$(BUILDDIR)/bin/$$($1_FILENAME): $$($1_OBJECTS)
	$(MKDIR) $(BUILDDIR)/bin
	$(AR) rcs $$@ $$^
else
$(BUILDDIR)/bin/$$($1_FILENAME): $$($1_OBJECTS)
	$(MKDIR) $(BUILDDIR)/bin
	$(CC) -o $$@ $$^ $$(LDFLAGS) $$($1_LDFLAGS)
endif
endef

$(foreach framework,$(FRAMEWORKS),$(eval $(call make-product,$(framework))))
$(foreach app,$(APPS),$(eval $(call make-product,$(app))))

-include $(DEPENDS)

clean:
	rm -f $(CLEAN)
