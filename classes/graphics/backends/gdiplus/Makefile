########## smooth directory makefile ##########

# Change these variables to fit this location:
SMOOTH_PATH = ../../../..

include $(dir $(firstword $(MAKEFILE_LIST)))/$(SMOOTH_PATH)/Makefile-options

# Enter object files here:
OBJECTS	  = 

ifeq ($(BUILD_GDIPLUS),True)
	OBJECTS += bitmapgdiplus.o

ifneq ($(BUILD_CAIRO),True)
	OBJECTS += fontgdiplus.o surfacegdiplus.o
endif
endif

## Do not change anything below this line. ##

include $(dir $(firstword $(MAKEFILE_LIST)))/$(SMOOTH_PATH)/Makefile-commands
