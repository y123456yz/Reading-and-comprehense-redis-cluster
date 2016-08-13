# Clear out all vpaths, then set just one (default vpath) for the main build
# directory.
vpath
vpath % .

# Clear the default suffixes, so that built-in rules are not used.
.SUFFIXES :

SHELL := /bin/sh

CC := gcc

# Configuration parameters.
DESTDIR =
BINDIR := $(DESTDIR)/usr/local/bin
INCLUDEDIR := $(DESTDIR)/usr/local/include
LIBDIR := $(DESTDIR)/usr/local/lib
DATADIR := $(DESTDIR)/usr/local/share
MANDIR := $(DESTDIR)/usr/local/share/man
srcroot := 
objroot := 
abs_srcroot := /root/yangyazhou/redis/Reading-and-comprehense-redis-2.9.11-master/redis-3.0/deps/jemalloc/
abs_objroot := /root/yangyazhou/redis/Reading-and-comprehense-redis-2.9.11-master/redis-3.0/deps/jemalloc/

# Build parameters.
CPPFLAGS :=  -D_GNU_SOURCE -D_REENTRANT -I$(srcroot)include -I$(objroot)include
CFLAGS := -std=gnu99 -Wall -pipe -g3 -O3 -funroll-loops  -fvisibility=hidden
LDFLAGS := 
EXTRA_LDFLAGS := 
LIBS :=  -lm -lpthread
RPATH_EXTRA := 
SO := so
IMPORTLIB := so
O := o
A := a
EXE := 
LIBPREFIX := lib
REV := 1
install_suffix := 
ABI := elf
XSLTPROC := /usr/bin/xsltproc
AUTOCONF := /usr/bin/autoconf
_RPATH = -Wl,-rpath,$(1)
RPATH = $(if $(1),$(call _RPATH,$(1)))
cfghdrs_in := include/jemalloc/jemalloc_defs.h.in include/jemalloc/internal/size_classes.sh
cfghdrs_out := include/jemalloc/jemalloc_defs.h include/jemalloc/internal/size_classes.h
cfgoutputs_in := Makefile.in doc/html.xsl.in doc/manpages.xsl.in doc/jemalloc.xml.in include/jemalloc/jemalloc.h.in include/jemalloc/internal/jemalloc_internal.h.in test/jemalloc_test.h.in
cfgoutputs_out := Makefile doc/html.xsl doc/manpages.xsl doc/jemalloc.xml include/jemalloc/jemalloc.h include/jemalloc/internal/jemalloc_internal.h test/jemalloc_test.h
enable_autogen := 0
enable_experimental := 1
DSO_LDFLAGS = -shared -Wl,-soname,$(@F)
SOREV = so.1
PIC_CFLAGS = -fPIC -DPIC
CTARGET = -o $@
LDTARGET = -o $@
MKLIB = ar crus $@
CC_MM = 1

ifeq (macho, $(ABI))
TEST_LIBRARY_PATH := DYLD_FALLBACK_LIBRARY_PATH="$(objroot)lib"
else
ifeq (pecoff, $(ABI))
TEST_LIBRARY_PATH := PATH="$(PATH):$(objroot)lib"
else
TEST_LIBRARY_PATH :=
endif
endif

LIBJEMALLOC := $(LIBPREFIX)jemalloc$(install_suffix)

# Lists of files.
BINS := $(srcroot)bin/pprof $(objroot)bin/jemalloc.sh
CHDRS := $(objroot)include/jemalloc/jemalloc$(install_suffix).h \
	$(objroot)include/jemalloc/jemalloc_defs$(install_suffix).h
CSRCS := $(srcroot)src/jemalloc.c $(srcroot)src/arena.c $(srcroot)src/atomic.c \
	$(srcroot)src/base.c $(srcroot)src/bitmap.c $(srcroot)src/chunk.c \
	$(srcroot)src/chunk_dss.c $(srcroot)src/chunk_mmap.c \
	$(srcroot)src/ckh.c $(srcroot)src/ctl.c $(srcroot)src/extent.c \
	$(srcroot)src/hash.c $(srcroot)src/huge.c $(srcroot)src/mb.c \
	$(srcroot)src/mutex.c $(srcroot)src/prof.c $(srcroot)src/quarantine.c \
	$(srcroot)src/rtree.c $(srcroot)src/stats.c $(srcroot)src/tcache.c \
	$(srcroot)src/util.c $(srcroot)src/tsd.c
