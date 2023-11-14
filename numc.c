// version 9
#include "numc.h"
#include <structmember.h>
// TODO : for computer
//#include "/Library/Frameworks/Python.framework/Versions/3.7/include/python3.7m/structmember.h"

PyTypeObject Matrix61cType;

/* Helper functions for initalization of matrices and vectors */

/*
 * Return a tuple given rows and cols
 */
PyObject *get_shape(int rows, int cols) {
    if (rows == 1 || cols == 1) {
        return PyTuple_Pack(1, PyLong_FromLong(rows * cols));
    } else {
        return PyTuple_Pack(2, PyLong_FromLong(rows), PyLong_FromLong(cols));
    }
}

/*
 * Matrix(rows, cols, low, high). Fill a matrix random double values
 */
int init_rand(PyObject *self, int rows, int cols, unsigned int seed, double low, double high) {
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    rand_matrix(new_mat, seed, low, high);
    ((Matrix61c *) self)->mat = new_mat;
    ((Matrix61c *) self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * Matrix(rows, cols, val). Fill a matrix of dimension rows * cols with val
 */
int init_fill(PyObject *self, int rows, int cols, double val) {
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed)
        return alloc_failed;
    else {
        fill_matrix(new_mat, val);
        ((Matrix61c *) self)->mat = new_mat;
        ((Matrix61c *) self)->shape = get_shape(new_mat->rows, new_mat->cols);
    }
    return 0;
}

/*
 * Matrix(rows, cols, 1d_list). Fill a matrix with dimension rows * cols with 1d_list values
 */
int init_1d(PyObject *self, int rows, int cols, PyObject *lst) {
    if (rows * cols != PyList_Size(lst)) {
        PyErr_SetString(PyExc_ValueError, "Incorrect number of elements in list");
        return -1;
    }
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    int count = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            set(new_mat, i, j, PyFloat_AsDouble(PyList_GetItem(lst, count)));
            count++;
        }
    }
    ((Matrix61c *) self)->mat = new_mat;
    ((Matrix61c *) self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * Matrix(2d_list). Fill a matrix with dimension len(2d_list) * len(2d_list[0])
 */
int init_2d(PyObject *self, PyObject *lst) {
    int rows = PyList_Size(lst);
    if (rows == 0) {
        PyErr_SetString(PyExc_ValueError, "Cannot initialize numc.Matrix with an empty list");
        return -1;
    }
    int cols;
    if (!PyList_Check(PyList_GetItem(lst, 0))) {
        PyErr_SetString(PyExc_ValueError, "List values not valid");
        return -1;
    } else {
        cols = PyList_Size(PyList_GetItem(lst, 0));
    }
    for (int i = 0; i < rows; i++) {
        if (!PyList_Check(PyList_GetItem(lst, i)) ||
            PyList_Size(PyList_GetItem(lst, i)) != cols) {
            PyErr_SetString(PyExc_ValueError, "List values not valid");
            return -1;
        }
    }
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            set(new_mat, i, j,
                PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(lst, i), j)));
        }
    }
    ((Matrix61c *) self)->mat = new_mat;
    ((Matrix61c *) self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * This deallocation function is called when reference count is 0
 */
void Matrix61c_dealloc(Matrix61c *self) {
    deallocate_matrix(self->mat);
    Py_TYPE(self)->tp_free(self);
}

/* For immutable types all initializations should take place in tp_new */
PyObject *Matrix61c_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    /* size of allocated memory is tp_basicsize + nitems*tp_itemsize*/
    Matrix61c *self = (Matrix61c *) type->tp_alloc(type, 0);
    return (PyObject *) self;
}

/*
 * This matrix61c type is mutable, so needs init function. Return 0 on success otherwise -1
 */
int Matrix61c_init(PyObject *self, PyObject *args, PyObject *kwds) {
    /* Generate random matrices */
    if (kwds != NULL) {
        PyObject *rand = PyDict_GetItemString(kwds, "rand");
        if (!rand) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
        if (!PyBool_Check(rand)) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
        if (rand != Py_True) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }

        PyObject *low = PyDict_GetItemString(kwds, "low");
        PyObject *high = PyDict_GetItemString(kwds, "high");
        PyObject *seed = PyDict_GetItemString(kwds, "seed");
        double double_low = 0;
        double double_high = 1;
        unsigned int unsigned_seed = 0;

        if (low) {
            if (PyFloat_Check(low)) {
                double_low = PyFloat_AsDouble(low);
            } else if (PyLong_Check(low)) {
                double_low = PyLong_AsLong(low);
            }
        }

        if (high) {
            if (PyFloat_Check(high)) {
                double_high = PyFloat_AsDouble(high);
            } else if (PyLong_Check(high)) {
                double_high = PyLong_AsLong(high);
            }
        }

        if (double_low >= double_high) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }

        // Set seed if argument exists
        if (seed) {
            if (PyLong_Check(seed)) {
                unsigned_seed = PyLong_AsUnsignedLong(seed);
            }
        }

        PyObject *rows = NULL;
        PyObject *cols = NULL;
        if (PyArg_UnpackTuple(args, "args", 2, 2, &rows, &cols)) {
            if (rows && cols && PyLong_Check(rows) && PyLong_Check(cols)) {
                return init_rand(self, PyLong_AsLong(rows), PyLong_AsLong(cols), unsigned_seed, double_low, double_high);
            }
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
    }
    PyObject *arg1 = NULL;
    PyObject *arg2 = NULL;
    PyObject *arg3 = NULL;
    if (PyArg_UnpackTuple(args, "args", 1, 3, &arg1, &arg2, &arg3)) {
        /* arguments are (rows, cols, val) */
        if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) && (PyLong_Check(arg3) || PyFloat_Check(arg3))) {
            if (PyLong_Check(arg3)) {
                return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), PyLong_AsLong(arg3));
            } else
                return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), PyFloat_AsDouble(arg3));
        } else if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) && PyList_Check(arg3)) {
            /* Matrix(rows, cols, 1D list) */
            return init_1d(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), arg3);
        } else if (arg1 && PyList_Check(arg1) && arg2 == NULL && arg3 == NULL) {
            /* Matrix(rows, cols, 1D list) */
            return init_2d(self, arg1);
        } else if (arg1 && arg2 && PyLong_Check(arg1) && PyLong_Check(arg2) && arg3 == NULL) {
            /* Matrix(rows, cols, 1D list) */
            return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), 0);
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return -1;
    }
}

/*
 * List of lists representations for matrices
 */
PyObject *Matrix61c_to_list(Matrix61c *self) {
    int rows = self->mat->rows;
    int cols = self->mat->cols;
    PyObject *py_lst = NULL;
    if (self->mat->is_1d) {  // If 1D matrix, print as a single list
        py_lst = PyList_New(rows * cols);
        int count = 0;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                PyList_SetItem(py_lst, count, PyFloat_FromDouble(get(self->mat, i, j)));
                count++;
            }
        }
    } else {  // if 2D, print as nested list
        py_lst = PyList_New(rows);
        for (int i = 0; i < rows; i++) {
            PyList_SetItem(py_lst, i, PyList_New(cols));
            PyObject *curr_row = PyList_GetItem(py_lst, i);
            for (int j = 0; j < cols; j++) {
                PyList_SetItem(curr_row, j, PyFloat_FromDouble(get(self->mat, i, j)));
            }
        }
    }
    return py_lst;
}

PyObject *Matrix61c_class_to_list(Matrix61c *self, PyObject *args) {
    PyObject *mat = NULL;
    if (PyArg_UnpackTuple(args, "args", 1, 1, &mat)) {
        if (!PyObject_TypeCheck(mat, &Matrix61cType)) {
            PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
            return NULL;
        }
        Matrix61c *mat61c = (Matrix61c *) mat;
        return Matrix61c_to_list(mat61c);
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }
}

/*
 * Add class methods
 */
PyMethodDef Matrix61c_class_methods[] = {
        {"to_list", (PyCFunction) Matrix61c_class_to_list, METH_VARARGS, "Returns a list representation of numc.Matrix"},
        {NULL, NULL, 0, NULL}
};

/*
 * Matrix61c string representation. For printing purposes.
 */
PyObject *Matrix61c_repr(PyObject *self) {
    PyObject *py_lst = Matrix61c_to_list((Matrix61c *) self);
    return PyObject_Repr(py_lst);
}

/* NUMBER METHODS */

/*
 * Add the second numc.Matrix (Matrix61c) object to the first one. The first operand is
 * self, and the second operand can be obtained by casting `args`.
 */
PyObject *Matrix61c_add(Matrix61c *self, PyObject *args) {
    /* TODO: YOUR CODE HERE */

    if (!PyObject_TypeCheck(args, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "numc.matrix does not support + with other types");
        return NULL;
    }
    Matrix61c *other = (Matrix61c *) args;
    int rows = other->mat->rows;
    int cols = other->mat->cols;

    if (self->mat->rows != rows || self->mat->cols != cols) {
        PyErr_SetString(PyExc_ValueError, "Arguments' dimensions invalid");
        return NULL;
    }

    matrix *total;

    if (allocate_matrix(&total, rows, cols)) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix");
        return NULL;
    }

    // if it fails then is some non zero number
    // TODO : can take out as this will only return 0
    if (add_matrix(total, self->mat, other->mat)) {
        PyErr_SetString(PyExc_RuntimeError, "Adding matrix function failed");
        deallocate_matrix(total);
        return NULL;
    }

    Matrix61c *newMatrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
    if (newMatrix == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix");
        deallocate_matrix(total);
        return NULL;
    }
    newMatrix->mat = total;

    /** do this so that the format is correct for the print with 1 (5,) instead of (5,1) */
    if (rows == 1) {
        newMatrix->shape = PyTuple_Pack(1, PyLong_FromLong(cols));
    }

    else if (cols == 1) {
        newMatrix->shape = PyTuple_Pack(1, PyLong_FromLong(rows));
    }

    else {
        newMatrix->shape = PyTuple_Pack(2, PyLong_FromLong(rows), PyLong_FromLong(cols));
    }

    return (PyObject *) newMatrix;
    // return Py_BuildValue("s", "ryan Matrix61c_add not finished");
}

/*
 * Substract the second numc.Matrix (Matrix61c) object from the first one. The first operand is
 * self, and the second operand can be obtained by casting `args`.
 */
PyObject *Matrix61c_sub(Matrix61c *self, PyObject *args) {
    /* TODO: YOUR CODE HERE */

    if (!PyObject_TypeCheck(args, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "numc.matrix does not support - with other types");
        return NULL;
    }
    Matrix61c *other = (Matrix61c *) args;
    int rows = other->mat->rows;
    int cols = other->mat->cols;

    if (self->mat->rows != rows || self->mat->cols != cols) {
        PyErr_SetString(PyExc_ValueError, "Arguments' dimensions invalid");
        return NULL;
    }

    matrix *total;

    if (allocate_matrix(&total, rows, cols)) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix");
        return NULL;
    }

    // if it fails then is some non zero number
    // TODO : can take out as this will only return 0
    if (sub_matrix(total, self->mat, other->mat)) {
        PyErr_SetString(PyExc_RuntimeError, "Subtracting matrix function failed");
        deallocate_matrix(total);
        return NULL;
    }

    Matrix61c *newMatrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
    if (newMatrix == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix");
        deallocate_matrix(total);
        return NULL;
    }
    newMatrix->mat = total;

    /** do this so that the format is correct for the print with 1 (5,) instead of (5,1) */
    if (rows == 1) {
        newMatrix->shape = PyTuple_Pack(1, PyLong_FromLong(cols));
    }

    else if (cols == 1) {
        newMatrix->shape = PyTuple_Pack(1, PyLong_FromLong(rows));
    }

    else {
        newMatrix->shape = PyTuple_Pack(2, PyLong_FromLong(rows), PyLong_FromLong(cols));
    }

    return (PyObject *) newMatrix;
    //return Py_BuildValue("s", "ryan Matrix61c_sub not finished");
}

