.PHONY: run
run: build
	@mkdir -p tmp
	@touch tmp/test1.wc
	@touch tmp/test2.wc
	@touch tmp/test3.wc
	@./tgt/a.out

.PHONY: build
build: src/main.cpp src/VirtualMachine.cpp
	@mkdir -p tgt
	@clang++ src/main.cpp src/VirtualMachine.cpp -o tgt/a.out -g -Wall -ldl

.PHONY: clean
clean:
	@rm -f -r tgt
	@rm -f -r tmp
