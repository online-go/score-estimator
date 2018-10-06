VERSION=1.0
CXX=g++

# Threads actually seem to slow things down a lot, at least on my machine, even
# with just 2 threads. So by default lets not use them, but I'm keeping it easy
# to toggle it on in case something illuminates why this would be.
#CXXFLAGS=-O3 -g3 -Wall --pedantic --std=c++14 -DDEBUG=1 -DUSE_THREADS=1 -lpthread
CXXFLAGS=-O3 -g3 -Wall --pedantic --std=c++14 -DDEBUG=1 -DUSE_THREADS=0

EMCC_FLAGS=-s MODULARIZE=1 -s EXPORT_NAME="'OGSScoreEstimator'" -s EXPORTED_FUNCTIONS="['_estimate']" --memory-init-file 0

all build: run_estimator_tests

quick: build
	#./run_estimator_tests test_games/hard/12508083.game 
	#./run_estimator_tests test_games/hard/9307720.game 
	./run_estimator_tests test_games/mid/3964661.game

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
	emcc $(EMCC_FLAGS) -O3 -g0 jsbindings.cc -o score_estimator.js
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
