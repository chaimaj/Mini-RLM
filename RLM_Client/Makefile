#
# Makefile for development of game using Dragonfly
#
# 'make' to build executable
# 'make depend' to generate new dependency list
# 'make clean' to remove all constructed files
## Variables of interest:
#   GAMESRC is the source code files for the game being developed
#   GAME is the game main() source
#   DEBUG can be set to -DDEBUG to enable various debug flags
#

CC= g++
PLATFORM= -DCYGWIN
LDFLAGS= -lncurses -lrt -lm
#INCPATH= include
#LIBRARY=libdragonfly.a
ENGSRC= \
    Frame.cpp\
    Sprite.cpp\
    Position.cpp\
    utility.cpp\
    
GAME= main.cpp
EXECUTABLE= main
OBJECTS = $(ENGSRC:.cpp=.o)
	
#$(LIBRARY): $(OBJECTS) Makefile
#	ar rcs $(LIBRARY) $(OBJECTS)

all: $(EXECUTABLE) Makefile
 
$(EXECUTABLE):	$(OBJECTS)  $(GAME) $(ENGSRC) 
	$(CC) $(GAME) $(OBJECTS) -o $@ $(LDFLAGS)
 
.cpp.o:	
	$(CC) -c $(DEBUG)   $(PLATFORM) $< -o $@
#debug:	$(CC)	$(CXXFLAGS)	$(OBJECTS)	-o	$@	$(LDFLAGS)
 
clean:
	rm -rf	$(OBJECTS)	$(EXECUTABLE)	core dragonfly.log Makefile.bak *~
 
depend:	
	makedepend *.cpp 2> /dev/null

# DO NOT DELETE

Frame.o: Frame.h
main.o: /usr/include/unistd.h /usr/include/sys/unistd.h /usr/include/_ansi.h
main.o: /usr/include/newlib.h /usr/include/sys/config.h
main.o: /usr/include/machine/ieeefp.h /usr/include/sys/features.h
main.o: /usr/include/sys/cdefs.h /usr/include/machine/_default_types.h
main.o: /usr/include/sys/types.h /usr/include/machine/_types.h
main.o: /usr/include/sys/_types.h /usr/include/sys/lock.h
main.o: /usr/include/machine/types.h /usr/include/pthread.h
main.o: /usr/include/signal.h /usr/include/sys/signal.h /usr/include/sched.h
main.o: /usr/include/sys/sched.h /usr/include/time.h /usr/include/sys/reent.h
main.o: /usr/include/machine/time.h /usr/include/stdlib.h
main.o: /usr/include/machine/stdlib.h /usr/include/alloca.h
main.o: /usr/include/stdio.h /usr/include/sys/stdio.h /usr/include/string.h
main.o: /usr/include/sys/string.h /usr/include/netdb.h
main.o: /usr/include/inttypes.h /usr/include/stdint.h
main.o: /usr/include/bits/wordsize.h /usr/include/sys/socket.h
main.o: /usr/include/features.h /usr/include/cygwin/socket.h
main.o: /usr/include/asm/socket.h /usr/include/cygwin/if.h
main.o: /usr/include/cygwin/sockios.h /usr/include/sys/uio.h
main.o: /usr/include/sys/time.h /usr/include/netinet/in.h
main.o: /usr/include/cygwin/in.h /usr/include/asm/byteorder.h
main.o: /usr/include/bits/endian.h /usr/include/cygwin/in6.h
main.o: /usr/include/arpa/inet.h /usr/include/curses.h
main.o: /usr/include/ncurses_dll.h /usr/include/unctrl.h utility.h Position.h
main.o: Frame.h Sprite.h
Position.o: Position.h
Sprite.o: Sprite.h Frame.h
utility.o: /usr/include/stdio.h /usr/include/_ansi.h /usr/include/newlib.h
utility.o: /usr/include/sys/config.h /usr/include/machine/ieeefp.h
utility.o: /usr/include/sys/features.h /usr/include/sys/cdefs.h
utility.o: /usr/include/machine/_default_types.h /usr/include/sys/reent.h
utility.o: /usr/include/sys/_types.h /usr/include/machine/_types.h
utility.o: /usr/include/sys/lock.h /usr/include/sys/types.h
utility.o: /usr/include/machine/types.h /usr/include/sys/stdio.h
utility.o: /usr/include/time.h /usr/include/machine/time.h
utility.o: /usr/include/string.h /usr/include/sys/string.h utility.h
utility.o: Position.h
