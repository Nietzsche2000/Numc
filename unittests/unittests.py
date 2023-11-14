from utils import *
from unittest import TestCase

"""
For each operation, you should write tests to test  on matrices of different sizes.
Hint: use dp_mc_matrix to generate dumbpy and numc matrices with the same data and use
      cmp_dp_nc_matrix to compare the results
"""

# TODO: remove
import random
import time


class TestAdd(TestCase):
    avg = []

    def test_small_add(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(2, 2, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(2, 2, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)
        self.avg.append(speed_up)

    def test_small_add2(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(4, 4, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(4, 4, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)
        self.avg.append(speed_up)

    def test_medium_add(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(20, 20, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(20, 20, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)
        self.avg.append(speed_up)

    def test_large_add(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(1024, 1024, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(1024, 1024, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)
        self.avg.append(speed_up)

    def test1(self):
        # TODO: YOUR CODE HERE
        matrixRows = 2000
        matrixCols = 2000
        iter = 100
        speedupArr = []

        # for i in range(iter):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        speedupArr.append(speed_up)
        print(speed_up)
        self.avg.append(speed_up)

    def test2(self):
        # TODO: YOUR CODE HERE
        matrixRows = 3000
        matrixCols = 2000
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print(speed_up)
        self.avg.append(speed_up)

    def test3(self):
        # TODO: YOUR CODE HERE
        matrixRows = 4000
        matrixCols = 2000
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print(speed_up)
        self.avg.append(speed_up)

    def test4(self):
        # TODO: YOUR CODE HERE
        matrixRows = 4000
        matrixCols = 5000
        iter = 100
        speedupArr = []

        # for i in range(iter):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        speedupArr.append(speed_up)
        print(speed_up)
        self.avg.append(speed_up)

    def test5(self):
        # TODO: YOUR CODE HERE
        matrixRows = 4012
        matrixCols = 5003
        iter = 100
        speedupArr = []

        # for i in range(iter):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        speedupArr.append(speed_up)
        print(speed_up)
        self.avg.append(speed_up)

    def test6(self):
        # TODO: YOUR CODE HERE
        matrixRows = 9012
        matrixCols = 1003
        iter = 100
        speedupArr = []

        # for i in range(iter):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        speedupArr.append(speed_up)
        print(speed_up)
        self.avg.append(speed_up)

    def test7(self):
        # TODO: YOUR CODE HERE
        matrixRows = 2000
        matrixCols = 2000
        iter = 100
        speedupArr = []

        # for i in range(iter):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        speedupArr.append(speed_up)
        print(speed_up)
        self.avg.append(speed_up)

    def test8(self):
        # TODO: YOUR CODE HERE
        matrixRows = 3000
        matrixCols = 2000
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print(speed_up)
        self.avg.append(speed_up)

    def test9(self):
        # TODO: YOUR CODE HERE
        matrixRows = 4000
        matrixCols = 2000
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        self.avg.append(speed_up)

    def test10(self):
        # TODO: YOUR CODE HERE
        matrixRows = 4000
        matrixCols = 5000
        iter = 100
        speedupArr = []

        # for i in range(iter):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        speedupArr.append(speed_up)
        print(speed_up)
        self.avg.append(speed_up)

    def test11(self):
        # TODO: YOUR CODE HERE
        matrixRows = 4012
        matrixCols = 5003
        iter = 100

        # for i in range(iter):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print(speed_up)
        self.avg.append(speed_up)

    def test12(self):
        # TODO: YOUR CODE HERE
        matrixRows = 9012
        matrixCols = 1003
        iter = 100
        speedupArr = []

        # for i in range(iter):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        speedupArr.append(speed_up)
        print(speed_up)
        self.avg.append(speed_up)

    def test_a(self):
        print(sum(self.avg) / len(self.avg))

    def test13(self):
        # TODO: YOUR CODE HERE
        matrixRows = 3231
        matrixCols = 1
        iter = 100
        speedupArr = []

        # for i in range(iter):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        speedupArr.append(speed_up)
        print(speed_up)
        self.avg.append(speed_up)

    def test13(self):
        # TODO: YOUR CODE HERE
        matrixRows = 1
        matrixCols = 12231
        iter = 100
        speedupArr = []

        # for i in range(iter):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        speedupArr.append(speed_up)
        print(speed_up)
        self.avg.append(speed_up)

    def rando(self):
        low = 1
        high = 10000
        for i in range(low, high):
            row1 = random.randint(1, 2000)
            col1 = random.randint(1, 2000)
            print("---------------------------------------------------\n")
            print("----------ADD RANDO---------\n")
            print("Matrix 1: Row: " + str(row1) + " Col " + str(col1))
            dp_mat1, nc_mat1 = rand_dp_nc_matrix(row1, col1, seed=0)
            dp_mat2, nc_mat2 = rand_dp_nc_matrix(row1, col1, seed=1)
            is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
            # print("Matrix 1: Row: " + str(row1) + " Col " + str(col1))
            # print("Matrix 2: Row: " + str(row2) + " Col " + str(col2))
            self.assertTrue(is_correct)
            print_speedup(speed_up)
            print("---------------------------------------------------\n")


class TestSub(TestCase):
    def test_small_sub(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(2, 2, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(2, 2, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_sub(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(1492, 1791, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(1492, 1791, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_sub(self):
        # TODO: YOUR CODE HERE
        pass

    def test_special(self):
        # TODO: YOUR CODE HERE
        matrixRows = 3231
        matrixCols = 1
        iter = 100
        speedupArr = []

        # for i in range(iter):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
        self.assertTrue(is_correct)
        speedupArr.append(speed_up)
        print(speed_up)

    def test_special2(self):
        # TODO: YOUR CODE HERE
        matrixRows = 1
        matrixCols = 1282
        iter = 100
        speedupArr = []

        # for i in range(iter):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(matrixRows, matrixCols, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
        self.assertTrue(is_correct)
        speedupArr.append(speed_up)
        print(speed_up)

    def rando(self):
        low = 1
        high = 10000
        for i in range(low, high):
            row1 = random.randint(1, 2000)
            col1 = random.randint(1, 2000)
            print("---------------------------------------------------\n")
            print("----------SUB RANDO---------\n")
            print("Matrix 1: Row: " + str(row1) + " Col " + str(col1))
            print("Matrix 1: Row: " + str(row1) + " Col " + str(col1))
            dp_mat1, nc_mat1 = rand_dp_nc_matrix(row1, col1, seed=0)
            dp_mat2, nc_mat2 = rand_dp_nc_matrix(row1, col1, seed=1)
            is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
            # print("Matrix 1: Row: " + str(row1) + " Col " + str(col1))
            # print("Matrix 2: Row: " + str(row2) + " Col " + str(col2))
            self.assertTrue(is_correct)
            print_speedup(speed_up)
            print("---------------------------------------------------\n")


class TestAbs(TestCase):
    def test_small_abs(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_abs(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(250, 299, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_abs(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(1624, 1187, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_special(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(1, 1187, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_special2(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(1341, 1, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def rando(self):
        low = 1
        high = 10000
        for i in range(low, high):
            row1 = random.randint(1, 2000)
            col1 = random.randint(1, 2000)
            print("---------------------------------------------------\n")
            print("----------ABS RANDO---------\n")
            print("Matrix 1: Row: " + str(row1) + " Col " + str(col1))
            dp_mat, nc_mat = rand_dp_nc_matrix(row1, col1, seed=0)
            is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
            self.assertTrue(is_correct)
            print_speedup(speed_up)
            print("---------------------------------------------------\n")


class TestNeg(TestCase):
    def test_small_neg(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_neg(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(250, 500, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_neg(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(1599, 1299, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_special(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(1, 1299, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_special2(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(1343, 1, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def rando(self):
        low = 1
        high = 10000
        for i in range(low, high):
            row1 = random.randint(1, 2000)
            col1 = random.randint(1, 2000)
            print("---------------------------------------------------\n")
            print("----------NEG RANDO---------\n")
            print("Matrix 1: Row: " + str(row1) + " Col " + str(col1))
            dp_mat, nc_mat = rand_dp_nc_matrix(row1, col1, seed=0)
            is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
            self.assertTrue(is_correct)
            print_speedup(speed_up)
            print("---------------------------------------------------\n")


class TestMul(TestCase):
    def test_small_mul(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(2, 2, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(2, 2, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_mul(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(250, 250, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(250, 300, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_mul(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(1500, 1699, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(1699, 1500, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_mul1(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(1000, 1699, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(1699, 1500, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_mul2(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(500, 1699, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(1699, 1500, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_rando(self):
        # TODO: YOUR CODE HERE
        low = 1
        high = 10000

        for i in range(low, high):
            row1 = random.randint(1, 2000)
            col1 = random.randint(1, 2000)
            row2 = col1
            col2 = random.randint(1, 2000)
            print("---------------------------------------------------\n")
            print("----------MUL RANDO---------\n")
            print("Matrix 1: Row: " + str(row1) + " Col " + str(col1))
            print("Matrix 2: Row: " + str(row2) + " Col " + str(col2))
            dp_mat1, nc_mat1 = rand_dp_nc_matrix(row1, col1, seed=0)
            dp_mat2, nc_mat2 = rand_dp_nc_matrix(row2, col2, seed=1)
            is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
            # time.sleep(5)
            # print("Matrix 1: Row: " + str(row1) + " Col " + str(col1))
            # print("Matrix 2: Row: " + str(row2) + " Col " + str(col2))
            if not is_correct:
                print("Matrix 1: Row: " + str(row1) + " Col " + str(col1))
                print("Matrix 2: Row: " + str(row2) + " Col " + str(col2))
                print("not right")
                print("not right")
                print("not right")
                print("not right")
                print("not right")
                print("not right")
                print("not right")
                # print("MAT1: ", nc_mat1)
                # print("MAT2: ", nc_mat2)
            self.assertTrue(is_correct)
            print_speedup(speed_up)
            print("---------------------------------------------------\n")

    def test_prob(self):
        # TODO: YOUR CODE HERE
        print("Matrix 1: Row: " + str(1500) + " Col " + str(1500))
        print("Matrix 2: Row: " + str(1500) + " Col " + str(1500))
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(1500, 1500, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(1500, 1500, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    # TODO: COME BACK LATER, IMPORTANT CASE
    def test_prob1(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(1182, 1168, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(1168, 1, seed=1)
        is_correct, speed_up = compute([nc_mat1, nc_mat2], [nc_mat1, nc_mat2], "mul")
        # is_correct, speed_up = compute([dp_mat1, dp_mat2], [dp_mat1, dp_mat2], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)


class TestPow(TestCase):
    def test_small_pow(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        is_correct, speed_up = compute([dp_mat, 3], [nc_mat, 3], "pow")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_pow(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(100, 100, seed=1)
        is_correct, speed_up = compute([dp_mat, 1000], [nc_mat, 1000], "pow")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_special(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(1, 1, seed=1)
        is_correct, speed_up = compute([dp_mat, 1000], [nc_mat, 1000], "pow")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_pow(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(100, 100, seed=1)
        is_correct, speed_up = compute([dp_mat, 1000], [nc_mat, 1000], "pow")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def rando(self):
        # TODO: YOUR CODE HERE
        low = 1
        high = 10000
        for i in range(low, high):
            # row1 = random.randint(1, 2000)
            r_c = random.randint(2, 500)
            to = random.randint(0, 5000)
            # col1 = random.randint(1, 2000)
            # row2 = col1
            # col2 = random.randint(1, 2000)
            print("---------------------------------------------------\n")
            print("----------POW RANDO---------\n")
            print("Matrix 1: Row: " + str(r_c) + " Col " + str(r_c))
            print("Pow: " + str(to))
            dp_mat1, nc_mat1 = rand_dp_nc_matrix(r_c, r_c, seed=1)
            is_correct, speed_up = compute([dp_mat1, to], [nc_mat1, to], "pow")
            # print("Matrix 1: Row: " + str(row1) + " Col " + str(col1))
            # print("Matrix 2: Row: " + str(row2) + " Col " + str(col2))
            if not is_correct:
                print("Matrix 1: Row: " + str(r_c) + " Col " + str(r_c))
                print("Pow: " + str(to))
                print("not right")
                print("not right")
                print("not right")
                print("not right")
                print("not right")
                print("not right")
                print("not right")
                # print("MAT1: ", nc_mat1)
                # print("MAT2: ", nc_mat2)
            self.assertTrue(is_correct)
            print_speedup(speed_up)
            print("---------------------------------------------------\n")

    def weird(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(100, 100, seed=1)
        is_correct, speed_up = compute([dp_mat, 5000], [nc_mat, 5000], "pow")
        # print(dp_mat)
        # print("\n")
        # print("\n")
        # print("\n")
        # print(nc_mat)
        self.assertTrue(is_correct)
        print_speedup(speed_up)


class TestGet(TestCase):
    def test_get(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        rand_row = np.random.randint(dp_mat.shape[0])
        rand_col = np.random.randint(dp_mat.shape[1])
        self.assertEqual(round(dp_mat[rand_row][rand_col], decimal_places),
                         round(nc_mat[rand_row][rand_col], decimal_places))


class TestSet(TestCase):
    def test_set(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        rand_row = np.random.randint(dp_mat.shape[0])
        rand_col = np.random.randint(dp_mat.shape[1])
        self.assertEquals(round(dp_mat[rand_row][rand_col], decimal_places),
                          round(nc_mat[rand_row][rand_col], decimal_places))


class TestShape(TestCase):
    def test_shape(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        self.assertTrue(dp_mat.shape == nc_mat.shape)


class TestSanity(TestCase):

    def test_large_add(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(1500, 1500, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(1500, 1500, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_sub(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(1500, 1500, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(1500, 1500, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_abs(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(1624, 1187, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_neg(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(1599, 1299, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    # def test_small_mul(self):
    #     # TODO: YOUR CODE HERE
    #     dp_mat1, nc_mat1 = rand_dp_nc_matrix(2, 2, seed=0)
    #     dp_mat2, nc_mat2 = rand_dp_nc_matrix(2, 2, seed=1)
    #     is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
    #     self.assertTrue(is_correct)
    #     print_speedup(speed_up)
    #
    # def test_medium_mul(self):
    #     # TODO: YOUR CODE HERE
    #     dp_mat1, nc_mat1 = rand_dp_nc_matrix(250, 250, seed=0)
    #     dp_mat2, nc_mat2 = rand_dp_nc_matrix(250, 300, seed=1)
    #     is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
    #     self.assertTrue(is_correct)
    #     print_speedup(speed_up)
    #
    # def test_large_mul(self):
    #     # TODO: YOUR CODE HERE
    #     dp_mat1, nc_mat1 = rand_dp_nc_matrix(1500, 1699, seed=0)
    #     dp_mat2, nc_mat2 = rand_dp_nc_matrix(1699, 1500, seed=1)
    #     is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
    #     self.assertTrue(is_correct)
    #     print_speedup(speed_up)
    #
    # def test_large_mul1(self):
    #     # TODO: YOUR CODE HERE
    #     dp_mat1, nc_mat1 = rand_dp_nc_matrix(1000, 1699, seed=0)
    #     dp_mat2, nc_mat2 = rand_dp_nc_matrix(1699, 1500, seed=1)
    #     is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
    #     self.assertTrue(is_correct)
    #     print_speedup(speed_up)
    #
    # def test_large_mul2(self):
    #     # TODO: YOUR CODE HERE
    #     dp_mat1, nc_mat1 = rand_dp_nc_matrix(500, 1699, seed=0)
    #     dp_mat2, nc_mat2 = rand_dp_nc_matrix(1699, 1500, seed=1)
    #     is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
    #     self.assertTrue(is_correct)
    #     print_speedup(speed_up)
    #
    # def test_large_pow(self):
    #     # TODO: YOUR CODE HERE
    #     for i in range(0, 5000):
    #         r = random.randint(1, 100)
    #         dp_mat, nc_mat = rand_dp_nc_matrix(r, r, seed=1)
    #         is_correct, speed_up = compute([dp_mat, i], [nc_mat, i], "pow")
    #         self.assertTrue(is_correct)
    #         print_speedup(speed_up)


class TestSlice(TestCase):

    def test_add(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(500, 1699, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(500, 1699, seed=0)
        is_correct, speed_up = compute([dp_mat1[10:50, 10:50], dp_mat2[10:50, 10:50]],
                                       [nc_mat1[10:50, 10:50], nc_mat2[10:50, 10:50]], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_sub(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(500, 1699, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(500, 1699, seed=0)
        is_correct, speed_up = compute([dp_mat1[10:50, 10:50], dp_mat2[10:50, 10:50]],
                                       [nc_mat1[10:50, 10:50], nc_mat2[10:50, 10:50]], "sub")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_mul(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(500, 1699, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(500, 1699, seed=0)
        is_correct, speed_up = compute([dp_mat1[10:50, 10:100], dp_mat2[10:100, 10:50]],
                                       [nc_mat1[10:50, 10:100], nc_mat2[10:100, 10:50]], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_pow(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(500, 1699, seed=0)
        for i in range(0, 1):
            is_correct, speed_up = compute([dp_mat1[10:100, 10:100], i],
                                           [nc_mat1[10:100, 10:100], i], "pow")
            self.assertTrue(is_correct)
            print_speedup(speed_up)

    def test_neg(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(2000, 1699, seed=0)
        is_correct, speed_up = compute([dp_mat1[1:625, 10:100]],
                                       [nc_mat1[1:625, 10:100]], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_abs(self):
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(2000, 1699, seed=0)
        is_correct, speed_up = compute([dp_mat1[1:625, 10:100]],
                                       [nc_mat1[1:625, 10:100]], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)