VERSION=1.0
cxx=g++ -O3 -g3 -Wall --pedantic --std=c++14 -DDEBUG=1

EMCC_FLAGS=-s MODULARIZE=1 -s EXPORT_NAME="'OGSScoreEstimator'" -s EXPORTED_FUNCTIONS="['_estimate']" --memory-init-file 0

all build: estimator

test: build
	./estimator test_games/12508083.game

test-all: build
	./estimator test_games/*.game

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

estimator: main.cc Goban.cc *.h Makefile
	$(cxx) main.cc Goban.cc -o estimator

grind: build
	valgrind --tool=cachegrind ./estimator  test_games/*.game

clean:
	rm -f estimator *.o *.js
