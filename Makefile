#TOOLS
BIGLOO = bigloo
BGLAFILE = bglafile
BGLTAGS = bgltags
LD = ld
AR = ar 
RANLIB = ranlib
CC = gcc

BIGLOO_VERSION := $(shell $(BIGLOO) -eval "(print *bigloo-version*) (exit 0)" -q)

BIGLOOLIBDIR := $(shell $(BIGLOO) -eval "(print *default-lib-dir*) (exit 0)" -q)

VERSION = 0.1

#Bigloo Flags
BHEAPFLAGS = -unsafe -q -mkaddheap -mkaddlib -v2 -heap-library biglooODBC -I src/Llib

BCOMMONFLAGS = -mkaddlib -fsharing -q \
               -copt '$(CCOMMONFLAGS)' -I src/Llib

BSAFEFLAGS = $(BCOMMONFLAGS) -cg -O3 -g 

BUNSAFEFLAGS = $(BCOMMONFLAGS) -O4 -unsafe

#C Flags
CCOMMONFLAGS = -fPIC -I src/Clib -I $(BIGLOOLIBDIR)

# directory variables
OBJDIR = objs
DISTDIR = dist

# sources
BIGLOOSRCS = src/Llib/odbc.scm src/Llib/dbi.scm
HEAPSRC = src/Misc/make_lib.scm


# object files

_SRCSWOUTDIR = $(foreach src, $(BIGLOOSRCS), $(notdir $(src)))
_BASEOBJSWOUTDIR = $(_SRCSWOUTDIR:%.scm=%)

SAFEOBJECTS = $(_BASEOBJSWOUTDIR:%=$(OBJDIR)/%_s.o)

SAFEOBJECTS += $(OBJDIR)/codbc_s.o


UNSAFEOBJECTS = $(_BASEOBJSWOUTDIR:%=$(OBJDIR)/%_u.o)

UNSAFEOBJECTS += $(OBJDIR)/codbc_u.o


_ESRCSWOUTDIR = $(foreach src, $(HEAPSRC), $(notdir $(src)))
_EBASEOBJSWOUTDIR = $(_ESRCSWOUTDIR:%.scm=%)

ESAFEOBJECTS = $(_EBASEOBJSWOUTDIR:%=$(OBJDIR)/%_s.o)

EUNSAFEOBJECTS = $(_EBASEOBJSWOUTDIR:%=$(OBJDIR)/%_u.o)


.SUFFIXES:

$(OBJDIR)/%_s.o:src/Llib/%.scm
	$(BIGLOO) -c $(BSAFEFLAGS) -o $@ $^

$(OBJDIR)/%_u.o:src/Llib/%.scm
	$(BIGLOO) -c $(BUNSAFEFLAGS) -o $@ $^



#targets

all: .afile .etags lib recette/testodbc


.afile: $(BIGLOOSRCS)
	$(BGLAFILE) -o $@  $^

.etags: $(BIGLOOSRCS)
	$(BGLTAGS) -o $@ $^

heap: $(DISTDIR)/biglooODBC.heap

$(DISTDIR)/biglooODBC.heap: $(DISTDIR) $(HEAPSRC)
	$(BIGLOO) $(BHEAPFLAGS) $(HEAPSRC)  -addheap $@

$(OBJDIR)/make_lib_s.o:src/Misc/make_lib.scm
	$(BIGLOO) -c $(BSAFEFLAGS) -o $@ $^

$(OBJDIR)/make_lib_u.o:src/Misc/make_lib.scm
	$(BIGLOO) -c $(BUNSAFEFLAGS) -o $@ $^

$(OBJDIR)/codbc_s.o:src/Clib/codbc.c
	$(CC) -O2 -c -o $@ $(CCOMMONFLAGS) $^ 


$(OBJDIR)/codbc_u.o:src/Clib/codbc.c
	$(CC) -O2 -c -o $@ $(CCOMMONFLAGS) $^ 

lib: init heap lib_s lib_u lib_s.a lib_u.a lib_es lib_eu

init: $(DISTDIR)/biglooODBC.init

$(DISTDIR)/biglooODBC.init : src/Misc/biglooODBC.init $(DISTDIR)
	cp $< $(DISTDIR)/


lib_s: $(OBJDIR) $(DISTDIR) $(SAFEOBJECTS)
	$(LD) -G -o $(DISTDIR)/libbiglooODBC_s-$(VERSION).so $(SAFEOBJECTS) -lc -lm

lib_s.a : $(OBJDIR) $(DISTDIR) $(SAFEOBJECTS)
	$(AR) qcv $(DISTDIR)/libbiglooODBC_s-$(VERSION).a $(SAFEOBJECTS);\
	$(RANLIB) $(DISTDIR)/libbiglooODBC_s-$(VERSION).a

lib_es : $(OBJDIR) $(DISTDIR) $(ESAFEOBJECTS)
	$(LD) -G -o $(DISTDIR)/libbiglooODBC_es-$(VERSION).so $(ESAFEOBJECTS) -lc -lm

lib_u : $(OBJDIR) $(DISTDIR) $(UNSAFEOBJECTS)
	$(LD) -G -o $(DISTDIR)/libbiglooODBC_u-$(VERSION).so $(UNSAFEOBJECTS) -lc -lm

lib_u.a : $(OBJDIR) $(DISTDIR) $(UNSAFEOBJECTS)
	$(AR) qcv $(DISTDIR)/libbiglooODBC_u-$(VERSION).a $(SAFEOBJECTS);\
	$(RANLIB) $(DISTDIR)/libbiglooODBC_u-$(VERSION).a

lib_eu : $(OBJDIR) $(DISTDIR) $(EUNSAFEOBJECTS)
	$(LD) -G -o $(DISTDIR)/libbiglooODBC_eu-$(VERSION).so $(EUNSAFEOBJECTS) -lc -lm


recette/testodbc : recette/testodbc.scm lib 
	$(BIGLOO) -static-all-bigloo $(BUNSAFEFLAGS) -L $(DISTDIR) -o $@ $< 


$(OBJDIR):
	mkdir $@

$(DISTDIR):
	mkdir $@

clean:
	rm -f -r $(OBJDIR);\
	rm -f -r $(DISTDIR);\
	rm -f biglooODBC.heap;\
	rm -f biglooODBC.init;\
	rm -f recette/testodbc



