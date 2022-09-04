# cmake wrapper

DEBUGGER = gdb

BUILDFDR = build/Debug
EXEC = editor

default:
	make -C $(BUILDFDR) -s

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
