CC = gcc
CFLAGS = -g -Wall -std=c99 -fopenmp -mavx -mfma -pthread
LDFLAGS = -fopenmp
CUNIT = -L/home/ff/cs61c/cunit/install/lib -I/home/ff/cs61c/cunit/install/include -lcunit
PYTHON = -I/usr/include/python3.6 -lpython3.6m

install:
	if [ ! -f files.txt ]; then touch files.txt; fi
	rm -rf build
	xargs rm -rf < files.txt
	python3 setup.py install --record files.txt

uninstall:
	if [ ! -f files.txt ]; then touch files.txt; fi
	rm -rf build
	xargs rm -rf < files.txt

clean:
	rm -f *.o
	rm -f test
	rm -rf build
	rm -rf __pycache__

test:
	rm -f test
	$(CC) $(CFLAGS) mat_test.c matrix.c -o test $(LDFLAGS) $(CUNIT) $(PYTHON)
	./test

.PHONY: test

addTest:
	python -m unittest unittests.TestAdd -v

absTest:
	python -m unittest unittests.TestAbs -v

mulTest:
	#python -m unittest unittests.TestMul -v
	python -m unittest unittests.TestMul.test_rando -v
	#python -m unittest unittests.TestMul.test_prob -v
	#python -m unittest unittests.TestMul.test_prob1 -v

Alltest:
	python -m unittest unittests.py -v


negTest:
	python -m unittest unittests.TestNeg -v

powTest:
	python -m unittest unittests.TestPow -v

weirdTest:
	python -m unittest unittests.TestMul.test_prob1 -v

weirdPow:
	python -m unittest unittests.TestPow.weird -v

addSpecial:
	python -m unittest unittests.TestAdd.test13 -v

subSpecial:
	python -m unittest unittests.TestSub -v

special:
	python -m unittest unittests.TestSub -v
	python -m unittest unittests.TestAdd.test13 -v

add-rando:
	python -m unittest unittests.TestAdd.rando -v

sub-rando:
	python -m unittest unittests.TestSub.rando -v

abs-rando:
	python -m unittest unittests.TestAbs.rando -v

neg-rando:
	python -m unittest unittests.TestNeg.rando -v
mul-rando:
	python -m unittest unittests.TestMul.test_rando -v
pow-rando:
	python -m unittest unittests.TestPow.rando -v

slice-rando:
	python -m unittest unittests.TestSlice -v

sanity:
	python -m unittest unittests.TestSanity -v

rando-all:
	#python -m unittest unittests.TestSlice -v
	#python -m unittest unittests.TestNeg.rando -v
	#python -m unittest unittests.TestAbs.rando -v
	#python -m unittest unittests.TestSub.rando -v
	#python -m unittest unittests.TestAdd.rando -v
	python -m unittest unittests.TestPow.rando -v
	#python -m unittest unittests.TestMul.test_rando -v
