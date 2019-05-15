# Best Makefile tutorial: https://stackoverflow.com/a/30602701/9555588

# CC = clang

EXE = morse
LIBOUT = libmorse.a
SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

FRAMEWORKS = -framework CoreFoundation -framework ApplicationServices

CFLAGS += -Wall
CPPFLAGS += -Iinclude  # default "include" directory
# CPPFLAGS += -I/usr/local/opt/curl/include

# LDFLAGS += -L/usr/local/opt/curl/lib
# LDFLAGS += -L/usr/local/Cellar/libsodium/1.0.17/lib
LDFLAGS += -L./lib
# LDLIBS += -lm

LDLIBS += -lcurl
LDLIBS += -lsodium

.PHONY: all clean

all: $(EXE) $(LIBOUT)

$(LIBOUT): $(OBJ)
	ar rcs $(LIBOUT) $(OBJ)

$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(FRAMEWORKS) $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)
