VERSION=0.6.1
CXX=g++

# Threads actually seem to slow things down a lot, at least on my machine, even
# with just 2 threads. So by default lets not use them, but I'm keeping it easy
# to toggle it on in case something illuminates why this would be.
CXXFLAGS=-O3 -g3 -Wall --pedantic --std=c++14 -DDEBUG=1 -DUSE_THREADS=1 -lpthread
#CXXFLAGS=-O3 -g3 -Wall --pedantic --std=c++14 -DDEBUG=1 -DUSE_THREADS=0

EMCC_FLAGS=--std=c++14 -s MODULARIZE=1 -s EXPORT_NAME="'OGSScoreEstimator'" -s EXPORTED_FUNCTIONS="['_estimate']" -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap']" --memory-init-file 0 -s WASM=0 

all build: run_estimator_tests

quick: build
	#./run_estimator_tests test_games/hard/12508083.game 
	#./run_estimator_tests test_games/hard/9307720.game 
	#./run_estimator_tests test_games/mid/3964661.game
	#./run_estimator_tests test_games/easy/13043976.game
	./run_estimator_tests test_games/easy/14242770.game


watch:
	watch -n 0.5 -c -d ./run_estimator_tests  \
		test_games/no_removals/*.game \
		test_games/patterns/*.game \
		test_games/easy/*.game \
		test_games/mid/*.game \
		test_games/hard/*.game \
		test_games/really_hard/*.game



pattern patterns: build
	./run_estimator_tests test_games/patterns/*.game


hard: build
	./run_estimator_tests test_games/hard/*.game
easy: build
	./run_estimator_tests test_games/easy/*.game
mid: build
	./run_estimator_tests test_games/mid/*.game
no_removal: build
	./run_estimator_tests test_games/no_removal/*.game

light: build
	./run_estimator_tests \
		test_games/no_removals/13790686.game \
		test_games/easy/13344450.game \
		test_games/mid/13647265.game \
		test_games/hard/12508083.game 

test tests: build
	./run_estimator_tests test_games/*/*.game

js-debug:
	emcc $(EMCC_FLAGS) -s DEMANGLE_SUPPORT=1 -s ASSERTIONS=2 -O1 -g3 jsbindings.cc -o score_estimator.js

js:
	emcc $(EMCC_FLAGS) -O2 -g0 jsbindings.cc -o score_estimator.js

dist:
	emcc $(EMCC_FLAGS) -O2 -g0 jsbindings.cc -o OGSScoreEstimator-$(VERSION).js
	@rm -f score_estimator.js.gz
	@gzip -k score_estimator.js
	@echo
	@echo Uncompressed: `wc -c score_estimator.js | awk '{print $$1}'` bytes
	@echo Compressed: `wc -c score_estimator.js.gz | awk '{print $$1}'` bytes
	@echo

run_estimator_tests: *.cc *.h Makefile
	$(CXX) $(CXXFLAGS) main.cc Goban.cc -o run_estimator_tests

grind: build
	valgrind --tool=cachegrind ./run_estimator_tests  test_games/*/*.game

clean:
	rm -f run_estimator_tests *.o *.js
