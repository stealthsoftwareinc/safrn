CXX=g++
CMAKE_CMD=cmake
CLANG_FORMAT_CMD=clang-format
STEALTH_CLANG_FORMAT_CMD=stealth-clang-format
BUILD_TYPE=Release
TEST_FILTER='*'

INTERNAL=false

# dependency variables
HOST=x86_64-linux-gnu

test: build
	cd target/lib/fortissimo-dir/src/test/cpp \
		&& ./fortissimo_test --gtest_filter=$(TEST_FILTER)
	cd target/shared/src/test/cpp \
		&& ./shared_test --gtest_filter=$(TEST_FILTER)
	cd target/server/src/test/cpp \
		&& ./server_test --gtest_filter=$(TEST_FILTER)

build: build-configure target/
	cd target/ \
		&& make -j4 \
		&& cp ffnet/src/main/cpp/safrnffnet safrnffnet \
	;
		# && cp client/src/main/cpp/client safrn-client \
		# && cp server/src/main/cpp/server_exec safrn-server \

debug-server-test: build
	cd target/server/src/test/cpp \
		&& gdb --args ./server_test --gtest_filter=$(TEST_FILTER) \
	;

debug-shared-test: build
	cd target/shared/src/test/cpp \
		&& gdb --args ./shared_test --gtest_filter=$(TEST_FILTER) \
	;

target/configure.success:
	cd target/ \
		&& $(CMAKE_CMD) -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
			-DCMAKE_CXX_COMPILER=$(shell which ${CXX}) ../ \
	;
	echo 'success' > target/configure.success

build-configure: dependencies target/ target/configure.success
configure: mopclean dependencies target/ target/configure.success

lib/dependencies.success:
	cd lib/ && make INTERNAL=$(INTERNAL) HOST=$(HOST) all
	echo 'success' > lib/dependencies.success

dependencies: lib/dependencies.success

dependencies-download-only:
	cd lib/ && make INTERNAL=$(INTERNAL) HOST=$(HOST) download-only

target/:
	mkdir -p target/

clean:
	[ -f target/Makefile ] && ( cd target/ && make clean ) || true;

mopclean:
	rm -rf target/

mrclean: mopclean
	cd lib && make mrclean
	rm -f lib/dependencies.success