/*
 * NOT element-wise multiplication. The first operand is self, and the second operand
 * can be obtained by casting `args`.
 */
PyObject *Matrix61c_multiply(Matrix61c *self, PyObject *args) {
    /* TODO: YOUR CODE HERE */

    if (!PyObject_TypeCheck(args, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "numc.matrix does not support * with other types");
        return NULL;
    }
    Matrix61c *other = (Matrix61c *) args;
    int rows = other->mat->rows;
//    int cols = other->mat->cols;

    if (self->mat->cols != rows) {
        PyErr_SetString(PyExc_ValueError, "Arguments' dimensions invalid");
        return NULL;
    }

    matrix *total;
    int newRows = self->mat->rows;
    int newCols = other->mat->cols;

    if (allocate_matrix(&total, newRows, newCols)) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix");
        return NULL;
    }

    // if it fails then is some non zero number
    // TODO : can take out as this will only return 0
    if (mul_matrix(total, self->mat, other->mat)) {
        PyErr_SetString(PyExc_RuntimeError, "Multiplying matrix function failed");
        deallocate_matrix(total);
        return NULL;
    }

    Matrix61c *newMatrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
    if (newMatrix == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix");
        deallocate_matrix(total);
        return NULL;
    }
    newMatrix->mat = total;

    /** do this so that the format is correct for the print with 1 (5,) instead of (5,1) */
    if (newRows == 1) {
        newMatrix->shape = PyTuple_Pack(1, PyLong_FromLong(newCols));
    }

    else if (newCols == 1) {
        newMatrix->shape = PyTuple_Pack(1, PyLong_FromLong(newRows));
    }

    else {
        newMatrix->shape = PyTuple_Pack(2, PyLong_FromLong(newRows), PyLong_FromLong(newCols));
    }


    return (PyObject *) newMatrix;
    // return Py_BuildValue("s", "ryan Matrix61c_multiply not finished");
}

/*
 * Negates the given numc.Matrix.
 */
PyObject *Matrix61c_neg(Matrix61c *self) {
    /* TODO: YOUR CODE HERE */

    int rows = self->mat->rows;
    int cols = self->mat->cols;

    matrix *total;

    if (allocate_matrix(&total, rows, cols)) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix");
        return NULL;
    }

    // if it fails then is some non zero number
    // TODO : can take out as this will only return 0
    if (neg_matrix(total, self->mat)) {
        PyErr_SetString(PyExc_RuntimeError, "Negate matrix function failed");
        deallocate_matrix(total);
        return NULL;
    }

    Matrix61c *newMatrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
    if (newMatrix == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix");
        deallocate_matrix(total);
        return NULL;
    }
    newMatrix->mat = total;

    /** do this so that the format is correct for the print with 1 (5,) instead of (5,1) */
    if (rows == 1) {
        newMatrix->shape = PyTuple_Pack(1, PyLong_FromLong(cols));
    }

    else if (cols == 1) {
        newMatrix->shape = PyTuple_Pack(1, PyLong_FromLong(rows));
    }

    else {
        newMatrix->shape = PyTuple_Pack(2, PyLong_FromLong(rows), PyLong_FromLong(cols));
    }

    return (PyObject *) newMatrix;
    // return Py_BuildValue("s", "ryan Matrix61c_neg not finished");
}

/*
 * Take the element-wise absolute value of this numc.Matrix.
 */
PyObject *Matrix61c_abs(Matrix61c *self) {
    /* TODO: YOUR CODE HERE */

    int rows = self->mat->rows;
    int cols = self->mat->cols;

    matrix *total;


    if (allocate_matrix(&total, rows, cols)) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix");
        return NULL;
    }

    // if it fails then is some non zero number
    // TODO : can take out as this will only return 0
    if (abs_matrix(total, self->mat)) {
        PyErr_SetString(PyExc_RuntimeError, "Absolute value matrix function failed");
        deallocate_matrix(total);
        return NULL;
    }

    Matrix61c *newMatrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
    if (newMatrix == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix");
        deallocate_matrix(total);
        return NULL;
    }
    newMatrix->mat = total;

    /** do this so that the format is correct for the print with 1 (5,) instead of (5,1) */
    if (rows == 1) {
        newMatrix->shape = PyTuple_Pack(1, PyLong_FromLong(cols));
    }

    else if (cols == 1) {
        newMatrix->shape = PyTuple_Pack(1, PyLong_FromLong(rows));
    }

    else {
        newMatrix->shape = PyTuple_Pack(2, PyLong_FromLong(rows), PyLong_FromLong(cols));
    }

    return (PyObject *) newMatrix;
    //return Py_BuildValue("s", "ryan Matrix61c_abs not finished");
}

/*
 * Raise numc.Matrix (Matrix61c) to the `pow`th power. You can ignore the argument `optional`.
 */
PyObject *Matrix61c_pow(Matrix61c *self, PyObject *pow, PyObject *optional) {
    /* TODO: YOUR CODE HERE */

    if (!PyLong_Check(pow)) {
        PyErr_SetString(PyExc_TypeError, "Exponent must be of type integer");
        return NULL;
    }

    int rows = self->mat->rows;
    int cols = self->mat->cols;
    int power = PyLong_AsLong(pow);

    if (rows != cols) {
        PyErr_SetString(PyExc_ValueError, "Matrix must be square");
        return NULL;
    }

    if (power < 0) {
        PyErr_SetString(PyExc_ValueError, "Exponent must be positive");
        return NULL;
    }

    matrix *total;

    if (allocate_matrix(&total, rows, cols)) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix");
        return NULL;
    }


    // if it fails then is some non zero number
    // TODO : can take out as this will only return 0
    if (pow_matrix(total, self->mat, power)) {
        PyErr_SetString(PyExc_RuntimeError, "Power matrix function failed");
        deallocate_matrix(total);
        return NULL;
    }

    Matrix61c *newMatrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
    if (newMatrix == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate matrix");
        deallocate_matrix(total);
        return NULL;
    }
    newMatrix->mat = total;

    /** do this so that the format is correct for the print with 1 (5,) instead of (5,1) */
    if (rows == 1) {
        newMatrix->shape = PyTuple_Pack(1, PyLong_FromLong(cols));
    }

    else if (cols == 1) {
        newMatrix->shape = PyTuple_Pack(1, PyLong_FromLong(rows));
    }

    else {
        newMatrix->shape = PyTuple_Pack(2, PyLong_FromLong(rows), PyLong_FromLong(cols));
    }


    return (PyObject *) newMatrix;
    // return Py_BuildValue("s", "ryan Matrix61c_pow not finished");
}


/*
 * Create a PyNumberMethods struct for overloading operators with all the number methods you have
 * define. You might find this link helpful: https://docs.python.org/3.6/c-api/typeobj.html
 */
PyNumberMethods Matrix61c_as_number = {
        /* TODO: YOUR CODE HERE */
        .nb_add = (binaryfunc) Matrix61c_add,
        .nb_subtract = (binaryfunc) Matrix61c_sub,
        .nb_multiply = (binaryfunc) Matrix61c_multiply,
        .nb_power = (ternaryfunc) Matrix61c_pow,
        .nb_negative = (unaryfunc) Matrix61c_neg,
        .nb_absolute = (unaryfunc) Matrix61c_abs
};


/* INSTANCE METHODS */

/*
 * Given a numc.Matrix self, parse `args` to (int) row, (int) col, and (double/int) val.
 * Return None in Python (this is different from returning null).
 */
PyObject *Matrix61c_set_value(Matrix61c *self, PyObject *args) {
    /* TODO: YOUR CODE HERE */

    PyObject *arg1 = NULL;
    PyObject *arg2 = NULL;
    PyObject *arg3 = NULL;

    // TODO : fix 1 to 3 ... to 3 to 3 ... nevermind it looks good because the arg will be none otherwise
    if (PyArg_UnpackTuple(args, "args", 1, 3, &arg1, &arg2, &arg3)) {

        if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) && (PyLong_Check(arg3) || PyFloat_Check(arg3))) {
            int row = (int) PyLong_AsLong(arg1);
            int col = (int) PyLong_AsLong(arg2);

            if (row < 0 || col < 0 || row >= self->mat->rows || col >= self->mat->cols) {
                PyErr_SetString(PyExc_IndexError, "row or column index out of range");
                return NULL; // TODO: IS IT NONE? ... I think this is correct because we are throwing an error
            }

            double val = (double) (PyFloat_Check(arg3) ? PyFloat_AsDouble(arg3) : PyLong_AsDouble(arg3));

            set(self->mat, row, col, val);
        }
        else {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return NULL;
        }
    }
    else {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }

    return Py_None;
    // return Py_BuildValue("s", "ryan Matrix61c_set_value not finished");
}

/*
 * Given a numc.Matrix `self`, parse `args` to (int) row and (int) col.
 * Return the value at the `row`th row and `col`th column, which is a Python
 * float/int.
 */
PyObject *Matrix61c_get_value(Matrix61c *self, PyObject *args) {
    /* TODO: YOUR CODE HERE */

    PyObject *arg1 = NULL;
    PyObject *arg2 = NULL;

    if (!PyArg_UnpackTuple(args, "args", 2, 2, &arg1, &arg2)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }

    if (!(arg1 && arg2 && PyLong_Check(arg1) && PyLong_Check(arg2))) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }

    int row = (int) PyLong_AsLong(arg1);
    int col = (int) PyLong_AsLong(arg2);

    if (row < 0 || col < 0 || row >= self->mat->rows || col >= self->mat->cols) {
        PyErr_SetString(PyExc_IndexError, "row or column index out of range");
        return NULL; // TODO: IS IT NONE? ... RYAN: no this is good I think
    }

    double retVal = get(self->mat, row, col);

    return PyFloat_FromDouble(retVal);
    // return Py_BuildValue("s", "ryan Matrix61c_get_value not finished");
}

/*
 * Create an array of PyMethodDef structs to hold the instance methods.
 * Name the python function corresponding to Matrix61c_get_value as "get" and Matrix61c_set_value
 * as "set"
 * You might find this link helpful: https://docs.python.org/3.6/c-api/structures.html
 */
PyMethodDef Matrix61c_methods[] = {
        /* TODO: YOUR CODE HERE */
        {"set", (PyCFunction) Matrix61c_set_value, METH_VARARGS, "Change the value at a specific row and column index"},
        {"get", (PyCFunction) Matrix61c_get_value, METH_VARARGS, "Get the value at a specific row and column index"},
        {NULL, NULL, 0, NULL}
};

