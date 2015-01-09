STD=c11
override CFLAGS+=-Werror -Wall -g -fPIC -O2 -DNDEBUG -ftrapv -Wfloat-equal -Wundef -Wwrite-strings -Wconversion -Wuninitialized -pedantic -std=$(STD)
PREFIX=/usr/bin/
BUILDDIR=bin/
DEBUGDIR=debug/
LIBS=

CC=cc

TARGET=decode
SOURCES=$(wildcard src/*.c)

#Makes everything
all:
	mkdir -p $(BUILDDIR)  2> /dev/null
	$(CC) $(CFLAGS) $(LIBS) $(SOURCES) -o $(BUILDDIR)$(TARGET)

#Uses picky extensions and makes everything(Extensions may break compiling)
dev:
	make all CFLAGS+="-Wshadow -Wunreachable-code -Wswitch-enum -Wswitch-default -Wcast-align -Winit-self -Wpointer-arith"

#Run the preprocessor only
pp:
	mkdir -p $(DEBUGDIR) 2> /dev/null
	$(CC) -E $(LIBS) $(SOURCES) > $(DEBUGDIR)preprocessed.i

#Run the preprocessor and asm generator
asm:
	mkdir -p $(DEBUGDIR) 2> /dev/null
	$(CC) -S $(LIBS) $(SOURCES)
	mv *.s $(DEBUGDIR)

#Create object code
obj:
	mkdir -p $(DEBUGDIR) 2> /dev/null
	$(CC) -c $(LIBS) $(SOURCES)
	mv *.o $(DEBUGDIR)

#Make all diagnostics files
diagnostics: pp asm obj

#Cleans directory(no uninstall!)
clean: 
	rm -rf $(BUILDDIR) $(DEBUGDIR)

#Installs into specified(or default) directory
install:
	install -d $(PREFIX)$(TARGET)
	install $(BUILDDIR)$(TARGET) $(PREFIX)$(TARGET)

#Uninstalls from specified(or default)directory
uninstall: 
	rm -rf $(PREFIX)$(TARGET)
