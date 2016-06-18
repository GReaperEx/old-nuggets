
SRCD = src
OBJD = obj
OBJS = $(OBJD)/kfront.o \
       $(OBJD)/kmain.o \
       $(OBJD)/BIOSinfo.o \
       $(OBJD)/mmapInfo.o \
       $(OBJD)/TextUI.o \
       $(OBJD)/IDT.o \
       $(OBJD)/OSstring.o \
       $(OBJD)/datetime.o \
       $(OBJD)/OSalloc.o \
       $(OBJD)/exceptions.o \
       $(OBJD)/OStypes.o \
       $(OBJD)/SSE.o \
       $(OBJD)/OSpower.o \
       $(OBJD)/PCI.o \
       $(OBJD)/paging.o \
       $(OBJD)/kInt64.o \
       $(OBJD)/OSprocess.o \
       $(OBJD)/syscalls.o

CFLAGS = -Wall -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -c -O2 -mno-ms-bitfields -s -m32 -fno-strict-aliasing

ifeq ($(ENV), windows)
  LD = i686-elf-ld
  CC = i686-elf-gcc
else
  LD = ld
  CC = gcc
endif

image.sys : $(OBJS)
	$(LD) $(OBJS) -o image.sys -T linker.ld -s -melf_i386

$(OBJD)/syscalls.o : $(SRCD)/OStypes.h $(SRCD)/BIOSinfo.h $(SRCD)/datetime.h $(SRCD)/OSalloc.h $(SRCD)/OSpower.h $(SRCD)/TextUI.h $(SRCD)/OSprocess.h $(SRCD)/syscalls.c
	$(CC) $(SRCD)/syscalls.c -o $(OBJD)/syscalls.o $(CFLAGS)

$(OBJD)/OSprocess.o :  $(SRCD)/OSprocess.h $(SRCD)/OStypes.h $(SRCD)/ports.h $(SRCD)/IDT.h $(SRCD)/OSalloc.h $(SRCD)/paging.h $(SRCD)/OSstring.h $(SRCD)/OSprocess.c
	$(CC) $(SRCD)/OSprocess.c -o $(OBJD)/OSprocess.o $(CFLAGS)

$(OBJD)/kInt64.o : $(SRCD)/kInt64.h $(SRCD)/OStypes.h $(SRCD)/kInt64.c
	$(CC) $(SRCD)/kInt64.c -o $(OBJD)/kInt64.o $(CFLAGS)

$(OBJD)/paging.o : $(SRCD)/paging.h $(SRCD)/OStypes.h $(SRCD)/paging.c
	$(CC) $(SRCD)/paging.c -o $(OBJD)/paging.o $(CFLAGS)

$(OBJD)/PCI.o : $(SRCD)/PCI.h $(SRCD)/ports.h $(SRCD)/OSalloc.h $(SRCD)/OStypes.h $(SRCD)/PCI.c
	$(CC) $(SRCD)/PCI.c -o $(OBJD)/PCI.o $(CFLAGS)

$(OBJD)/OSpower.o : $(SRCD)/TextUI.h $(SRCD)/ports.h $(SRCD)/OSpower.c
	$(CC) $(SRCD)/OSpower.c -o $(OBJD)/OSpower.o $(CFLAGS)

$(OBJD)/SSE.o : $(SRCD)/SSE.h $(SRCD)/SSE.c
	$(CC) $(SRCD)/SSE.c -o $(OBJD)/SSE.o $(CFLAGS)

$(OBJD)/OStypes.o : $(SRCD)/OStypes.h $(SRCD)/OSstring.h $(SRCD)/OStypes.c
	$(CC) $(SRCD)/OStypes.c -o $(OBJD)/OStypes.o $(CFLAGS)

$(OBJD)/exceptions.o : $(SRCD)/TextUI.h $(SRCD)/IDT.h $(SRCD)/OStypes.h $(SRCD)/ports.h $(SRCD)/exceptions.c
	$(CC) $(SRCD)/exceptions.c -o $(OBJD)/exceptions.o $(CFLAGS)

$(OBJD)/OSalloc.o : $(SRCD)/OSalloc.h $(SRCD)/mmapInfo.h $(SRCD)/paging.h $(SRCD)/OSalloc.c
	$(CC) $(SRCD)/OSalloc.c -o $(OBJD)/OSalloc.o $(CFLAGS)

$(OBJD)/datetime.o : $(SRCD)/datetime.h $(SRCD)/ports.h $(SRCD)/datetime.c
	$(CC) $(SRCD)/datetime.c -o $(OBJD)/datetime.o $(CFLAGS)

$(OBJD)/OSstring.o : $(SRCD)/OSstring.h $(SRCD)/OSstring.c
	$(CC) $(SRCD)/OSstring.c -o $(OBJD)/OSstring.o $(CFLAGS)

$(OBJD)/IDT.o : $(SRCD)/IDT.h $(SRCD)/ports.h $(SRCD)/IDT.c
	$(CC) $(SRCD)/IDT.c -o $(OBJD)/IDT.o $(CFLAGS)

$(OBJD)/TextUI.o : $(SRCD)/TextUI.h $(SRCD)/ports.h $(SRCD)/BIOSinfo.h $(SRCD)/TextUI.c
	$(CC) $(SRCD)/TextUI.c -o $(OBJD)/TextUI.o $(CFLAGS)

$(OBJD)/mmapInfo.o : $(SRCD)/mmapInfo.h $(SRCD)/kInt64.h $(SRCD)/OStypes.h $(SRCD)/mmapInfo.c
	$(CC) $(SRCD)/mmapInfo.c -o $(OBJD)/mmapInfo.o $(CFLAGS)

$(OBJD)/BIOSinfo.o : $(SRCD)/BIOSinfo.h $(SRCD)/BIOSinfo.c
	$(CC) $(SRCD)/BIOSinfo.c -o $(OBJD)/BIOSinfo.o $(CFLAGS)

$(OBJD)/kmain.o : $(SRCD)/BIOSinfo.h $(SRCD)/ports.h $(SRCD)/mmapInfo.h $(SRCD)/TextUI.h $(SRCD)/IDT.h $(SRCD)/exceptions.h $(SRCD)/OSstring.h $(SRCD)/datetime.h $(SRCD)/OSalloc.h $(SRCD)/SSE.h $(SRCD)/OStypes.h $(SRCD)/OSpower.h $(SRCD)/paging.h $(SRCD)/PCI.h $(SRCD)/OSprocess.h $(SRCD)/syscalls.h $(SRCD)/kmain.c
	$(CC) $(SRCD)/kmain.c -o $(OBJD)/kmain.o $(CFLAGS)

$(OBJD)/kfront.o : $(SRCD)/kfront.asm
	nasm -felf32 $(SRCD)/kfront.asm -o $(OBJD)/kfront.o

.PHONY : clean

clean :
	rm $(OBJS) image.sys -f