/* INDEXING */

/** function that takes in v and returns a number representing which case
 * the number falls into -- this is a helper function for Matrix61c_set_subscript */
/** return 0 if success and otherwise -1 */
int set_subscript_helper(Matrix61c *self, PyObject *v, Py_ssize_t left_self_rows, Py_ssize_t left_self_cols, Py_ssize_t left_row_offset, Py_ssize_t left_col_offset) {

//    printf("rows: %zd\n", left_self_rows);
//    printf("cols: %zd\n", left_self_cols);
//    printf("sum: %zd\n", left_self_rows + left_self_cols);

    /** new case definitions */
    // (section 1) If the resulting slice after indexing is 1 by 1, then v should be an integer or a float.
    // (section 2) If it is an 1D matrix (1 by n or n by 1), then v should be a list of numbers, and each number can either be int or float, and the length of the list should be the same as the number of elements of this slice.
    // (section 3) If it is 2D, then v should be a 2D list where the ith element of this list is a 1D list of integers/floats to set the ith row of the slice.

    // create a function that returns 0, 1, 2, 3 that tells me which case v falls into and if it falls into none then that will be 0

    // new case 1:

    /** create a double list that holds row * cols doubles */
    // TODO : need to free
    double *new_vals_list = (double *) calloc(left_self_rows * left_self_cols, sizeof(double));

    /** case definitions */
    // case 1: 1d matrix with a key that is an integer
    // case 2: 1d matrix with a key that is a single slice
    // case 3: 2d matrix with a key that is an integer
    // case 4: 2d matrix with a key that is a single slice
    // case 5: 2d matrix with a key that is a tuple with contents: (a1: integer, a2: integer)
    // case 6: 2d matrix with a key that is a tuple with contents: (a1: integer, a2: slice)
    // case 7: 2d matrix with a key that is a tuple with contents: (a1: slice, a2: integer)
    // case 8: 2d matrix with a key that is a tuple with contents: (a1: slice, a2: slice)

    /** variable that is 1 (true) if v is a good value and 0 (false) if v is a bad value
     * set the default to false */
    int success = 0;

    /** enter here if the matrix shape based on the index (left-side) is 1 by 1 ... NUMBER */
    if (left_self_rows == 1 && left_self_cols == 1) {

        // printf("entered (target 1)\n");

        /** in this section v should be a float/int */

        /** if v is not a float/int */
        if (!(PyLong_Check(v) || PyFloat_Check(v))) {

            /** free the new_vals_list because code is returning */
            free(new_vals_list);

            PyErr_SetString(PyExc_TypeError, "Value is not valid");
            return -1;
        }

        /** here v is a float/int */
        else {
            success = 1;

            /** add the double to the list */
            new_vals_list[0] = PyFloat_AsDouble(v);
        }

    }

    /** enter here if the rows is 1 or if cols is 1 ... 1D MATRIX */
    else if (left_self_rows == 1 || left_self_cols == 1) {

        /** in this section v should be a list with float/ints */

        /** enter here if v is not a list */
        if (!(PyList_Check(v))) {

            /** free the new_vals_list because code is returning */
            free(new_vals_list);

            PyErr_SetString(PyExc_TypeError, "Value is not valid");
            return -1;
        }

        /** enter here if v is a list */
        else {

            /** get the length of the outside list (list) */
            Py_ssize_t inside_list_length = PyList_Size(v);

            /** get the value that is not equal to 1 inside this loop */
            Py_ssize_t non_one_value = (left_self_rows > left_self_cols) ? (left_self_rows) : (left_self_cols);

            /** check to make sure that the dimensions match */
            if (inside_list_length != non_one_value) {

                /** free the new_vals_list because code is returning */
                free(new_vals_list);

                PyErr_SetString(PyExc_ValueError, "Value is not valid");
                return -1;
            }

            /** loop through all the items in the list, now that we know the dimensions match */
            for (int index1 = 0; index1 < inside_list_length; index1 += 1) {

                /** get the item at the index */
                PyObject *outside_list_item = PyList_GetItem(v, index1);

                /** if the item is not a float/int */
                if (!(PyLong_Check(outside_list_item) || PyFloat_Check(outside_list_item))) {

                    /** free the new_vals_list because code is returning */
                    free(new_vals_list);

                    PyErr_SetString(PyExc_ValueError, "Value is not valid");
                    return -1;
                }

                /** add the item to the double list */
                new_vals_list[index1] = PyFloat_AsDouble(outside_list_item);
            }

            /** we got a list of doubles */
            success = 1;
        }
    }

    /** enter here if the rows and cols are both not 1 ... 2D MATRIX */
    else {

        /** in this section v should be a list of lists with float/ints */

        /** enter here if v is not a list */
        if (!(PyList_Check(v))) {

            /** free the new_vals_list because code is returning */
            free(new_vals_list);

            PyErr_SetString(PyExc_TypeError, "Value is not valid");
            return -1;
        }

        /** enter here if v is a list */
        else {

            /** get the length of the outside list (list) ... NUMBER OF ROWS */
            Py_ssize_t inside_list_length = PyList_Size(v);

            /** check to make sure that the ROW dimensions match */
            if (inside_list_length != left_self_rows) {

                /** free the new_vals_list because code is returning */
                free(new_vals_list);

                PyErr_SetString(PyExc_ValueError, "Value is not valid");
                return -1;
            }

            /** loop through all the items in the list, now that we now we have the correct ROW dimensions */
            for (int index1 = 0; index1 < inside_list_length; index1 += 1) {

                /** get the item at the index */
                PyObject *outside_list_item = PyList_GetItem(v, index1);

                /** if the item is not a list */
                if (!(PyList_Check(outside_list_item))) {

                    /** free the new_vals_list because code is returning */
                    free(new_vals_list);

                    PyErr_SetString(PyExc_ValueError, "Value is not valid");
                    return -1;
                }

                /** enter here if the value is a list */
                else {

                    /** get the length of the inside list (list of lists) */
                    Py_ssize_t inside_list_list_length = PyList_Size(outside_list_item);

                    /** check to make sure that the COL dimensions match */
                    if (inside_list_list_length != left_self_cols) {

                        /** free the new_vals_list because code is returning */
                        free(new_vals_list);

                        PyErr_SetString(PyExc_ValueError, "Value is not valid");
                        return -1;
                    }

                    /** loop through all the items in the list of lists, now that we now we have the correct COL dimensions */
                    for (int index2 = 0; index2 < inside_list_list_length; index2 += 1) {

                        /** get the item at the index */
                        PyObject *inside_list_item = PyList_GetItem(outside_list_item, index2);

                        /** if the item is not a float/int */
                        if (!(PyLong_Check(inside_list_item) || PyFloat_Check(inside_list_item))) {

                            /** free the new_vals_list because code is returning */
                            free(new_vals_list);

                            PyErr_SetString(PyExc_ValueError, "Value is not valid");
                            return -1;
                        }

                        /** add the item to the double list putting the values in row major order */
                        new_vals_list[index1 * left_self_cols + index2] = PyFloat_AsDouble(inside_list_item);
                    }
                }
            }
            /** we got a list of lists with doubles */
            success = 1;
        }
    }

    /** now I have either left the code or made it to this point because the input was valid */

    /** if the input was valid */
    if (success) {

        // FIXME : take out
//        printf("n1: %f\n", new_vals_list[0]);
//        printf("n1: %f\n", new_vals_list[1]);
//        printf("n1: %f\n", new_vals_list[2]);
//        printf("n1: %f\n", new_vals_list[3]);
//
//        printf("\n\nls_rows: %d\n", left_self_rows);
//        printf("ls_cols: %d\n", left_self_cols);
//
//        printf("row_offset: %d\n", left_row_offset);
//        printf("col_offset: %d\n", left_col_offset);

        /** loop through all the set values in the list ... need to add the starting point to not just be 0 idiot */
        // FIXME : major bug right here -- should be better
        for (int ro = left_row_offset; ro < (left_row_offset + left_self_rows); ro += 1) {
            for (int co = left_col_offset; co < (left_col_offset + left_self_cols); co += 1) {

                // FIXME : take out
                // printf("numbers_list (in order): %f\n", new_vals_list[(ro - left_row_offset) * left_self_cols + (co - left_col_offset)]);

                /** set the values to the matrix itself */
                set(self->mat, ro, co, new_vals_list[(ro - left_row_offset) * left_self_cols + (co - left_col_offset)]);
                // such an idiot!
                // set(self->mat, ro, co, new_vals_list[ro * left_self_cols + co]);
            }
        }

        /* free the list, now that I am done with it */
        free(new_vals_list);

        return 0;
    }

    /** something went wrong -- should never be at this point */
    else {
        PyErr_SetString(PyExc_NotImplementedError, "something went wrong");
        return -1;
    }
}

/*
 * Given a numc.Matrix `self`, index into it with `key`. Return the indexed result.
 */