format:
	$(CLANG_FORMAT_CMD) -i --style=file\
		$(wildcard shared/src/main/cpp/*.cpp shared/src/main/cpp/*.h) \
		$(wildcard shared/src/main/cpp/*/*.cpp shared/src/main/cpp/*/*.h) \
		$(wildcard shared/src/main/cpp/*/*/*.cpp shared/src/main/cpp/*/*/*.h) \
		$(wildcard shared/src/main/cpp/*/*/*/*.cpp shared/src/main/cpp/*/*/*/*.h) \
		$(wildcard client/src/main/cpp/*.cpp client/src/main/cpp/*.h) \
		$(wildcard client/src/main/cpp/*/*.cpp client/src/main/cpp/*/*.h) \
		$(wildcard client/src/main/cpp/*/*/*.cpp client/src/main/cpp/*/*/*.h) \
		$(wildcard client/src/main/cpp/*/*/*/*.cpp client/src/main/cpp/*/*/*/*.h) \
		$(wildcard server/src/main/cpp/*.cpp server/src/main/cpp/*.h) \
		$(wildcard server/src/main/cpp/*/*.cpp server/src/main/cpp/*/*.h) \
		$(wildcard server/src/main/cpp/*/*/*.cpp server/src/main/cpp/*/*/*.h) \
		$(wildcard server/src/main/cpp/*/*/*/*.cpp server/src/main/cpp/*/*/*/*.h) \
		$(wildcard shared/src/test/cpp/*.cpp shared/src/test/cpp/*.h) \
		$(wildcard shared/src/test/cpp/*/*.cpp shared/src/test/cpp/*/*.h) \
		$(wildcard shared/src/test/cpp/*/*/*.cpp shared/src/test/cpp/*/*/*.h) \
		$(wildcard shared/src/test/cpp/*/*/*/*.cpp shared/src/test/cpp/*/*/*/*.h) \
		$(wildcard client/src/test/cpp/*.cpp client/src/test/cpp/*.h) \
		$(wildcard client/src/test/cpp/*/*.cpp client/src/test/cpp/*/*.h) \
		$(wildcard client/src/test/cpp/*/*/*.cpp client/src/test/cpp/*/*/*.h) \
		$(wildcard client/src/test/cpp/*/*/*/*.cpp client/src/test/cpp/*/*/*/*.h) \
		$(wildcard server/src/test/cpp/*.cpp server/src/test/cpp/*.h) \
		$(wildcard server/src/test/cpp/*/*.cpp server/src/test/cpp/*/*.h) \
		$(wildcard server/src/test/cpp/*/*/*.cpp server/src/test/cpp/*/*/*.h) \
		$(wildcard server/src/test/cpp/*/*/*/*.cpp server/src/test/cpp/*/*/*/*.h) \
		$(wildcard ffnet/src/main/cpp/*.cpp ffnet/src/main/cpp/*.h) \
	;

stealth-format:
	$(STEALTH_CLANG_FORMAT_CMD) -i \
		$(wildcard shared/src/main/cpp/*.cpp shared/src/main/cpp/*.h) \
		$(wildcard shared/src/main/cpp/*/*.cpp shared/src/main/cpp/*/*.h) \
		$(wildcard shared/src/main/cpp/*/*/*.cpp shared/src/main/cpp/*/*/*.h) \
		$(wildcard shared/src/main/cpp/*/*/*/*.cpp shared/src/main/cpp/*/*/*/*.h) \
		$(wildcard client/src/main/cpp/*.cpp client/src/main/cpp/*.h) \
		$(wildcard client/src/main/cpp/*/*.cpp client/src/main/cpp/*/*.h) \
		$(wildcard client/src/main/cpp/*/*/*.cpp client/src/main/cpp/*/*/*.h) \
		$(wildcard client/src/main/cpp/*/*/*/*.cpp client/src/main/cpp/*/*/*/*.h) \
		$(wildcard server/src/main/cpp/*.cpp server/src/main/cpp/*.h) \
		$(wildcard server/src/main/cpp/*/*.cpp server/src/main/cpp/*/*.h) \
		$(wildcard server/src/main/cpp/*/*/*.cpp server/src/main/cpp/*/*/*.h) \
		$(wildcard server/src/main/cpp/*/*/*/*.cpp server/src/main/cpp/*/*/*/*.h) \
		$(wildcard shared/src/test/cpp/*.cpp shared/src/test/cpp/*.h) \
		$(wildcard shared/src/test/cpp/*/*.cpp shared/src/test/cpp/*/*.h) \
		$(wildcard shared/src/test/cpp/*/*/*.cpp shared/src/test/cpp/*/*/*.h) \
		$(wildcard shared/src/test/cpp/*/*/*/*.cpp shared/src/test/cpp/*/*/*/*.h) \
		$(wildcard client/src/test/cpp/*.cpp client/src/test/cpp/*.h) \
		$(wildcard client/src/test/cpp/*/*.cpp client/src/test/cpp/*/*.h) \
		$(wildcard client/src/test/cpp/*/*/*.cpp client/src/test/cpp/*/*/*.h) \
		$(wildcard client/src/test/cpp/*/*/*/*.cpp client/src/test/cpp/*/*/*/*.h) \
		$(wildcard server/src/test/cpp/*.cpp server/src/test/cpp/*.h) \
		$(wildcard server/src/test/cpp/*/*.cpp server/src/test/cpp/*/*.h) \
		$(wildcard server/src/test/cpp/*/*/*.cpp server/src/test/cpp/*/*/*.h) \
		$(wildcard server/src/test/cpp/*/*/*/*.cpp server/src/test/cpp/*/*/*/*.h) \
		$(wildcard ffnet/src/main/cpp/*.cpp ffnet/src/main/cpp/*.h) \
	;
