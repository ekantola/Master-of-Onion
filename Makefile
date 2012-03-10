# Master of Onion makefile

VERSION = 1.0.0

CXXFLAGS = -Wall -O -g
LDFLAGS = $(shell allegro-config --libs) -lSDL -lXxf86vm -lXxf86dga -lpthread -lX11 -lXext -ldl

CXX = g++

OUTDIR = files
BINARY = moon
BINARY_STATIC = $(BINARY)-static
KEYTEST = keytest
KEYTEST_STATIC = $(KEYTEST)-static
HEADERS = *.h
DOCS = HISTORY LICENSE README index.html
LEVELS = *.lev
IMAGES = *.pcx
CONFFILE = moon.cfg

BINPKG = $(OUTDIR)/moon-$(VERSION)-bin.tar.bz2
BINPKG_STATIC = $(OUTDIR)/moon-$(VERSION)-bin-static.tar.bz2
SRCPKG = $(OUTDIR)/moon-$(VERSION)-src.tar.bz2

CONFIG = config.o
EQUIPMENT = equipment.o
KEYNAMES = keynames.o
LEVEL = level.o
MISC = misc.o
MOON = moon.o
PLAYERS = players.o
SHIPS = ships.o
SHOTS = shots.o

OBJS = $(CONFIG) $(EQUIPMENT) $(KEYNAMES) $(LEVEL) $(MISC) $(MOON) $(PLAYERS) \
       $(SHIPS) $(SHOTS)
KEYTEST_OBJ = keytest.o

.PHONY: all clean binpkg binpkg-static srcpkg
.PRECIOUS: $(OBJS) $(BINARY)

all: $(OBJS) $(KEYTEST_OBJS) $(BINARY) $(KEYTEST)

clean:
	$(RM) $(OBJS) $(KEYTEST_OBJ) $(BINARY) $(KEYTEST) \
              $(BINARY_STATIC) $(KEYTEST_STATIC)

binpkg: $(OUTDIR) $(BINPKG)
binpkg-static: $(OUTDIR) $(BINPKG_STATIC)
srcpkg: $(OUTDIR) $(SRCPKG)


$(BINPKG): $(BINARY) $(KEYTEST) $(LEVELS) $(IMAGES) $(DOCS) $(CONFFILE)
	tar -cjf $@ $^

$(BINPKG_STATIC): $(BINARY_STATIC) $(KEYTEST_STATIC) $(LEVELS) $(IMAGES) \
                  $(DOCS) $(CONFFILE)
	tar -cjf $@ $^

$(SRCPKG): $(subst .o,.cc,$(OBJS) $(KEYTEST_OBJ)) $(HEADERS) $(LEVELS) \
           $(IMAGES) $(DOCS) $(CONFFILE) Makefile
	tar -cjf $@ $^

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(BINARY): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(BINARY_STATIC): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS) -static

$(KEYTEST): $(KEYTEST_OBJ) $(KEYNAMES)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(KEYTEST_STATIC): $(KEYTEST_OBJ) $(KEYNAMES)
	$(CXX) -o $@ $^ $(LDFLAGS) -static


$(CONFIG): %.o: %.cc %.h
$(EQUIPMENT): %.o: %.cc %.h players.h ships.h shots.h internal.h
$(KEYNAMES): %.o: %.cc %.h
$(LEVEL): %.o: %.cc %.h internal.h
$(MISC): %.o: %.cc %.h internal.h
$(MOON): %.o: %.cc colors.h config.h equipment.h keynames.h level.h misc.h \
                   players.h ships.h internal.h
$(PLAYERS): %.o: %.cc %.h colors.h equipment.h ships.h internal.h
$(SHOTS): %.o: %.cc %.h colors.h equipment.h misc.h players.h ships.h internal.h
$(SHIPS): %.o: %.cc %.h colors.h equipment.h misc.h players.h internal.h
$(KEYTEST_OBJ): %.o: %.cc internal.h keynames.h