PyObject *Matrix61c_subscript(Matrix61c *self, PyObject *key) {
    /* TODO: YOUR CODE HERE */

    /** check to make sure that the values are not null */
    if (self == NULL || self->mat == NULL) {
        PyErr_SetString(PyExc_NotImplementedError, "(-1) need to make correct");
        return NULL;
    }

    int is_1d = self->mat->is_1d;
    int case_number = 0;

    /** get the rows and columns of the given matrix self->mat */
    int self_rows = self->mat->rows;
    int self_cols = self->mat->cols;

    /** the provided matrix */
    matrix* given_matrix = self->mat;

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------Used-By-Everything-Else-----------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // (cases 1-6)
    /** set up the variables for the PySlice_GetIndicesEx */
    Py_ssize_t start;
    Py_ssize_t stop;
    Py_ssize_t step_size;
    Py_ssize_t length;
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------


    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------Only-Used-By-Set-With-First-Slice-Or-Both-Slice-----------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // (case 7 and 8 only)
    /** set up the variables for the PySlice_GetIndicesEx (outside double case) */
    Py_ssize_t start_outside;
    Py_ssize_t stop_outside;
    Py_ssize_t step_size_outsize;
    Py_ssize_t length_outsize;

    /** set up the variables for the PySlice_GetIndicesEx (inside double case) */
    Py_ssize_t start_inside;
    Py_ssize_t stop_inside;
    Py_ssize_t step_size_inside;
    Py_ssize_t length_inside;
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // (cases 5-8)
    /** now need to check to see what is inside the tuple to make sure that it is valid */
    // could be either two slices/ints
    PyObject *arguement_one;
    PyObject *arguement_two;
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------

    // PyLong_Check(key)
    // PySlice_Check(key)
    // PyTuple_Check(key)
    // PyArg_UnpackTuple(key, "key", 2, 2, &one, &two)

    /** enter here if we have a 1d matrix (case 1-2) */
    // if is_1d is positive or negative but just not equal to 0
    if (is_1d) {

        /** if the key is an integer (case 1) */
        if (PyLong_Check(key)) {
            case_number = 1;
        }

        /** if the key is a single slice */
        // TODO : slice
        else if (PySlice_Check(key)) {

            /** get the max of the numbers between the rows and cols so we know what the highest index could be */
            // int largest_sequence_length = (self_rows > self_cols) ? (self_rows - 1) : (self_cols - 1);
            int largest_sequence_length = (self_rows > self_cols) ? (self_rows) : (self_cols);

            /** get value to use below: check to see if the slice worked */
            // TODO : check largest_sequence_length
            int failed_to_get_slice_values = PySlice_GetIndicesEx((PyObject *) key, largest_sequence_length, &start, &stop, &step_size, &length);

            /** the slice function failed */
            if (failed_to_get_slice_values) {
                // the PySlice_GetIndicesEx set the error message so just need to return null
                // this will also check to make sure they are integers or none
                // done
                return NULL;
            }

            /** the slice function worked, valid slice (case 2) */
            else {
                case_number = 2;
            }
        }

        /** the key is not valid for a 1d matrix */
        else {
            /** if the key is a tuple, we want different error message */
            if (PyTuple_Check(key)) {
                // done
                PyErr_SetString(PyExc_TypeError, "1D matrices only support single slice!");
                return NULL;
            }
            else {
                // done
                PyErr_SetString(PyExc_TypeError, "Key is not valid");
                return NULL;
            }
        }
    }

    /** enter here if we have a 2d matrix */
     // if is_1d is equal to 0
    else if (!is_1d) {
        // the key could either be an integer, a single slice, or a tuple of two slices/ints

        /** if the key is an integer (case 3) */
        if (PyLong_Check(key)) {
            case_number = 3;
        }

        /** if the key is a single slice */
        // TODO : slice
        else if (PySlice_Check(key)) {

            /** get the max of the numbers between the rows and cols so we know what the highest index could be */
            // int largest_sequence_length = (self_rows > self_cols) ? (self_rows - 1) : (self_cols - 1);
            // int largest_sequence_length = (self_rows > self_cols) ? (self_rows) : (self_cols);
            // this just refers to the rows
            int largest_sequence_length = self_rows;

            /** get value to use below: check to see if the slice worked */
            // TODO : check largest_sequence_length -- yep good thing I checked this
            int failed_to_get_slice_values = PySlice_GetIndicesEx((PyObject *) key, largest_sequence_length, &start, &stop, &step_size, &length);

            /** the slice function failed */
            if (failed_to_get_slice_values) {
                // the PySlice_GetIndicesEx set the error message so just need to return null
                // this will also check to make sure they are integers or none
                // done
                return NULL;
            }

            /** the slice function worked, valid slice (case 4) */
            else {
                case_number = 4;
            }
        }

        /** if the key is a tuple */
        else if (PyTuple_Check(key)) {

            /** if cannot unpack the tuple */
            if (!PyArg_UnpackTuple(key, "key", 2, 2, &arguement_one, &arguement_two)) {
                // done
                PyErr_SetString(PyExc_TypeError, "Invalid arguments");
                return NULL;
            }

            /** if can unpack the tuple */
            else {

                /** if arguement_one is an integer */
                if (PyLong_Check(arguement_one)) {

                    /** if arguement_two is an integer (case 5) */
                    if (PyLong_Check(arguement_two)) {
                        case_number = 5;
                    }

                    /** if arguement_two is a slice */
                    // TODO : slice
                    else if (PySlice_Check(arguement_two)) {

                        /** get the max of the numbers between the rows and cols so we know what the highest index could be */
                        // int largest_sequence_length = (self_rows > self_cols) ? (self_rows - 1) : (self_cols - 1);
                        // int largest_sequence_length = (self_rows > self_cols) ? (self_rows) : (self_cols);
                        // this just refers to the cols
                        int largest_sequence_length = self_cols;

                        /** get value to use below: check to see if the slice worked */
                        // TODO : check largest_sequence_length -- yep good thing I checked this
                        int failed_to_get_slice_values = PySlice_GetIndicesEx((PyObject *) arguement_two, largest_sequence_length, &start, &stop, &step_size, &length);

                        /** the slice function failed */
                        if (failed_to_get_slice_values) {
                            // the PySlice_GetIndicesEx set the error message so just need to return null
                            // this will also check to make sure they are integers or none
                            // done
                            return NULL;
                        }

                        /** the slice function worked, valid slice (case 6) */
                        else {
                            case_number = 6;
                        }
                    }

                    /** arguement_two is not valid */
                    else {
                        // done
                        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
                        return NULL;
                    }

                }

                /** if arguement_one is a slice */
                // TODO : slice
                else if (PySlice_Check(arguement_one)) {

                    /** get the max of the numbers between the rows and cols so we know what the highest index could be */
                    // int largest_sequence_length = (self_rows > self_cols) ? (self_rows - 1) : (self_cols - 1);
                    // int largest_sequence_length = (self_rows > self_cols) ? (self_rows) : (self_cols);
                    // this just refers to the rows
                    int largest_sequence_length = self_rows;

                    /** get value to use below: check to see if the slice worked */
                    // TODO : check largest_sequence_length -- yep good thing I checked this
                    int failed_to_get_slice_values = PySlice_GetIndicesEx((PyObject *) arguement_one, largest_sequence_length, &start_outside, &stop_outside, &step_size_outsize, &length_outsize);

                    /** the slice function failed */
                    if (failed_to_get_slice_values) {
                        // the PySlice_GetIndicesEx set the error message so just need to return null
                        // this will also check to make sure they are integers or none
                        // done
                        return NULL;
                    }

                    /** the slice function worked, valid slice */
                    else {

                        /** if arguement_two is an integer (case 7) */
                        if (PyLong_Check(arguement_two)) {
                            case_number = 7;
                        }

                        /** if arguement_two is a slice */
                        // TODO : slice
                        else if (PySlice_Check(arguement_two)) {

                            /** get the max of the numbers between the rows and cols so we know what the highest index could be */
                            // int largest_sequence_length = (self_rows > self_cols) ? (self_rows - 1) : (self_cols - 1);
                            // int largest_sequence_length = (self_rows > self_cols) ? (self_rows) : (self_cols);
                            // this just refers to the cols
                            int largest_sequence_length = self_cols;

                            /** get value to use below: check to see if the slice worked */
                            // TODO : check largest_sequence_length -- yep good thing I checked this
                            int failed_to_get_slice_values = PySlice_GetIndicesEx((PyObject *) arguement_two, largest_sequence_length, &start_inside, &stop_inside, &step_size_inside, &length_inside);

                            /** the slice function failed */
                            if (failed_to_get_slice_values) {
                                // the PySlice_GetIndicesEx set the error message so just need to return null
                                // this will also check to make sure they are integers or none
                                // done
                                return NULL;
                            }

                            /** the slice function worked, valid slice (case 8) */
                            else {
                                case_number = 8;
                            }
                        }

                        /** arguement_two is not valid */
                        else {
                            // done
                            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
                            return NULL;
                        }
                    }
                }

                /** if arguement_one is neither an integer or a slice */
                else {
                    // now we do not have to check arguement_two because arguement_one is not valid
                    // done
                    PyErr_SetString(PyExc_TypeError, "Invalid arguments");
                    return NULL;
                }
            }
        }

        /** the key is not valid for a 2d matrix */
        else {
            // here the key is not valid so we do not have to check what it is actually we know that it is wrong
            // done
            PyErr_SetString(PyExc_TypeError, "Key is not valid");
            return NULL;
        }
    }

    /** enter here if we have neither a 1d or 2d matrix */
    else {
        // this should never happen as we should not have neither a 1d or 2d matrix
        PyErr_SetString(PyExc_NotImplementedError, "(7) need to make correct");
        return NULL;
    }


    /** case definitions */
    // case 1: 1d matrix with a key that is an integer
    // case 2: 1d matrix with a key that is a single slice
    // case 3: 2d matrix with a key that is an integer
    // case 4: 2d matrix with a key that is a single slice
    // case 5: 2d matrix with a key that is a tuple with contents: (a1: integer, a2: integer)
    // case 6: 2d matrix with a key that is a tuple with contents: (a1: integer, a2: slice)
    // case 7: 2d matrix with a key that is a tuple with contents: (a1: slice, a2: integer)
    // case 8: 2d matrix with a key that is a tuple with contents: (a1: slice, a2: slice)

    // TODO : something that need to remember to check is: when to return a number if happens to be 1 by 1 matrix -- did not do this on some!


    switch (case_number) {

        /** case 1: 1d matrix with a key that is an integer */
        // TODO : done
        case 1: {
            // uncomment line below to print
            // printf("Choice is 1\n");

            // get the index_key as an integer
            long index_key = PyLong_AsLong(key);

            /** if the rows of the 1d matrix is 1 */
            if (self_rows == 1) {
                // now we know that the self_rows is 1 and the self_cols is a number greater then or equal to 1

                /** if the index_key is not in range of the 1d matrix */
                if (index_key >= self_cols || index_key < 0) {
                    // done
                    PyErr_SetString(PyExc_IndexError, "Index out of range");
                    return NULL;
                }

                /** the index_key is in range of the 1d matrix */
                else {
                    return PyFloat_FromDouble(get(given_matrix, 0, index_key));
                }
            }

            /** if the cols of the 1d matrix is 1 */
            else if (self_cols == 1) {
                // now we know that the self_cols is 1 and the self_rows is a number greater then or equal to 1

                /** if the index_key is not in range of the 1d matrix */
                if (index_key >= self_rows || index_key < 0) {
                    // done
                    PyErr_SetString(PyExc_IndexError, "Index out of range");
                    return NULL;
                }

                /** the index_key is in range of the 1d matrix */
                else {
                    return PyFloat_FromDouble(get(given_matrix, index_key, 0));
                }
            }

            /** this was not a 1d matrix and was wrongly identified */
            else {
                PyErr_SetString(PyExc_NotImplementedError, "(b.2) need to make correct");
                return NULL;
            }

            break;
        }
        /** case 2: 1d matrix with a key that is a single slice */
        // TODO : done
        case 2: {
            // uncomment line below to print
            // printf("Choice is 2\n");

            /** here the slice information is invalid since the start is greater then or equal to stop */
            // stop cannot be treated as stop by definition rather just the max(self_rows, self_cols)
            if (start >= stop || step_size != 1) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return NULL;
            }

            /** if the rows of the 1d matrix is 1 */
            if (self_rows == 1) {

                /** here the slice length is 1 */
                if ((stop - start == 1) || (self_cols - start == 1)) {
                    // return the item at that spot
                    return PyFloat_FromDouble(get(given_matrix, 0, start));
                }

                /** here the slice is not length 1 */
                else {
                    matrix *matrix_to_return = NULL;
                    int success = allocate_matrix_ref(&matrix_to_return, given_matrix, 0, start, 1, stop - start);

                    /** need to check that allocate memory worked */
                    if (success != 0) {
                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful memory allocation.");
                        return NULL;
                    }

                    Matrix61c *python_matrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);

                    /** need to check that the new matrix call worked */
                    if (python_matrix == NULL) {
                        deallocate_matrix(matrix_to_return);
                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful matrix61c function call.");
                        return NULL;
                    }

                    python_matrix->mat = matrix_to_return;
                    python_matrix->shape = PyTuple_Pack(1, PyLong_FromLong(stop - start));
                    return (PyObject *) python_matrix;
                }
            }

            /** if the cols of the 1d matrix is 1 */
            else if (self_cols == 1) {

                /** here the slice length is 1 */
                if ((stop - start == 1) || (self_rows - start == 1)) {
                    // return the item at that spot
                    return PyFloat_FromDouble(get(given_matrix, start, 0));
                }

                /** here the slice is not length 1 */
                else {
                    matrix *matrix_to_return = NULL;
                    int success = allocate_matrix_ref(&matrix_to_return, given_matrix, start, 0, stop - start, 1);

                    /** need to check that allocate memory worked */
                    if (success != 0) {
                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful memory allocation.");
                        return NULL;
                    }

                    Matrix61c *python_matrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);

                    /** need to check that the new matrix call worked */
                    if (python_matrix == NULL) {
                        deallocate_matrix(matrix_to_return);
                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful matrix61c function call.");
                        return NULL;
                    }

                    python_matrix->mat = matrix_to_return;
                    python_matrix->shape = PyTuple_Pack(1, PyLong_FromLong(stop - start));
                    return (PyObject *) python_matrix;
                }

            }

            /** this was not a 1d matrix and was wrongly identified */
            else {
                PyErr_SetString(PyExc_NotImplementedError, "(r.2) need to make correct");
                return NULL;
            }

            break;
        }
        /** case 3: 2d matrix with a key that is an integer */
        // TODO : done
        case 3: {
            // uncomment line below to print
            //printf("Choice is 3\n");

            /** get the index value */
            long case_3_index_key = PyLong_AsLong(key);

            /** checking to see if the index value is in range of the matrix */
            // must be only looking at the rows
            if ((case_3_index_key < 0) || (case_3_index_key >= self_rows)) {
                PyErr_SetString(PyExc_IndexError, "Index out of range");
                return NULL;
            }

            /** here the given index is correct */
            else {
                matrix *matrix_to_return = NULL;
                int success = allocate_matrix_ref(&matrix_to_return, given_matrix, case_3_index_key, 0, 1, self_cols);

                /** need to check that allocate memory worked */
                if (success != 0) {
                    PyErr_SetString(PyExc_RuntimeError, "Unsuccessful memory allocation.");
                    return NULL;
                }

                Matrix61c *python_matrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);

                /** need to check that the new matrix call worked */
                if (python_matrix == NULL) {
                    deallocate_matrix(matrix_to_return);
                    PyErr_SetString(PyExc_RuntimeError, "Unsuccessful matrix61c function call.");
                    return NULL;
                }

                python_matrix->mat = matrix_to_return;
                python_matrix->shape = PyTuple_Pack(1, PyLong_FromLong(self_cols));
                return (PyObject *) python_matrix;
            }

            break;
        }
        /** case 4: 2d matrix with a key that is a single slice */
        // TODO : done
        case 4: {
            // uncomment line below to print
            //printf("Choice is 4\n");

            /** need to check if the slice is valid ... here the slice has the same start stop */
            if (start == stop) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return NULL;
            }

            /** enter here if the left index is greater then the rows or the start is negative techinacally do not need to worry
             * because the python function will make it positive */
            else if (start >= self_rows || start < 0) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return NULL;
            }

            /** enter here if the left index is greater then the right index */
            else if (start >= stop) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return NULL;
            }

            /** enter here if the step size is not 1 */
            else if (step_size != 1) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return NULL;
            }

            /** enter here if nothing was wrong with the numbers */
            else {
                matrix *matrix_to_return = NULL;
                Py_ssize_t new_rows = stop - start;

                int success = allocate_matrix_ref(&matrix_to_return, given_matrix, start, 0, new_rows, self_cols);

                /** need to check that allocate memory worked */
                if (success != 0) {
                    PyErr_SetString(PyExc_RuntimeError, "Unsuccessful memory allocation.");
                    return NULL;
                }

                Matrix61c *python_matrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);

                /** need to check that the new matrix call worked */
                if (python_matrix == NULL) {
                    deallocate_matrix(matrix_to_return);
                    PyErr_SetString(PyExc_RuntimeError, "Unsuccessful matrix61c function call.");
                    return NULL;
                }

                python_matrix->mat = matrix_to_return;

                /** check whether this is a 1d matrix */
                /** if enters here then is a 1d matrix */
                if (new_rows == 1) {
                    python_matrix->shape = PyTuple_Pack(1, PyLong_FromLong(self_cols));
                }

                /** if enters here then is a 1d matrix */
                else if (self_cols == 1) {
                    python_matrix->shape = PyTuple_Pack(1, PyLong_FromLong(new_rows));
                }

                /** if enters here then is not a 1d matrix */
                else {
                    python_matrix->shape = PyTuple_Pack(2, PyLong_FromLong(new_rows), PyLong_FromLong(self_cols));
                }

                return (PyObject *) python_matrix;
            }

            break;
        }
        /** case 5: 2d matrix with a key that is a tuple with contents: (a1: integer, a2: integer) */
        // TODO : done
        case 5: {
            // uncomment line below to print
            // printf("Choice is 5\n");

            /** extract the values from the two indexes */
            long a1 = PyLong_AsLong(arguement_one);
            long a2 = PyLong_AsLong(arguement_two);

            /** check to see if the first input is less then 0 or greater/equal then to the number of rows */
            if (a1 < 0 || a1 >= self_rows) {
                PyErr_SetString(PyExc_IndexError, "Index out of range!");
                return NULL;
            }

                /** check to see if the second input is less then 0 or greater/equal then to the number of cols */
            else if (a2 < 0 || a2 >= self_cols) {
                PyErr_SetString(PyExc_IndexError, "Index out of range!");
                return NULL;
            }

                /** here the given value is correct ... this will always just result in a number because it starts as a 2d matrix */
            else {
                return PyFloat_FromDouble(get(given_matrix, a1, a2));
            }

            break;
        }
        /** case 6: 2d matrix with a key that is a tuple with contents: (a1: integer, a2: slice) */
        // TODO : done
        case 6: {
            // uncomment line below to print
            // printf("Choice is 6\n");

            /** extract the values from the tuple */
            long int_arg = PyLong_AsLong(arguement_one);


            /** check to see if the first input is less then 0 or greater/equal then to the number of rows */
            if (int_arg < 0 || int_arg >= self_rows) {
                PyErr_SetString(PyExc_IndexError, "Index out of range!");
                return NULL;
            }

            /** need to check if the slice is valid ... here the slice has the same start stop */
            if (start == stop) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return NULL;
            }

                /** enter here if the left index is greater then the cols or the start is negative technically do not need to worry
                 * because the python function will make it positive */
            else if (start >= self_cols || start < 0) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return NULL;
            }

                /** enter here if the left index is greater then the right index */
            else if (start >= stop) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return NULL;
            }

                /** enter here if the step size is not 1 */
            else if (step_size != 1) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return NULL;
            }

                /** enter here if nothing was wrong with the numbers */
            else {
                Py_ssize_t new_cols = stop - start;

                /** need to check to see if I have a 1 by 1 matrix */
                if (new_cols == 1) {
                    return PyFloat_FromDouble(get(given_matrix, int_arg, start));
                }

                    /** I do not have a 1 by 1 matrix */
                else {
                    matrix *matrix_to_return = NULL;
                    /** will always have 1 row because the first index is just a number */
                    int success = allocate_matrix_ref(&matrix_to_return, given_matrix, int_arg, start, 1, new_cols);

                    /** need to check that allocate memory worked */
                    if (success != 0) {
                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful memory allocation.");
                        return NULL;
                    }

                    Matrix61c *python_matrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);

                    /** need to check that the new matrix call worked */
                    if (python_matrix == NULL) {
                        deallocate_matrix(matrix_to_return);
                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful matrix61c function call.");
                        return NULL;
                    }

                    python_matrix->mat = matrix_to_return;
                    python_matrix->shape = PyTuple_Pack(1, PyLong_FromLong(new_cols));
                    return (PyObject *) python_matrix;
                }
            }

            break;
        }
        /** case 7: 2d matrix with a key that is a tuple with contents: (a1: slice, a2: integer) */
        // TODO : done
        case 7: {
            // uncomment line below to print
            // printf("Choice is 7\n");

            /** extract the values from the tuple */
            long int_arg_two = PyLong_AsLong(arguement_two);

            /** check to see if the second input is less then 0 or greater/equal then to the number of cols */
            if (int_arg_two < 0 || int_arg_two >= self_cols) {
                PyErr_SetString(PyExc_IndexError, "Index out of range!");
                return NULL;
            }

            /** need to check if the slice is valid ... here the slice has the same start stop */
            if (start_outside == stop_outside) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return NULL;
            }

                /** enter here if the left index is greater then the rows or the start is negative technically do not need
                 * to worry because the python function will make it positive */
            else if (start_outside >= self_rows || start_outside < 0) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return NULL;
            }

                /** enter here if the left index is greater then the right index */
            else if (start_outside >= stop_outside) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return NULL;
            }

                /** enter here if the step size is not 1 */
            else if (step_size_outsize != 1) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return NULL;
            }

                /** enter here if nothing was wrong with the numbers */
            else {
                Py_ssize_t new_rows = stop_outside - start_outside;

                /** need to check to see if I have a 1 by 1 matrix */
                if (new_rows == 1) {
                    return PyFloat_FromDouble(get(given_matrix, start_outside, int_arg_two));
                }

                    /** I do not have a 1 by 1 matrix */
                else {
                    matrix *matrix_to_return = NULL;
                    /** will always have 1 col because the second index is just a number */
                    int success = allocate_matrix_ref(&matrix_to_return, given_matrix, start_outside, int_arg_two, new_rows, 1);

                    /** need to check that allocate memory worked */
                    if (success != 0) {
                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful memory allocation.");
                        return NULL;
                    }

                    Matrix61c *python_matrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);

                    /** need to check that the new matrix call worked */
                    if (python_matrix == NULL) {
                        deallocate_matrix(matrix_to_return);
                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful matrix61c function call.");
                        return NULL;
                    }

                    python_matrix->mat = matrix_to_return;
                    python_matrix->shape = PyTuple_Pack(1, PyLong_FromLong(new_rows));
                    return (PyObject *) python_matrix;
                }
            }

            break;
        }
        /** case 8: 2d matrix with a key that is a tuple with contents: (a1: slice, a2: slice) */
        // TODO : done
        case 8: {
            // uncomment line below to print
            // printf("Choice is 8\n");

            /** need to check if the slice is valid ... here the slice has the same start stop */
            if (start_outside == stop_outside || start_inside == stop_inside) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return NULL;
            }

            /** enter here if the left index is greater then the rows or the start is negative technically do not need
             * to worry because the python function will make it positive */
            else if ((start_outside >= self_rows || start_outside < 0) || (start_inside >= self_cols || start_inside < 0)) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return NULL;
            }

            /** enter here if the left index is greater then the right index */
            else if ((start_outside >= stop_outside) || (start_inside >= stop_inside)) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return NULL;
            }

            /** enter here if the step size is not 1 */
            else if ((step_size_outsize != 1) || step_size_inside != 1) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return NULL;
            }

            /** enter here if nothing was wrong with the numbers */
            else {
                Py_ssize_t new_rows = stop_outside - start_outside;
                Py_ssize_t new_cols = stop_inside - start_inside;

                /** need to check to see if I have a 1 by 1 matrix */
                if ((new_rows == 1) && (new_cols == 1)) {
                    return PyFloat_FromDouble(get(given_matrix, start_outside, start_inside));
                }

                /** I do not have a 1 by 1 matrix */
                else {
                    matrix *matrix_to_return = NULL;

                    /** will always have not always have 1 col or 1 row because the second index is also a slice */
                    int success = allocate_matrix_ref(&matrix_to_return, given_matrix, start_outside, start_inside, new_rows, new_cols);

                    /** need to check that allocate memory worked */
                    if (success != 0) {
                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful memory allocation.");
                        return NULL;
                    }

                    Matrix61c *python_matrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);

                    /** need to check that the new matrix call worked */
                    if (python_matrix == NULL) {
                        deallocate_matrix(matrix_to_return);
                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful matrix61c function call.");
                        return NULL;
                    }

                    python_matrix->mat = matrix_to_return;

                    /** check if rows is 1 */
                    if (new_rows == 1) {
                        python_matrix->shape = PyTuple_Pack(1, PyLong_FromLong(new_cols));
                    }

                    /** check if cols is 1 */
                    else if (new_cols == 1) {
                        python_matrix->shape = PyTuple_Pack(1, PyLong_FromLong(new_rows));
                    }

                    /** neither rows or cols is one */
                    else {
                        python_matrix->shape = PyTuple_Pack(2, PyLong_FromLong(new_rows), PyLong_FromLong(new_cols));
                    }

                    return (PyObject *) python_matrix;
                }
            }

            break;
        }
        /** default case: none of the other cases hit */
        // TODO : done -- added NotImplementedError
        default: {
            // uncomment line below to print
            // printf("Choice is default\n");

            PyErr_SetString(PyExc_NotImplementedError, "Matrix not supported, switch statement error.");
            return NULL;
            break;
        }
    }

    return Py_BuildValue("s", "ryan Matrix61c_subscript not finished");
}

