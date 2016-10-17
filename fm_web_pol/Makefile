#
# @(#)% %
#
# Copyright (c) 2003, 2013, Oracle and/or its affiliates. All rights reserved. 
#
#      This material is the confidential property of Oracle Corporation
#      or its subsidiaries or licensors and may be used, reproduced, stored
#      or transmitted only in accordance with a valid Oracle license or
#      sublicense agreement.
#

OS=linux
VERSION=7.5

##########
CC_solaris = cc
CC_hpux = cc
CC_hpux_ia64 = cc
CC_aix = xlc_r
CC_linux = gcc
CC = $(CC_$(OS))

C++_solaris = CC
C++_hpux = aCC
C++_hpux_ia64 = aCC
# not sure about aix
C++_aix = xlc_r 
C++_linux = gcc
C++ = $(C++_$(OS))
##########

CFLAGS_solaris= -g -xcg92
CFLAGS_hpux= -g -Ae +Z +DAportable
CFLAGS_hpux_ia64= -g -Ae +Z 
CFLAGS_aix= -g -Drs6000 -D__aix -Dunix -D__unix
CFLAGS_linux = -m32 -fPIC 

CFLAGS= $(CFLAGS_$(OS))

C++FLAGS_solaris= -g -library=%none -DPIN_NOT_USING_OSTREAM
C++FLAGS_hpux= -g -AA +Z +DAportable +nostl -DPIN_NOT_USING_OSTREAM -I/usr/include -I/opt/aCC/include -I/usr 
C++FLAGS_hpux_ia64= -g -AA +Z +nostl -DPIN_NOT_USING_OSTREAM -I/usr/include -I/opt/aCC/include -I/usr
C++FLAGS_aix= -g -Drs6000 -D__aix -Dunix -D__unix
C++FLAGS_linux = -x c++ -m32 -DPIN_NOT_USING_OSTREAM

C++FLAGS= $(C++FLAGS_$(OS)) -DPCMCPP_CONST_SAFE

CPPFLAGS = -I$(INCDIR) -I$(ST_HOME)/include -DFOR_CM -DFM_SUBSCRIPTION_POL_DLL

LDFLAGS_solaris= -G -library=%none 
LDFLAGS_hpux= -b
LDFLAGS_hpux_ia64= -b
LDFLAGS_aix= -G -bnoentry -bexpall -brtl -lc
LDFLAGS_linux= -m32 -shared
LDFLAGS = $(LDFLAGS_$(OS))

SL_EXT_solaris= so
SL_EXT_hpux= sl
SL_EXT_hpux_ia64= so
SL_EXT_aix= a
SL_EXT_linux= so
SL_EXT= $(SL_EXT_$(OS))

####

##PINDIR=../../..
PINDIR=$(PIN_HOME)
INCDIR=$(PINDIR)/include
LIBDIR=$(PINDIR)/lib

###########

INCFILES= $(INCDIR)/pcm_ops.h

#
FILES= fm_web_pol_config.c \
        fm_web_acc_create_customer.c \
	fm_web_plan_details.c \
	fm_web_acc_details.c \
	fm_web_bill_details.c \
	fm_web_inv.c \
	fm_plan_transition.c \
	fm_web_pymnt.c \
	fm_web_adj.c \
	fm_web_adj_his.c \
	fm_web_pymnt_his.c

#

OBJECTS= fm_web_pol_config.o \
        fm_web_acc_create_customer.o \
	fm_web_plan_details.o \
        fm_web_acc_details.o \
        fm_web_bill_details.o \
	fm_web_inv.o\
	fm_plan_transition.o \
	fm_web_pymnt.o\
	fm_web_adj.o \
	fm_web_adj_his.o \
	fm_web_pymnt_his.o
LIBSUBS=$(LIBDIR)/fm_web_pol.$(SL_EXT)

###########

all: $(LIBSUBS)

clean:
	rm -f $(OBJECTS) core

clobber: clean
	rm -f $(LIBSUBS)

lint:
	lint $(CPPFLAGS) $(FILES)

tags: FRC

###########

$(LIBSUBS): $(OBJECTS) $(C++_OBJECTS) $(INCFILES) Makefile
	$(C++) -o $(LIBSUBS) $(LDFLAGS) $(OBJECTS) $(C++_OBJECTS) -lm

$(OBJECTS): $(INCFILES) Makefile
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(FILES)

$(C++_OBJECTS): $(INCFILES) Makefile $(C++_FILES)
	$(C++) -c $(C++FLAGS) $(CPPFLAGS) $(C++_FILES)

#fm_subscription_pol_init.o: $(INCFILES) Makefile
#	$(C++) -c $(C++FLAGS) $(CPPFLAGS) fm_subscription_pol_init.cpp

#fm_subscription_pol_snowball_discount.o: $(INCFILES) Makefile
#	$(C++) -c $(C++FLAGS) $(CPPFLAGS) fm_subscription_pol_snowball_discount.cpp

#

FRC:

