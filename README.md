# numc

- **Important**
	- You will have 6 tokens every 24 hours for the Gradescope assignment.
	- You will get negative points up to the number of points it is worth if you fail to complete task 5
	- You can either allocate new matrices and throw errors in numc.c or matrix.c.
	- We will not be directly testing your C code. All tests will be in Python!
	- You may change the function signatures in matrix.h and matrix.c as you deem appropriate, but you may not change the function signatures in numc.h and numc.c.
	- You may change the skeleton code in numc.c.
	- You may NOT add/remove any additional imports.
	- If you would like to run the reference solution to compare your solution to, you can import the dumbpy library on hive as we have already installed it there for you! (```import dumbpy```)


# 

1. Virtual Environment
	1. ```python3.6 -m venv .venv```
	2. ```source .venv/bin/activate```
	3. ```pip3 install -r requirements.txt```
	4. ```deactivate```

# 

1. [x] **Task 1: Matrix functions in C**
	1. [x] Complete all functions in matrix.c
		1. [x] ```int allocate_matrix(matrix **mat, int rows, int cols)```
		2. [x] ```int allocate_matrix_ref(matrix **mat, matrix *from, int row_offset, int col_offset, int rows, int cols)```
		3. [x] ```void deallocate_matrix(matrix *mat)```
		4. [x] ```double get(matrix *mat, int row, int col)```
		5. [x] ```void set(matrix *mat, int row, int col, double val)```
		6. [x] ```void fill_matrix(matrix *mat, double val)```
		7. [x] ```int add_matrix(matrix *result, matrix *mat1, matrix *mat2)```
		8. [x] ```int sub_matrix(matrix *result, matrix *mat1, matrix *mat2)```
		9. [x] ```int mul_matrix(matrix *result, matrix *mat1, matrix *mat2)```
		10. [x] ```int pow_matrix(matrix *result, matrix *mat, int pow)```
		11. [x] ```int neg_matrix(matrix *result, matrix *mat)```
		12. [x] ```int abs_matrix(matrix *result, matrix *mat)```


	Summary:
	
	Task 1:
	
	    1. In order for memory to be aligned to 64 byte boundary, the memory aligned allocation was used along with memory aligned free. For this reason, manual zeroing of the memory was done using SIMD and Open MP.

	    2. For deallocate, a recursive algorithm was used, in order to exploit the underlying graph structure of matrix allocation. The idea holding on the law, deallocate will not be called twice on the same memory.
	
# 
2. [x] **Task 2: Writing the setup file**

	**Summary**: In this section of the assignment we refered to the links that [api reference](https://docs.python.org/3.6/distutils/apiref.html) and [c extensions](https://docs.python.org/3.6/extending/building.html). We also went through this [video](https://www.youtube.com/watch?v=CYDakDJv2p4) and the idea here was that we added compliation flags that we deemed would improve performance, like ```-O3``` and ```-Ofast``` but also understand that some of the flags will hinder accuracy.
	
# 

3. [x] **Task 3: Writing the Python-C interface**
	1. [x] Complete all functions in numc.c
		1. [x] ```PyObject *Matrix61c_add(Matrix61c *self, PyObject *args)```
		2. [x] ```PyObject *Matrix61c_sub(Matrix61c *self, PyObject *args)```
		3. [x] ```PyObject *Matrix61c_multiply(Matrix61c *self, PyObject *args)```
		4. [x] ```PyObject *Matrix61c_neg(Matrix61c *self)```
		5. [x] ```‌PyObject *Matrix61c_abs(Matrix61c *self)```
		6. [x] ```PyObject *Matrix61c_pow(Matrix61c *self, PyObject *pow, PyObject *optional)```
		7. [x] ```PyObject *Matrix61c_set_value(Matrix61c *self, PyObject *args)```
		8. [x] ```PyObject *Matrix61c_get_value(Matrix61c *self, PyObject *args)```
		9. [x] ```PyObject *Matrix61c_subscript(Matrix61c *self, PyObject *key)```
		10. [x] ```int Matrix61c_set_subscript(Matrix61c *self, PyObject *key, PyObject *v)```
			11. [x] ```int set_subscript_helper(Matrix61c *self, PyObject *v, Py_ssize_t left_self_rows, Py_ssize_t left_self_cols, Py_ssize_t left_row_offset, Py_ssize_t left_col_offset)```

	**Summary**:

# 

4. [x] **Task 4: Speeding up matrix operations**

	**Summary**: (Task 4: PERFORMANCE AND OPTIMIZATION)

	In order to increase the performance of the provided functions, the following strategies were implemented:

	Task 4: PERFORMANCE AND OPTIMIZATION

	In order to increase the performance of the provided functions, the following strategies were implemented:

	1. Cache Blocking:

		    a) Cache blocking, as covered in the Labs, was implemented to enhance the performance of memory accesses and writes.

		    In order to perform cache blocking, the following strategies were in place:

	        1) In the very beginning, research was done on the Intel Microprocessors with respect to their memory specifications, L1, L2, L3.

	            With that in place, the memory allocation was placed on the 64 byte boundary, aligned with the caches, in order to prevent false sharing, low hit rate, etc.

	        2) Following that, the multiplication algorithm was implemented using the block size of 32, which was manually tuned to perform the best on the Hive machines.

	    b) Several experiments were performed to find the optimal factors for cache performance.

	2. SIMD:

	    a) Intel Intrinsics were used extensively wherever possible. Several bit hacks were used in conjunction with SIMD to improve performance.

	3. OPEN MP:

	    a) Open MP pragmas were used wherever appropriate to improve performance.

	4. INFINITY AND ALL:

	    a) Floating point Infinity rule: Infinity + x = Infinity was exploited in power. The idea being if infinity why perform more operations on it.

	    b) For negate and absolute functions, the boolean bit arithmetic was used.

	        1) In Negate, xoring the value with -0.0 was performed to result in negation.

	        2) In abs, and not of -0.0 was performed to result in absolute value.
	
	In addition, bit methods were used to perform min and finding the row and col 64 byte boundary.