/*
 * Given a numc.Matrix `self`, index into it with `key`, and set the indexed result to `v`.
 */
/** return 0 if success and otherwise -1 */
// NULL == -1 (in this function)
int Matrix61c_set_subscript(Matrix61c *self, PyObject *key, PyObject *v) {
    /* TODO: YOUR CODE HERE */
    // -----------------------

    /** check to make sure that the values are not null */
    if (self == NULL || self->mat == NULL) {
        PyErr_SetString(PyExc_NotImplementedError, "(-1) need to make correct");
        return -1;
    }

    int is_1d = self->mat->is_1d;
    int case_number = 0;

    /** get the rows and columns of the given matrix self->mat */
    int self_rows = self->mat->rows;
    int self_cols = self->mat->cols;

    /** the provided matrix */
//    matrix* given_matrix = self->mat;

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------Used-By-Everything-Else-----------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // (cases 1-6)
    /** set up the variables for the PySlice_GetIndicesEx */
    Py_ssize_t start;
    Py_ssize_t stop;
    Py_ssize_t step_size;
    Py_ssize_t length;
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------


    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------Only-Used-By-Set-With-First-Slice-Or-Both-Slice-----------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // (case 7 and 8 only)
    /** set up the variables for the PySlice_GetIndicesEx (outside double case) */
    Py_ssize_t start_outside;
    Py_ssize_t stop_outside;
    Py_ssize_t step_size_outsize;
    Py_ssize_t length_outsize;

    /** set up the variables for the PySlice_GetIndicesEx (inside double case) */
    Py_ssize_t start_inside;
    Py_ssize_t stop_inside;
    Py_ssize_t step_size_inside;
    Py_ssize_t length_inside;
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------

    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // (cases 5-8)
    /** now need to check to see what is inside the tuple to make sure that it is valid */
    // could be either two slices/ints
    PyObject *arguement_one;
    PyObject *arguement_two;
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------------------------------------------------

    // PyLong_Check(key)
    // PySlice_Check(key)
    // PyTuple_Check(key)
    // PyArg_UnpackTuple(key, "key", 2, 2, &one, &two)

    /** enter here if we have a 1d matrix (case 1-2) */
    // if is_1d is positive or negative but just not equal to 0
    if (is_1d) {

        /** if the key is an integer (case 1) */
        if (PyLong_Check(key)) {
            case_number = 1;
        }

            /** if the key is a single slice */
            // TODO : slice
        else if (PySlice_Check(key)) {

            /** get the max of the numbers between the rows and cols so we know what the highest index could be */
            // int largest_sequence_length = (self_rows > self_cols) ? (self_rows - 1) : (self_cols - 1);
            int largest_sequence_length = (self_rows > self_cols) ? (self_rows) : (self_cols);

            /** get value to use below: check to see if the slice worked */
            // TODO : check largest_sequence_length
            int failed_to_get_slice_values = PySlice_GetIndicesEx((PyObject *) key, largest_sequence_length, &start, &stop, &step_size, &length);

            /** the slice function failed */
            if (failed_to_get_slice_values) {
                // the PySlice_GetIndicesEx set the error message so just need to return null
                // this will also check to make sure they are integers or none
                // done
                return -1;
            }

                /** the slice function worked, valid slice (case 2) */
            else {
                case_number = 2;
            }
        }

            /** the key is not valid for a 1d matrix */
        else {
            /** if the key is a tuple, we want different error message */
            if (PyTuple_Check(key)) {
                // done
                PyErr_SetString(PyExc_TypeError, "1D matrices only support single slice!");
                return -1;
            }
            else {
                // done
                PyErr_SetString(PyExc_TypeError, "Key is not valid");
                return -1;
            }
        }
    }

    /** enter here if we have a 2d matrix */
    // if is_1d is equal to 0
    else if (!is_1d) {
        // the key could either be an integer, a single slice, or a tuple of two slices/ints

        /** if the key is an integer (case 3) */
        if (PyLong_Check(key)) {
            case_number = 3;
        }

        /** if the key is a single slice */
        // TODO : slice
        else if (PySlice_Check(key)) {

            /** get the max of the numbers between the rows and cols so we know what the highest index could be */
            // int largest_sequence_length = (self_rows > self_cols) ? (self_rows - 1) : (self_cols - 1);
            // int largest_sequence_length = (self_rows > self_cols) ? (self_rows) : (self_cols);
            // this just refers to the rows
            int largest_sequence_length = self_rows;

            /** get value to use below: check to see if the slice worked */
            // TODO : check largest_sequence_length -- yep good thing I checked this
            int failed_to_get_slice_values = PySlice_GetIndicesEx((PyObject *) key, largest_sequence_length, &start, &stop, &step_size, &length);

            /** the slice function failed */
            if (failed_to_get_slice_values) {
                // the PySlice_GetIndicesEx set the error message so just need to return null
                // this will also check to make sure they are integers or none
                // done
                return -1;
            }

            /** the slice function worked, valid slice (case 4) */
            else {
                case_number = 4;
            }
        }

        /** if the key is a tuple */
        else if (PyTuple_Check(key)) {

            /** if cannot unpack the tuple */
            if (!PyArg_UnpackTuple(key, "key", 2, 2, &arguement_one, &arguement_two)) {
                // done
                PyErr_SetString(PyExc_TypeError, "Invalid arguments");
                return -1;
            }

            /** if can unpack the tuple */
            else {

                /** if arguement_one is an integer */
                if (PyLong_Check(arguement_one)) {

                    /** if arguement_two is an integer (case 5) */
                    if (PyLong_Check(arguement_two)) {
                        case_number = 5;
                    }

                    /** if arguement_two is a slice */
                    // TODO : slice
                    else if (PySlice_Check(arguement_two)) {

                        /** get the max of the numbers between the rows and cols so we know what the highest index could be */
                        // int largest_sequence_length = (self_rows > self_cols) ? (self_rows - 1) : (self_cols - 1);
                        // int largest_sequence_length = (self_rows > self_cols) ? (self_rows) : (self_cols);
                        // this just refers to the cols
                        int largest_sequence_length = self_cols;

                        /** get value to use below: check to see if the slice worked */
                        // TODO : check largest_sequence_length -- yep good thing I checked this
                        int failed_to_get_slice_values = PySlice_GetIndicesEx((PyObject *) arguement_two, largest_sequence_length, &start, &stop, &step_size, &length);

                        /** the slice function failed */
                        if (failed_to_get_slice_values) {
                            // the PySlice_GetIndicesEx set the error message so just need to return null
                            // this will also check to make sure they are integers or none
                            // done
                            return -1;
                        }

                        /** the slice function worked, valid slice (case 6) */
                        else {
                            case_number = 6;
                        }
                    }

                    /** arguement_two is not valid */
                    else {
                        // done
                        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
                        return -1;
                    }

                }

                /** if arguement_one is a slice */
                // TODO : slice
                else if (PySlice_Check(arguement_one)) {

                    /** get the max of the numbers between the rows and cols so we know what the highest index could be */
                    // int largest_sequence_length = (self_rows > self_cols) ? (self_rows - 1) : (self_cols - 1);
                    // int largest_sequence_length = (self_rows > self_cols) ? (self_rows) : (self_cols);
                    // this just refers to the rows
                    int largest_sequence_length = self_rows;

                    /** get value to use below: check to see if the slice worked */
                    // TODO : check largest_sequence_length -- yep good thing I checked this
                    int failed_to_get_slice_values = PySlice_GetIndicesEx((PyObject *) arguement_one, largest_sequence_length, &start_outside, &stop_outside, &step_size_outsize, &length_outsize);

                    /** the slice function failed */
                    if (failed_to_get_slice_values) {
                        // the PySlice_GetIndicesEx set the error message so just need to return null
                        // this will also check to make sure they are integers or none
                        // done
                        return -1;
                    }

                    /** the slice function worked, valid slice */
                    else {

                        /** if arguement_two is an integer (case 7) */
                        if (PyLong_Check(arguement_two)) {
                            case_number = 7;
                        }

                        /** if arguement_two is a slice */
                        // TODO : slice
                        else if (PySlice_Check(arguement_two)) {

                            /** get the max of the numbers between the rows and cols so we know what the highest index could be */
                            // int largest_sequence_length = (self_rows > self_cols) ? (self_rows - 1) : (self_cols - 1);
                            // int largest_sequence_length = (self_rows > self_cols) ? (self_rows) : (self_cols);
                            // this just refers to the cols
                            int largest_sequence_length = self_cols;

                            /** get value to use below: check to see if the slice worked */
                            // TODO : check largest_sequence_length -- yep good thing I checked this
                            int failed_to_get_slice_values = PySlice_GetIndicesEx((PyObject *) arguement_two, largest_sequence_length, &start_inside, &stop_inside, &step_size_inside, &length_inside);

                            /** the slice function failed */
                            if (failed_to_get_slice_values) {
                                // the PySlice_GetIndicesEx set the error message so just need to return null
                                // this will also check to make sure they are integers or none
                                // done
                                return -1;
                            }

                            /** the slice function worked, valid slice (case 8) */
                            else {
                                case_number = 8;
                            }
                        }

                        /** arguement_two is not valid */
                        else {
                            // done
                            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
                            return -1;
                        }
                    }
                }

                /** if arguement_one is neither an integer or a slice */
                else {
                    // now we do not have to check arguement_two because arguement_one is not valid
                    // done
                    PyErr_SetString(PyExc_TypeError, "Invalid arguments");
                    return -1;
                }
            }
        }

        /** the key is not valid for a 2d matrix */
        else {
            // here the key is not valid so we do not have to check what it is actually we know that it is wrong
            // done
            PyErr_SetString(PyExc_TypeError, "Key is not valid");
            return -1;
        }
    }

    /** enter here if we have neither a 1d or 2d matrix */
    else {
        // this should never happen as we should not have neither a 1d or 2d matrix
        PyErr_SetString(PyExc_NotImplementedError, "(7) need to make correct");
        return -1;
    }



    // Again, the key could either be an integer, a single slice, or a tuple of two slices/ints.

    /** new case definitions */
    // (new case 1) If the resulting slice after indexing is 1 by 1, then v should be an integer or a float.
    // (new case 2) If it is an 1D matrix (1 by n or n by 1), then v should be a list of numbers, and each number can either be int or float, and the length of the list should be the same as the number of elements of this slice.
    // (new case 3) If it is 2D, then v should be a 2D list where the ith element of this list is a 1D list of integers/floats to set the ith row of the slice.

    // create a function that returns 0, 1, 2, 3 that tells me which case v falls into and if it falls into none then that will be 0

    // new case 1:

    /** case definitions */
    // case 1: 1d matrix with a key that is an integer
    // case 2: 1d matrix with a key that is a single slice
    // case 3: 2d matrix with a key that is an integer
    // case 4: 2d matrix with a key that is a single slice
    // case 5: 2d matrix with a key that is a tuple with contents: (a1: integer, a2: integer)
    // case 6: 2d matrix with a key that is a tuple with contents: (a1: integer, a2: slice)
    // case 7: 2d matrix with a key that is a tuple with contents: (a1: slice, a2: integer)
    // case 8: 2d matrix with a key that is a tuple with contents: (a1: slice, a2: slice)

    // TODO : something that need to remember to check is: when to return a number if happens to be 1 by 1 matrix -- did not do this on some!


    switch (case_number) {

            /** case 1: 1d matrix with a key that is an integer */
            // TODO : done -- done
        case 1: {
            // uncomment line below to print
            // printf("Choice is 1\n");

            // get the index_key as an integer
            long index_key = PyLong_AsLong(key);

            /** if the rows of the 1d matrix is 1 */
            if (self_rows == 1) {
                // now we know that the self_rows is 1 and the self_cols is a number greater then or equal to 1

                /** if the index_key is not in range of the 1d matrix */
                if (index_key >= self_cols || index_key < 0) {
                    // done
                    PyErr_SetString(PyExc_IndexError, "Index out of range");
                    return -1;
                }

                /** the index_key is in range of the 1d matrix */
                else {
                    return set_subscript_helper(self, v, 1, 1, 0, index_key);
                    //return PyFloat_FromDouble(get(given_matrix, 0, index_key));
                }
            }

            /** if the cols of the 1d matrix is 1 */
            else if (self_cols == 1) {
                // now we know that the self_cols is 1 and the self_rows is a number greater then or equal to 1

                /** if the index_key is not in range of the 1d matrix */
                if (index_key >= self_rows || index_key < 0) {
                    // done
                    PyErr_SetString(PyExc_IndexError, "Index out of range");
                    return -1;
                }

                /** the index_key is in range of the 1d matrix */
                else {
                    return set_subscript_helper(self, v, 1, 1, index_key, 0);
                    // return PyFloat_FromDouble(get(given_matrix, index_key, 0));
                }
            }

            /** this was not a 1d matrix and was wrongly identified */
            else {
                PyErr_SetString(PyExc_NotImplementedError, "(b.2) need to make correct");
                return -1;
            }

            break;
        }
            /** case 2: 1d matrix with a key that is a single slice */
            // TODO : done -- done
        case 2: {
            // uncomment line below to print
            // printf("Choice is 2\n");

            /** here the slice information is invalid since the start is greater then or equal to stop */
            // stop cannot be treated as stop by definition rather just the max(self_rows, self_cols)
            if (start >= stop || step_size != 1) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return -1;
            }

            /** if the rows of the 1d matrix is 1 */
            if (self_rows == 1) {

                /** here the slice length is 1 */
                if ((stop - start == 1) || (self_cols - start == 1)) {
                    // return the item at that spot
                    return set_subscript_helper(self, v, 1, 1, 0, start);
                    // return PyFloat_FromDouble(get(given_matrix, 0, start));
                }

                /** here the slice is not length 1 */
                else {
//                    matrix *matrix_to_return = NULL;
                    return set_subscript_helper(self, v, 1, stop - start, 0, start);

//                    int success = allocate_matrix_ref(&matrix_to_return, given_matrix, 0, start, 1, stop - start);
//
//                    /** need to check that allocate memory worked */
//                    if (success != 0) {
//                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful memory allocation.");
//                        return NULL;
//                    }
//
//                    Matrix61c *python_matrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
//
//                    /** need to check that the new matrix call worked */
//                    if (python_matrix == NULL) {
//                        deallocate_matrix(matrix_to_return);
//                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful matrix61c function call.");
//                        return NULL;
//                    }
//
//                    python_matrix->mat = matrix_to_return;
//                    python_matrix->shape = PyTuple_Pack(1, PyLong_FromLong(stop - start));
//                    return (PyObject *) python_matrix;
                }
            }

            /** if the cols of the 1d matrix is 1 */
            else if (self_cols == 1) {

                /** here the slice length is 1 */
                if ((stop - start == 1) || (self_rows - start == 1)) {
                    // return the item at that spot
                    return set_subscript_helper(self, v, 1, 1, start, 0);
                    // return PyFloat_FromDouble(get(given_matrix, start, 0));
                }

                /** here the slice is not length 1 */
                else {
//                    matrix *matrix_to_return = NULL;
                    return set_subscript_helper(self, v, stop - start, 1, start, 0);
//                    int success = allocate_matrix_ref(&matrix_to_return, given_matrix, start, 0, stop - start, 1);
//
//                    /** need to check that allocate memory worked */
//                    if (success != 0) {
//                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful memory allocation.");
//                        return NULL;
//                    }
//
//                    Matrix61c *python_matrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
//
//                    /** need to check that the new matrix call worked */
//                    if (python_matrix == NULL) {
//                        deallocate_matrix(matrix_to_return);
//                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful matrix61c function call.");
//                        return NULL;
//                    }
//
//                    python_matrix->mat = matrix_to_return;
//                    python_matrix->shape = PyTuple_Pack(1, PyLong_FromLong(stop - start));
//                    return (PyObject *) python_matrix;
                }

            }

                /** this was not a 1d matrix and was wrongly identified */
            else {
                PyErr_SetString(PyExc_NotImplementedError, "(r.2) need to make correct");
                return -1;
            }

            break;
        }
            /** case 3: 2d matrix with a key that is an integer */
            // TODO : done -- done
        case 3: {
            // uncomment line below to print
            //printf("Choice is 3\n");

            /** get the index value */
            long case_3_index_key = PyLong_AsLong(key);

            /** checking to see if the index value is in range of the matrix */
            // must be only looking at the rows
            if ((case_3_index_key < 0) || (case_3_index_key >= self_rows)) {
                PyErr_SetString(PyExc_IndexError, "Index out of range");
                return -1;
            }

            /** here the given index is correct */
            else {
//                matrix *matrix_to_return = NULL;
                return set_subscript_helper(self, v, 1, self_cols, case_3_index_key, 0);
//                int success = allocate_matrix_ref(&matrix_to_return, given_matrix, case_3_index_key, 0, 1, self_cols);
//
//                /** need to check that allocate memory worked */
//                if (success != 0) {
//                    PyErr_SetString(PyExc_RuntimeError, "Unsuccessful memory allocation.");
//                    return NULL;
//                }
//
//                Matrix61c *python_matrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
//
//                /** need to check that the new matrix call worked */
//                if (python_matrix == NULL) {
//                    deallocate_matrix(matrix_to_return);
//                    PyErr_SetString(PyExc_RuntimeError, "Unsuccessful matrix61c function call.");
//                    return NULL;
//                }
//
//                python_matrix->mat = matrix_to_return;
//                python_matrix->shape = PyTuple_Pack(1, PyLong_FromLong(self_cols));
//                return (PyObject *) python_matrix;
            }

            break;
        }
            /** case 4: 2d matrix with a key that is a single slice */
            // TODO : done -- done
        case 4: {
            // uncomment line below to print
            //printf("Choice is 4\n");

            /** need to check if the slice is valid ... here the slice has the same start stop */
            if (start == stop) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return -1;
            }

            /** enter here if the left index is greater then the rows or the start is negative techinacally do not need to worry
              * because the python function will make it positive */
            else if (start >= self_rows || start < 0) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return -1;
            }

            /** enter here if the left index is greater then the right index */
            else if (start >= stop) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return -1;
            }

            /** enter here if the step size is not 1 */
            else if (step_size != 1) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return -1;
            }

            /** enter here if nothing was wrong with the numbers */
            else {
//                matrix *matrix_to_return = NULL;
                Py_ssize_t new_rows = stop - start;

                return set_subscript_helper(self, v, new_rows, self_cols, start, 0);

//                int success = allocate_matrix_ref(&matrix_to_return, given_matrix, start, 0, new_rows, self_cols);
//
//                /** need to check that allocate memory worked */
//                if (success != 0) {
//                    PyErr_SetString(PyExc_RuntimeError, "Unsuccessful memory allocation.");
//                    return NULL;
//                }
//
//                Matrix61c *python_matrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
//
//                /** need to check that the new matrix call worked */
//                if (python_matrix == NULL) {
//                    deallocate_matrix(matrix_to_return);
//                    PyErr_SetString(PyExc_RuntimeError, "Unsuccessful matrix61c function call.");
//                    return NULL;
//                }
//
//                python_matrix->mat = matrix_to_return;
//
//                /** check whether this is a 1d matrix */
//                /** if enters here then is a 1d matrix */
//                if (new_rows == 1) {
//                    python_matrix->shape = PyTuple_Pack(1, PyLong_FromLong(self_cols));
//                }
//
//                    /** if enters here then is a 1d matrix */
//                else if (self_cols == 1) {
//                    python_matrix->shape = PyTuple_Pack(1, PyLong_FromLong(new_rows));
//                }
//
//                    /** if enters here then is not a 1d matrix */
//                else {
//                    python_matrix->shape = PyTuple_Pack(2, PyLong_FromLong(new_rows), PyLong_FromLong(self_cols));
//                }
//
//                return (PyObject *) python_matrix;
            }

            break;
        }
            /** case 5: 2d matrix with a key that is a tuple with contents: (a1: integer, a2: integer) */
            // TODO : done -- done
        case 5: {
            // uncomment line below to print
            // printf("Choice is 5\n");

            /** extract the values from the two indexes */
            long a1 = PyLong_AsLong(arguement_one);
            long a2 = PyLong_AsLong(arguement_two);

            /** check to see if the first input is less then 0 or greater/equal then to the number of rows */
            if (a1 < 0 || a1 >= self_rows) {
                PyErr_SetString(PyExc_IndexError, "Index out of range!");
                return -1;
            }

            /** check to see if the second input is less then 0 or greater/equal then to the number of cols */
            else if (a2 < 0 || a2 >= self_cols) {
                PyErr_SetString(PyExc_IndexError, "Index out of range!");
                return -1;
            }

            /** here the given value is correct ... this will always just result in a number because it starts as a 2d matrix */
            else {
                return set_subscript_helper(self, v, 1, 1, a1, a2);
                // return PyFloat_FromDouble(get(given_matrix, a1, a2));
            }

            break;
        }
            /** case 6: 2d matrix with a key that is a tuple with contents: (a1: integer, a2: slice) */
            // TODO : done -- done
        case 6: {
            // uncomment line below to print
            // printf("Choice is 6\n");

            /** extract the values from the tuple */
            long int_arg = PyLong_AsLong(arguement_one);


            /** check to see if the first input is less then 0 or greater/equal then to the number of rows */
            if (int_arg < 0 || int_arg >= self_rows) {
                PyErr_SetString(PyExc_IndexError, "Index out of range!");
                return -1;
            }

            /** need to check if the slice is valid ... here the slice has the same start stop */
            if (start == stop) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return -1;
            }

            /** enter here if the left index is greater then the cols or the start is negative technically do not need to worry
              * because the python function will make it positive */
            else if (start >= self_cols || start < 0) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return -1;
            }

            /** enter here if the left index is greater then the right index */
            else if (start >= stop) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return -1;
            }

            /** enter here if the step size is not 1 */
            else if (step_size != 1) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return -1;
            }

            /** enter here if nothing was wrong with the numbers */
            else {
                Py_ssize_t new_cols = stop - start;

                /** need to check to see if I have a 1 by 1 matrix */
                if (new_cols == 1) {
                    return set_subscript_helper(self, v, 1, 1, int_arg, start);
                    // return PyFloat_FromDouble(get(given_matrix, int_arg, start));
                }

                /** I do not have a 1 by 1 matrix */
                else {
//                    matrix *matrix_to_return = NULL;

                    return set_subscript_helper(self, v, 1, new_cols, int_arg, start);
//                    /** will always have 1 row because the first index is just a number */
//                    int success = allocate_matrix_ref(&matrix_to_return, given_matrix, int_arg, start, 1, new_cols);
//
//                    /** need to check that allocate memory worked */
//                    if (success != 0) {
//                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful memory allocation.");
//                        return NULL;
//                    }
//
//                    Matrix61c *python_matrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
//
//                    /** need to check that the new matrix call worked */
//                    if (python_matrix == NULL) {
//                        deallocate_matrix(matrix_to_return);
//                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful matrix61c function call.");
//                        return NULL;
//                    }
//
//                    python_matrix->mat = matrix_to_return;
//                    python_matrix->shape = PyTuple_Pack(1, PyLong_FromLong(new_cols));
//                    return (PyObject *) python_matrix;
                }
            }

            break;
        }
            /** case 7: 2d matrix with a key that is a tuple with contents: (a1: slice, a2: integer) */
            // TODO : done -- done
        case 7: {
            // uncomment line below to print
            // printf("Choice is 7\n");

            /** extract the values from the tuple */
            long int_arg_two = PyLong_AsLong(arguement_two);

            /** check to see if the second input is less then 0 or greater/equal then to the number of cols */
            if (int_arg_two < 0 || int_arg_two >= self_cols) {
                PyErr_SetString(PyExc_IndexError, "Index out of range!");
                return -1;
            }

            /** need to check if the slice is valid ... here the slice has the same start stop */
            if (start_outside == stop_outside) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return -1;
            }

            /** enter here if the left index is greater then the rows or the start is negative technically do not need
              * to worry because the python function will make it positive */
            else if (start_outside >= self_rows || start_outside < 0) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return -1;
            }

            /** enter here if the left index is greater then the right index */
            else if (start_outside >= stop_outside) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return -1;
            }

            /** enter here if the step size is not 1 */
            else if (step_size_outsize != 1) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return -1;
            }

            /** enter here if nothing was wrong with the numbers */
            else {
                Py_ssize_t new_rows = stop_outside - start_outside;

                /** need to check to see if I have a 1 by 1 matrix */
                if (new_rows == 1) {
                    return set_subscript_helper(self, v, 1, 1, start_outside, int_arg_two);
                    // return PyFloat_FromDouble(get(given_matrix, start_outside, int_arg_two));
                }

                /** I do not have a 1 by 1 matrix */
                else {
//                    matrix *matrix_to_return = NULL;

                    return set_subscript_helper(self, v, new_rows, 1, start_outside, int_arg_two);
//                    /** will always have 1 col because the second index is just a number */
//                    int success = allocate_matrix_ref(&matrix_to_return, given_matrix, start_outside, int_arg_two, new_rows, 1);
//
//                    /** need to check that allocate memory worked */
//                    if (success != 0) {
//                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful memory allocation.");
//                        return NULL;
//                    }
//
//                    Matrix61c *python_matrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
//
//                    /** need to check that the new matrix call worked */
//                    if (python_matrix == NULL) {
//                        deallocate_matrix(matrix_to_return);
//                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful matrix61c function call.");
//                        return NULL;
//                    }
//
//                    python_matrix->mat = matrix_to_return;
//                    python_matrix->shape = PyTuple_Pack(1, PyLong_FromLong(new_rows));
//                    return (PyObject *) python_matrix;
                }
            }

            break;
        }
            /** case 8: 2d matrix with a key that is a tuple with contents: (a1: slice, a2: slice) */
            // TODO : done -- done
        case 8: {
            // uncomment line below to print
            // printf("Choice is 8\n");

            /** need to check if the slice is valid ... here the slice has the same start stop */
            if (start_outside == stop_outside || start_inside == stop_inside) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return -1;
            }

            /** enter here if the left index is greater then the rows or the start is negative technically do not need
              * to worry because the python function will make it positive */
            else if ((start_outside >= self_rows || start_outside < 0) || (start_inside >= self_cols || start_inside < 0)) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return -1;
            }

            /** enter here if the left index is greater then the right index */
            else if ((start_outside >= stop_outside) || (start_inside >= stop_inside)) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return -1;
            }

            /** enter here if the step size is not 1 */
            else if ((step_size_outsize != 1) || step_size_inside != 1) {
                PyErr_SetString(PyExc_ValueError, "Slice info not valid!");
                return -1;
            }

            /** enter here if nothing was wrong with the numbers */
            else {
                Py_ssize_t new_rows = stop_outside - start_outside;
                Py_ssize_t new_cols = stop_inside - start_inside;

                /** need to check to see if I have a 1 by 1 matrix */
                if ((new_rows == 1) && (new_cols == 1)) {
                    return set_subscript_helper(self, v, 1, 1, start_outside, start_inside);
                    // return PyFloat_FromDouble(get(given_matrix, start_outside, start_inside));
                }

                /** I do not have a 1 by 1 matrix */
                else {
//                    matrix *matrix_to_return = NULL;

                    return set_subscript_helper(self, v, new_rows, new_cols, start_outside, start_inside);

//                    /** will always have not always have 1 col or 1 row because the second index is also a slice */
//                    int success = allocate_matrix_ref(&matrix_to_return, given_matrix, start_outside, start_inside, new_rows, new_cols);
//
//                    /** need to check that allocate memory worked */
//                    if (success != 0) {
//                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful memory allocation.");
//                        return NULL;
//                    }
//
//                    Matrix61c *python_matrix = (Matrix61c *) Matrix61c_new(&Matrix61cType, NULL, NULL);
//
//                    /** need to check that the new matrix call worked */
//                    if (python_matrix == NULL) {
//                        deallocate_matrix(matrix_to_return);
//                        PyErr_SetString(PyExc_RuntimeError, "Unsuccessful matrix61c function call.");
//                        return NULL;
//                    }
//
//                    python_matrix->mat = matrix_to_return;
//
//                    /** check if rows is 1 */
//                    if (new_rows == 1) {
//                        python_matrix->shape = PyTuple_Pack(1, PyLong_FromLong(new_cols));
//                    }
//
//                    /** check if cols is 1 */
//                    else if (new_cols == 1) {
//                        python_matrix->shape = PyTuple_Pack(1, PyLong_FromLong(new_rows));
//                    }
//
//                    /** neither rows or cols is one */
//                    else {
//                        python_matrix->shape = PyTuple_Pack(2, PyLong_FromLong(new_rows), PyLong_FromLong(new_cols));
//                    }
//
//                    return (PyObject *) python_matrix;
                }
            }

            break;
        }
            /** default case: none of the other cases hit */
            // TODO : done -- added NotImplementedError -- done
        default: {
            // uncomment line below to print
            // printf("Choice is default\n");

            PyErr_SetString(PyExc_NotImplementedError, "Matrix not supported, switch statement error.");
            return -1;
            break;
        }
    }

    // return Py_BuildValue("s", "ryan Matrix61c_subscript not finished");

    // -----------------------



    PyErr_SetString(PyExc_NotImplementedError, "ryan Matrix61c_set_subscript not finished");
    return -1;
}

