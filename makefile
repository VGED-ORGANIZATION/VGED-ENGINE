# cmake wrapper

DEBUGGER = gdb

BUILDFDR = bin/Debug
EXEC = editor

default:
	./build.sh

run:
	./$(BUILDFDR)/$(EXEC)

valgrind:
	valgrind --leak-check=full ./$(BUILDFDR)/$(EXEC) --test

debug:
	cd $(BUILDFDR)
	$(DEBUGGER) $(BUILDFDR)/$(EXEC)

cmake:
	mkdir -p $(BUILDFDR)
	cmake $(BUILDFDR)
