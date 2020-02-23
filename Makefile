ARG=default

.PHONY: run
run: build
	@./tgt/rcwt ${ARG}

.PHONY: check
check: src/test.cpp
	@mkdir -p tmp
	@mkdir -p tgt
	@touch tmp/_test1.wc
	@touch tmp/_test2.wc
	@touch tmp/_test3.wc
	@touch tmp/_test4a.ll
	@touch tmp/_test4b.ll
	@touch tmp/_test5.ll
	@clang++ src/test.cpp -o tgt/test -g -Wall
	@./tgt/test

.PHONY: build
build: src/main.cpp src/VirtualMachine.cpp src/jit_compiler.cpp src/converter.cpp src/error.cpp src/ll.cpp
	@mkdir -p tmp
	@mkdir -p tgt
	@clang++ src/main.cpp src/VirtualMachine.cpp src/jit_compiler.cpp src/converter.cpp src/error.cpp src/ll.cpp -o tgt/rcwt -g -Wall -ldl

.PHONY: clean
clean:
	@rm -f -r tgt
	@rm -f -r tmp
