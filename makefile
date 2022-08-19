# cmake wrapper

DEBUGGER = gdb

BUILDFDR = bin
EXEC = vged

default:
	cp -r Config/* bin/
	make -s --quiet -C $(BUILDFDR)

run:
	./$(BUILDFDR)/$(EXEC)

test:
	./$(BUILDFDR)/$(EXEC) --test
valgrind:
	valgrind --leak-check=full ./$(BUILDFDR)/$(EXEC) --test

noshade:
	./$(BUILDFDR)/$(EXEC) --delete-shader-cache

debug:
	cd $(BUILDFDR)
	$(DEBUGGER) $(BUILDFDR)/$(EXEC)

# use dmalloc sometimes because memory is hard
dmalloc:
	dmalloc -g -V -l malloclog -i 1 high
	./$(BUILDFDR)/$(EXEC)

cmake:
	mkdir -p $(BUILDFDR)
	cmake $(BUILDFDR)
