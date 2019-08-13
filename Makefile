
CC ?= gcc
WINDRES ?= windres
STRIP ?= strip
RM ?= rm -f

TARGET = q2textures
LIB = -lphysfs

CCX = i686-w64-mingw32-gcc
LIBX = /usr/i686-w64-mingw32/sys-root/mingw/lib

# Define V=1 to show command line.
ifdef V
    Q :=
    E := @true
else
    Q := @
    E := @echo
endif

all: 
	$(E) [CC] $(TARGET)
	$(Q)$(CC) -std=c99 -o $(TARGET) $(TARGET).c $(LIB)

win:
	$(E) [CC] $(TARGET).exe
	$(Q)$(CCX) -std=c99 -o $(TARGET).exe $(TARGET).c -static -static-libgcc $(LIBX)/libphysfs.a $(LIBX)/libz.a

strip: 
	$(E) [STRIP]
	$(Q)$(STRIP) $(TARGET)

clean:
	$(E) [CLEAN]
	$(Q)$(RM) $(TARGET) $(TARGET).exe
	
default: all