ifeq (macho, $(ABI))
CSRCS += $(srcroot)src/zone.c
endif
ifeq ($(IMPORTLIB),$(SO))
STATIC_LIBS := $(objroot)lib/$(LIBJEMALLOC).$(A)
endif
ifdef PIC_CFLAGS
STATIC_LIBS += $(objroot)lib/$(LIBJEMALLOC)_pic.$(A)
else
STATIC_LIBS += $(objroot)lib/$(LIBJEMALLOC)_s.$(A)
endif
DSOS := $(objroot)lib/$(LIBJEMALLOC).$(SOREV)
ifneq ($(SOREV),$(SO))
DSOS += $(objroot)lib/$(LIBJEMALLOC).$(SO)
endif
MAN3 := $(objroot)doc/jemalloc$(install_suffix).3
DOCS_XML := $(objroot)doc/jemalloc$(install_suffix).xml
DOCS_HTML := $(DOCS_XML:$(objroot)%.xml=$(srcroot)%.html)
DOCS_MAN3 := $(DOCS_XML:$(objroot)%.xml=$(srcroot)%.3)
DOCS := $(DOCS_HTML) $(DOCS_MAN3)
CTESTS := $(srcroot)test/aligned_alloc.c $(srcroot)test/allocated.c \
	$(srcroot)test/ALLOCM_ARENA.c $(srcroot)test/bitmap.c \
	$(srcroot)test/mremap.c $(srcroot)test/posix_memalign.c \
	$(srcroot)test/thread_arena.c $(srcroot)test/thread_tcache_enabled.c
ifeq ($(enable_experimental), 1)
CTESTS += $(srcroot)test/allocm.c $(srcroot)test/rallocm.c
endif

COBJS := $(CSRCS:$(srcroot)%.c=$(objroot)%.$(O))
CPICOBJS := $(CSRCS:$(srcroot)%.c=$(objroot)%.pic.$(O))
CTESTOBJS := $(CTESTS:$(srcroot)%.c=$(objroot)%.$(O))

.PHONY: all dist doc_html doc_man doc
.PHONY: install_bin install_include install_lib
.PHONY: install_html install_man install_doc install
.PHONY: tests check clean distclean relclean

.SECONDARY : $(CTESTOBJS)

# Default target.
all: build

dist: build_doc

$(srcroot)doc/%.html : $(objroot)doc/%.xml $(srcroot)doc/stylesheet.xsl $(objroot)doc/html.xsl
	$(XSLTPROC) -o $@ $(objroot)doc/html.xsl $<

$(srcroot)doc/%.3 : $(objroot)doc/%.xml $(srcroot)doc/stylesheet.xsl $(objroot)doc/manpages.xsl
	$(XSLTPROC) -o $@ $(objroot)doc/manpages.xsl $<

build_doc_html: $(DOCS_HTML)
build_doc_man: $(DOCS_MAN3)
build_doc: $(DOCS)

#
# Include generated dependency files.
#
ifdef CC_MM
-include $(COBJS:%.$(O)=%.d)
-include $(CPICOBJS:%.$(O)=%.d)
-include $(CTESTOBJS:%.$(O)=%.d)
endif

$(COBJS): $(objroot)src/%.$(O): $(srcroot)src/%.c
$(CPICOBJS): $(objroot)src/%.pic.$(O): $(srcroot)src/%.c
$(CPICOBJS): CFLAGS += $(PIC_CFLAGS)
$(CTESTOBJS): $(objroot)test/%.$(O): $(srcroot)test/%.c
$(CTESTOBJS): CPPFLAGS += -I$(objroot)test
ifneq ($(IMPORTLIB),$(SO))
$(COBJS): CPPFLAGS += -DDLLEXPORT
endif

ifndef CC_MM
# Dependencies
HEADER_DIRS = $(srcroot)include/jemalloc/internal \
	$(objroot)include/jemalloc $(objroot)include/jemalloc/internal
