TARGET=haribote.img

SRCDIR=.
INC=-I ../include
OBJDIR=../obj
LIBDIR=../lib

SRCS=$(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/*.cc) $(wildcard $(SRCDIR)/*.s)
OBJS=$(subst $(SRCDIR)/,$(OBJDIR)/,$(subst .s,.o,$(subst .cc,.o,$(SRCS:%.c=%.o))))
APPS=$(OBJS:%.o=%.hrb)

APILIB=$(LIBDIR)/libhrb.a

ASFLAGS=--32
CFLAGS=-m32 -O2 --std=c99 -Wall -Wextra -Werror -nostdinc $(INC)
CFLAGS+=-fno-stack-protector  # Avoid reference for __stack_chk_fail
CFLAGS+=-fno-pie
CXXFLAGS=-m32 -O2 --std=c++11 -Wall -Wextra -Werror -nostdinc $(INC)
CXXFLAGS+=-fno-rtti -fno-exceptions
CXXFLAGS+=-fno-stack-protector  # Avoid reference for __stack_chk_fail
CXXFLAGS+=-fno-pie

CRT0=$(LIBDIR)/crt0.o
LKSCRIPT=$(LIBDIR)/hrbapp.ls
LKOPT=-m elf_i386 -s -L $(LIBDIR)
LKAPP=ld -T $(LKSCRIPT) $(LKOPT) $(CRT0)
APPLIBS=-lhrb

all:	$(APPS)

.SUFFIXES:
.SUFFIXS:	.o .c .cc .s .hrb
.PRECIOUS:	$(OBJS)

$(OBJDIR)/%.o:	$(SRCDIR)/%.s
	as -o $@ $(ASFLAGS) $<
$(OBJDIR)/%.o:	$(SRCDIR)/%.c
	gcc -c -o $@ $(CFLAGS) $<
$(OBJDIR)/%.o:	$(SRCDIR)/%.cc
	g++ -c -o $@ $(CXXFLAGS) $<
$(OBJDIR)/%.hrb:	$(OBJDIR)/%.o $(APILIB) $(CRT0)
	$(LKAPP) -o $@ $< $(APILIB)

$(OBJDIR)/gview.o:	$(SRCDIR)/gview.c $(SRCDIR)/jpeg.ccc
$(OBJDIR)/gview.hrb:	$(OBJDIR)/gview.o bmp.obj $(APILIB) $(CRT0)
	$(LKAPP) -o $@ $< bmp.obj $(APILIB)
#$(OBJDIR)/bmp.o:	$(SRCDIR)/bmp.nasm
#	as $< -o $@

$(APILIB):
	make -C ../apilib