PyMappingMethods Matrix61c_mapping = {
        NULL,
        (binaryfunc) Matrix61c_subscript,
        (objobjargproc) Matrix61c_set_subscript,
};

/* INSTANCE ATTRIBUTES*/
PyMemberDef Matrix61c_members[] = {
        {
                "shape", T_OBJECT_EX, offsetof(Matrix61c, shape), 0,
                "(rows, cols)"
        },
        {NULL}  /* Sentinel */
};

PyTypeObject Matrix61cType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "numc.Matrix",
        .tp_basicsize = sizeof(Matrix61c),
        .tp_dealloc = (destructor) Matrix61c_dealloc,
        .tp_repr = (reprfunc) Matrix61c_repr,
        .tp_as_number = &Matrix61c_as_number,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_doc = "numc.Matrix objects",
        .tp_methods = Matrix61c_methods,
        .tp_members = Matrix61c_members,
        .tp_as_mapping = &Matrix61c_mapping,
        .tp_init = (initproc) Matrix61c_init,
        .tp_new = Matrix61c_new
};


struct PyModuleDef numcmodule = {
        PyModuleDef_HEAD_INIT,
        "numc",
        "Numc matrix operations",
        -1,
        Matrix61c_class_methods
};

/* Initialize the numc module */
PyMODINIT_FUNC PyInit_numc(void) {
    PyObject *m;

    if (PyType_Ready(&Matrix61cType) < 0)
        return NULL;

    m = PyModule_Create(&numcmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&Matrix61cType);
    PyModule_AddObject(m, "Matrix", (PyObject *) &Matrix61cType);
    printf("CS61C Fall 2020 Project 4: numc imported!\n");
    fflush(stdout);
    return m;
}