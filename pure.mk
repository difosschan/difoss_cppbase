###################################################################################
# Makefile                                                                        #
# @author: ChenRuirong(difoss@163.com)                                            #
# @since: 2015-2-26 17:10                                                         #
# @license GPLv3                                                                  #
# @copyright: ChenRuirong personal (difoss@163.com)                               #
# @version: <2015-2-26 17:10:00 ChenRuirong>                                      #
# @summary: built difoss base library pure version (both shared & static),        #
#     select from 'adsb-base' library,                                            #
#     use by mysql cluster of NG3 Transformation project.                         #
###################################################################################
include ../../platform.mk

LIB = libdifossbase_pure.so
STATIC = libdifossbase_pure_a.a
# find *.cpp files in current directory and sub-directories.
SRC = $(shell find . -name '*.cpp'|grep -v -E 'mysql_util|ndb_util|sqlite_util')
OBJ = $(SRC:.cpp=.o)
INSTALL_PATH = ../../../$(LIB_INSTALL_DIR)

###########################################
CXX=g++
LINK=$(CXX)
AR=ar
DEFS=-DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"1.0\" -DPACKAGE_STRING=\"1.0\" -DPACKAGE_BUGREPORT=\"support@revenco.com\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_STRING_H=1
INCLUDES = -I. -I../3rdparty
CPP_FLAGS = -shared -fPIC -fpic -Wall -ggdb
LD_FLAGS  = -shared -fPIC -lpthread

AR_FLAGS = -r -o
RM = rm
RM_FORCE = $(RM) -rf

###########################################
all       : $(STATIC) $(LIB)

$(LIB) : $(OBJ)
	$(LINK) $(LD_FLAGS) -o $@ $^

$(STATIC) : $(OBJ)
	$(AR) $(AR_FLAGS) $@ $^

%.o: %.cpp
	$(CC) -c $(CPP_FLAGS) $(DEFS) $(INCLUDES) $< -o $@

install    : $(LIB) $(STATIC)
	rm -f $(INSTALL_PATH)/$(STATIC) $(INSTALL_PATH)/$(LIB)
	cp -f $(LIB) $(INSTALL_PATH)/$(LIB)
	cp -f $(STATIC) $(INSTALL_PATH)/$(STATIC)

.PHONY : clean
clean      :
	$(RM_FORCE) $(OBJ)
	$(RM_FORCE) $(INSTALL_PATH)/$(LIB)
	$(RM_FORCE) $(INSTALL_PATH)/$(STATIC)