# 

5. [x] **Task 5: Tell us what you did!**

	Write up what you did in your README.md! While we do not have a specific format we are looking for, you should discuss what you did as a whole, the different python functions you implemented, what performance improvements you had, what were you surprised about, etc. More specifically, we want you to document your numc module and provide example usages. We expect a minimum of 6000 characters. This may seem like a lot, but documenting you’ve done actually takes quite a number of words, so just sufficiently covering all the things you did in the project should get you to the word count requirement. Again, failure to complete this task may result in negative points, so make sure you do it or you will lose points! (Read above for answers. Below are examples.)

# 

- **api examples**

	- matrix initialization
		- you can pass in the dimensions in a ```list_of_lists``` where each inside lists is a new row and the amount of numbers in the list is the column number
			
			```
			m1 = nc.Matrix(list_of_lists)
			m1 = nc.Matrix([[1.0, 2.0, 3.0], [4.0, 5.0, 6.0], [7.0, -8.0, 9.0]])
			```

		- you can pass in the number of ```rows``` and ```cols``` and all the values in a matrix of that dimension will be set to 0

			```
			aa = dp.Matrix(rows, cols);
			aa = dp.Matrix(1, 2);
			```

		- you can pass in the ```rows```, ```cols``` and a ```number`` to fill the matrix with and that will fill every stop with that number

			```
			aa = dp.Matrix(rows, cols, number)
			aa = dp.Matrix(1, 2, 4)
			```
			
	- matrix slicing
		- you can slice the matrix by passing in indexes to slice the rows and cols

			```
			>>> x2 = dp.Matrix([[1.0, 2.0, 3.0, 4.0, 5.0], [6.0, 7.0, 8.0, 9.0, 10.0], [11.0, 12.0, 13.0, 14.0, 15.0]])
			>>> x2[1:2, 2:3]
			8.0
			```

		- you can also assign with the slice as long as the dimension match

			```
			>>> x2 = dp.Matrix([[1.0, 2.0, 3.0, 4.0, 5.0], [6.0, 7.0, 8.0, 9.0, 10.0], [11.0, 12.0, 13.0, 14.0, 15.0]])
			>>> x2[1:2, 2:3]
			8.0
			>>> x2[1:2, 2:3] = 4
			```

- instance methods

	- there is also an instance method get that takes in ```rows``` and ```cols```

		```
		>>> x2 = dp.Matrix([[1.0, 2.0, 3.0, 4.0, 5.0], [6.0, 7.0, 8.0, 9.0, 10.0], [11.0, 12.0, 13.0, 14.0, 15.0]])
		>>> # x2.get(rows, cols)
		>>> x2.get(1, 2)
		4.0
		```

	- there is a set instance method as well that take ```rows```, ```cols``` and the ```value``` that you want to set to

		```
		>>> x2.set(rows, cols, value)
		>>> x2.set(1, 2, 100)
		```

- instance variables

	- there is an instance field called ```shape``` that returns a tuple of the dimensions.

		```
		>>> x2 = dp.Matrix([[1.0, 2.0, 3.0, 4.0, 5.0], [6.0, 7.0, 8.0, 9.0, 10.0], [11.0, 12.0, 13.0, 14.0, 15.0]])
		>>> x2.shape
		(3, 5)
		```
		



