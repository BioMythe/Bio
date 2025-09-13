BUILD_PATH  ?= Binaries
SOURCE_PATH ?= Source
TARGET_NAME ?= Mythe
TARGET      := $(BUILD_PATH)/$(TARGET_NAME)

ROUTINE_LIB_PATH ?= RoutineLib
ROUTINE_LIB      ?= libroutinelib.a

SOURCES ?= $(shell find $(SOURCE_PATH) -name *.c)
OBJECTS ?= $(SOURCES:%=$(BUILD_PATH)/Int/%.o)

CFLAGS  ?= -I$(SOURCE_PATH) -I$(ROUTINE_LIB_PATH)
LDFLAGS ?= $(ROUTINE_LIB)

RM    ?= rm
CC    ?= gcc
ECHO  ?= echo
MKDIR ?= mkdir

$(TARGET): $(OBJECTS)
	@$(MKDIR) -p $(dir $@)
	@$(ECHO) Build.Mythe: Linking final executable \"$@\"...
	@$(CC) $^ $(LDFLAGS) -o $@

$(BUILD_PATH)/Int/%.c.o: %.c
	@$(MKDIR) -p $(dir $@)
	@$(ECHO) Build.Mythe: Compiling \"$<\" to \"$@\"...
	@$(CC) $(CFLAGS) -c $< -o $@	

clean:
	@$(ECHO) Removing build directory \"$(BUILD_PATH)\"...
	@$(RM) -rf $(BUILD_PATH)