HEADERS = $(wildcard $(foreach dir,$(HEADER_DIRS),$(dir)/*.h))
$(COBJS) $(CPICOBJS) $(CTESTOBJS): $(HEADERS)
$(CTESTOBJS): $(objroot)test/jemalloc_test.h
endif

$(COBJS) $(CPICOBJS) $(CTESTOBJS): %.$(O):
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $(CPPFLAGS) $(CTARGET) $<
ifdef CC_MM
	@$(CC) -MM $(CPPFLAGS) -MT $@ -o $(@:%.$(O)=%.d) $<
endif

ifneq ($(SOREV),$(SO))
%.$(SO) : %.$(SOREV)
	@mkdir -p $(@D)
	ln -sf $(<F) $@
endif

$(objroot)lib/$(LIBJEMALLOC).$(SOREV) : $(if $(PIC_CFLAGS),$(CPICOBJS),$(COBJS))
	@mkdir -p $(@D)
	$(CC) $(DSO_LDFLAGS) $(call RPATH,$(RPATH_EXTRA)) $(LDTARGET) $+ $(LDFLAGS) $(LIBS) $(EXTRA_LDFLAGS)

$(objroot)lib/$(LIBJEMALLOC)_pic.$(A) : $(CPICOBJS)
$(objroot)lib/$(LIBJEMALLOC).$(A) : $(COBJS)
$(objroot)lib/$(LIBJEMALLOC)_s.$(A) : $(COBJS)

$(STATIC_LIBS):
	@mkdir -p $(@D)
	$(MKLIB) $+

$(objroot)test/bitmap$(EXE): $(objroot)src/bitmap.$(O)

$(objroot)test/%$(EXE): $(objroot)test/%.$(O) $(objroot)src/util.$(O) $(DSOS)
	@mkdir -p $(@D)
	$(CC) $(LDTARGET) $(filter %.$(O),$^) $(call RPATH,$(objroot)lib) $(objroot)lib/$(LIBJEMALLOC).$(IMPORTLIB) $(filter -lpthread,$(LIBS)) $(EXTRA_LDFLAGS)

build_lib_shared: $(DSOS)
build_lib_static: $(STATIC_LIBS)
build: build_lib_shared build_lib_static

install_bin:
	install -d $(BINDIR)
	@for b in $(BINS); do \
	echo "install -m 755 $$b $(BINDIR)"; \
	install -m 755 $$b $(BINDIR); \
done

install_include:
	install -d $(INCLUDEDIR)/jemalloc
	@for h in $(CHDRS); do \
	echo "install -m 644 $$h $(INCLUDEDIR)/jemalloc"; \
	install -m 644 $$h $(INCLUDEDIR)/jemalloc; \
done

install_lib_shared: $(DSOS)
	install -d $(LIBDIR)
	install -m 755 $(objroot)lib/$(LIBJEMALLOC).$(SOREV) $(LIBDIR)
ifneq ($(SOREV),$(SO))
	ln -sf $(LIBJEMALLOC).$(SOREV) $(LIBDIR)/$(LIBJEMALLOC).$(SO)
endif

install_lib_static: $(STATIC_LIBS)
	install -d $(LIBDIR)
	@for l in $(STATIC_LIBS); do \
	echo "install -m 755 $$l $(LIBDIR)"; \
	install -m 755 $$l $(LIBDIR); \
done

install_lib: install_lib_shared install_lib_static

install_doc_html:
	install -d $(DATADIR)/doc/jemalloc$(install_suffix)
	@for d in $(DOCS_HTML); do \
	echo "install -m 644 $$d $(DATADIR)/doc/jemalloc$(install_suffix)"; \
	install -m 644 $$d $(DATADIR)/doc/jemalloc$(install_suffix); \
done

install_doc_man:
	install -d $(MANDIR)/man3
	@for d in $(DOCS_MAN3); do \
	echo "install -m 644 $$d $(MANDIR)/man3"; \
	install -m 644 $$d $(MANDIR)/man3; \
done

install_doc: install_doc_html install_doc_man

install: install_bin install_include install_lib install_doc

tests: $(CTESTS:$(srcroot)%.c=$(objroot)%$(EXE))

check: tests
	@mkdir -p $(objroot)test
	@$(SHELL) -c 'total=0; \
		failures=0; \
		echo "========================================="; \
		for t in $(CTESTS:$(srcroot)%.c=$(objroot)%); do \
			total=`expr $$total + 1`; \
			/bin/echo -n "$${t} ... "; \
			$(TEST_LIBRARY_PATH) $${t}$(EXE) $(abs_srcroot) \
			  $(abs_objroot) > $(objroot)$${t}.out 2>&1; \
			if test -e "$(srcroot)$${t}.exp"; then \
				diff -w -u $(srcroot)$${t}.exp \
				  $(objroot)$${t}.out >/dev/null 2>&1; \
				fail=$$?; \
				if test "$${fail}" -eq "1" ; then \
					failures=`expr $${failures} + 1`; \
					echo "*** FAIL ***"; \
				else \
					echo "pass"; \
				fi; \
			else \
				echo "*** FAIL *** (.exp file is missing)"; \
				failures=`expr $${failures} + 1`; \
			fi; \
		done; \
		echo "========================================="; \
		echo "Failures: $${failures}/$${total}"'

clean:
	rm -f $(COBJS)
	rm -f $(CPICOBJS)
	rm -f $(COBJS:%.$(O)=%.d)
	rm -f $(CPICOBJS:%.$(O)=%.d)
	rm -f $(CTESTOBJS:%.$(O)=%$(EXE))
	rm -f $(CTESTOBJS)
	rm -f $(CTESTOBJS:%.$(O)=%.d)
	rm -f $(CTESTOBJS:%.$(O)=%.out)
	rm -f $(DSOS) $(STATIC_LIBS)

distclean: clean
	rm -rf $(objroot)autom4te.cache
	rm -f $(objroot)config.log
	rm -f $(objroot)config.status
	rm -f $(objroot)config.stamp
	rm -f $(cfghdrs_out)
	rm -f $(cfgoutputs_out)

relclean: distclean
	rm -f $(objroot)configure
	rm -f $(srcroot)VERSION
	rm -f $(DOCS_HTML)
	rm -f $(DOCS_MAN3)

#===============================================================================
# Re-configuration rules.

ifeq ($(enable_autogen), 1)
$(srcroot)configure : $(srcroot)configure.ac
	cd ./$(srcroot) && $(AUTOCONF)

$(objroot)config.status : $(srcroot)configure
	./$(objroot)config.status --recheck

$(srcroot)config.stamp.in : $(srcroot)configure.ac
	echo stamp > $(srcroot)config.stamp.in

$(objroot)config.stamp : $(cfgoutputs_in) $(cfghdrs_in) $(srcroot)configure
	./$(objroot)config.status
	@touch $@

# There must be some action in order for make to re-read Makefile when it is
# out of date.
$(cfgoutputs_out) $(cfghdrs_out) : $(objroot)config.stamp
	@true
endif
