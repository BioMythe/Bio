BUILD_PATH  ?= Binaries
SOURCE_PATH ?= Source
TARGET_NAME ?= libroutinelib.a
TARGET      := $(BUILD_PATH)/$(TARGET_NAME)

SOURCES ?= $(shell find $(SOURCE_PATH) -name *.c)
OBJECTS ?= $(SOURCES:%=$(BUILD_PATH)/Int/%.o)

RM    ?= rm
AR    ?= ar
CC    ?= gcc
ECHO  ?= echo
MKDIR ?= mkdir

$(TARGET): $(OBJECTS)
	@$(MKDIR) -p $(dir $@)
	@$(ECHO) Build.RoutineLib: Linking final static library \"$@\"...
	@$(AR) rcs $@ $^

$(BUILD_PATH)/Int/%.c.o: %.c
	@$(MKDIR) -p $(dir $@)
	@$(ECHO) Build.RoutineLib: Compiling \"$<\" to \"$@\"...
	@$(CC) $(CFLAGS) -c $< -o $@	

clean:
	@$(ECHO) Removing build directory \"$(BUILD_PATH)\"...
	@$(RM) -rf $(BUILD_PATH)
