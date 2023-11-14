from distutils.core import setup, Extension
import sysconfig


def main():
    CFLAGS = ['-g', '-Wall', '-std=c99', '-fopenmp', '-mavx', '-mfma', '-pthread', '-O3',
              '-march=native', '-fprefetch-loop-arrays', '-malign-data=cacheline']
    # CFLAGS = ['-g', '-Wall', '-std=c99', '-fopenmp', '-mavx', '-mfma', '-pthread', '-funroll-loops',
    #           '-march=native', '-O3', '-xopenmp=parallel', '-xvpara',
    #           '-xautopar', '-xM', '-x[T,P,O,N,W,K]', '-fprefetch-loop-arrays',
    #           '-xchip', '-xcache', '-xarch', '-xtarget', '-xregs',
    #           '-fbranch-probabilities', '-frename-registers', '-flto', '-mtune=native']
    #
    # CFLAGS = ['-g', '-Wall', '-std=c99', '-fopenmp', '-mavx', '-mfma', '-pthread', '-funroll-loops',
    #           '-freorder-blocks-and-partition', '-march=native', '-finstrument-functions', '-Ofast',
    #           '-fno-tree-loop-if-convert', '-fno-common', '-fipa-pta', '-fno-sched-interblock', '-fno-tree-copyrename',
    #           '-fno-peephole2', '-fno-expensive-optimizations',
    #           '-fno-ipa-sra', '-fgcse-las', '-fno-schedule-insns',
    #           '-fno-tree-loop-distribute-patterns', '-fno-caller-saves',
    #           '-fno-optimize-strlen', '-fno-inline-functions-called-once',
    #           '-fno-tree-slsr', '-fno-tree-scev-cprop', '-funroll-all-loops',
    #           '-fno-sched-dep-count-heuristic', '-fno-tree-ccp',
    #           '-fno-predictive-commoning', '-fno-ipa-pure-const',
    #           '-fno-merge-constants', '-fno-tree-pta'
    #           ]

    LDFLAGS = ['-fopenmp']
    # Use the setup function we imported and set up the modules.
    # You may find this reference helpful: https://docs.python.org/3.6/extending/building.html
    # TODO: YOUR CODE HERE
    module = Extension("numc", sources=["numc.c", "matrix.c"], extra_compile_args=CFLAGS, extra_link_args=LDFLAGS)
    setup(name="numcPartA", version="1.0", description="Part A test", ext_modules=[module])


if __name__ == "__main__":
    main()
