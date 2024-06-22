CXX ?= g++

default: server

CPPFLAGS := -I . -I http-server/src -I html-builder/src
CXXFLAGS := -Wall -Wextra -Wswitch-enum -pedantic -O2
LDFLAGS :=

SOURCES += main.cpp
SOURCES += build/profiler.o
SOURCES += build/endpoint-base.o build/endpoint-dispatcher.o
SOURCES += build/http-server.a
SOURCES += build/html-builder.o
server: $(SOURCES)
	$(CXX) -o $@ $(CXXFLAGS) $(CPPFLAGS) $^ $(LDFLAGS)

build:
	mkdir -p build

build/%.o: %.cpp | build
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

ENDPOINTS := $(shell find endpoints -type f)
build/endpoint-dispatcher.o: endpoint-dispatcher.cpp $(ENDPOINTS) | build
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

build/http-server.a: build
	cd http-server && make build/http-server.a
	ln -sf $(PWD)/http-server/build/http-server.a $@

build/html-builder.o: build
	cd html-builder && make build/html-builder.o
	ln -sf $(PWD)/html-builder/build/html-builder.o $@

.PHONY: test
test: server
	./server 8080

.PHONY: clean
clean:
	@FILES=$$(git clean -ndX); \
	if [ -z "$$FILES" ]; then \
		echo "No deletable files, skipping"; \
	else \
		echo "These files will be deleted:"; \
		echo "$$FILES"; \
		read -p "Do you want to delete them? [N/y] " yn; \
		if [ "$$yn" = "y" ] || [ "$$yn" = "Y" ]; then \
			git clean -fdX; \
		else \
			echo "Deletion cancelled."; \
		fi \
	fi
