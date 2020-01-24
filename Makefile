.PHONY: build
build: src/main.cpp src/VirtualMachine.cpp
	-mkdir -p tgt
	clang++ src/main.cpp src/VirtualMachine.cpp -o tgt/a.out -g -Wall

.PHONY: run
run: build tmp/test.wc
	./tgt/a.out

.PHONY: clean
clean:
	rm -f tgt/a.out