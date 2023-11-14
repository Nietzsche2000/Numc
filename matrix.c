#include "matrix.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))

#include <immintrin.h>
#include <x86intrin.h>

#endif

/* Below are some intel intrinsics that might be useful
 * void _mm256_storeu_pd (double * mem_addr, __m256d a)
 * __m256d _mm256_set1_pd (double a)
 * __m256d _mm256_set_pd (double e3, double e2, double e1, double e0)
 * __m256d _mm256_loadu_pd (double const * mem_addr)
 * __m256d _mm256_add_pd (__m256d a, __m256d b)
 * __m256d _mm256_sub_pd (__m256d a, __m256d b)
 * __m256d _mm256_fmadd_pd (__m256d a, __m256d b, __m256d c)
 * __m256d _mm256_mul_pd (__m256d a, __m256d b)
 * __m256d _mm256_cmp_pd (__m256d a, __m256d b, const int imm8)
 * __m256d _mm256_and_pd (__m256d a, __m256d b)
 * __m256d _mm256_max_pd (__m256d a, __m256d b)
*/

/*
 * Generates a random double between `low` and `high`.
 */
double rand_double(double low, double high) {
    double range = (high - low);
    double div = RAND_MAX / range;
    return low + (rand() / div);
}

/*
 * Generates a random matrix with `seed`.
 */
void rand_matrix(matrix *result, unsigned int seed, double low, double high) {
    srand(seed);
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            set(result, i, j, rand_double(low, high));
        }
    }
}


/** REQUIRED MACROS */

#define BLOCKSIZE 32 // 16 // 64
// SOURCE: COMPETITIVE BIT TRICKS
#define TOBE(x, b) (((x)+((b)-1)) & -(b))
#define MIN(x, y) ((y) ^(((x) ^ (y)) & -((x) < (y))))
#define ALIGN 64

/** INFINITIES */
static inline int isInfinity(__m256d x) {
    const __m256d mask = _mm256_set1_pd(-0.0);
    const __m256d inf = _mm256_set1_pd(1.0 / 0.0);

    x = _mm256_andnot_pd(mask, x);
    x = _mm256_cmp_pd(x, inf, _CMP_EQ_OQ);
    return _mm256_movemask_pd(x) != 0;
}


/*
 * Allocate space for a matrix struct pointed to by the double pointer mat with
 * `rows` rows and `cols` columns. You should also allocate memory for the data array
 * and initialize all entries to be zeros. Remember to set all fieds of the matrix struct.
 * `parent` should be set to NULL to indicate that this matrix is not a slice.
 * You should return -1 if either `rows` or `cols` or both have invalid values, or if any
 * call to allocate memory in this function fails. If you don't set python error messages here upon
 * failure, then remember to set it in numc.c.
 * Return 0 upon success and non-zero upon failure.
 */
int allocate_matrix(matrix **mat, int rows, int cols) {
    /* TODO: YOUR CODE HERE */

    /** CHECK VALID ROWS AND COLUMNS */
    if (rows <= 0 || cols <= 0) {
        // TODO: ERROR
        return -1;
    }

    int trueRows = TOBE(rows, BLOCKSIZE);
    int trueCols = TOBE(cols, BLOCKSIZE);

    /** MAKE SOME SPACE FOR DATA */
    double *m_data = (double *) _mm_malloc(trueRows * trueCols * sizeof(double), ALIGN); // have two of them.


    /** CHECK IF SUCCESS */
    if (m_data == NULL) {
        // TODO: ERROR
        return -1;
    }

    /** ZERO VECTOR */
    __m256d ZERO = _mm256_setzero_pd();
#pragma omp parallel for
    for (int i = 0; i < trueRows * trueCols; i += 8) {
        _mm256_storeu_pd(m_data + i, ZERO);
        _mm256_storeu_pd(m_data + i + 4, ZERO);
    }


    /** MAKE SOME SPACE FOR MAT */
    *mat = (matrix *) _mm_malloc(sizeof(matrix), ALIGN);

    /** CHECK IF SUCCESS */
    if (*mat == NULL) {
        // TODO: ERROR

        /** FREE OUR PREVIOUS MALLOCS */
        _mm_free(m_data);
        return -1;
    }

    /** SET THE FIELDS */
    (*mat)->rows = rows;
    (*mat)->cols = cols;
    (*mat)->trueRows = trueRows;
    (*mat)->trueCols = trueCols;
    (*mat)->data = m_data;
    (*mat)->is_1d = rows == 1 || cols == 1;
    (*mat)->ref_cnt = 1;
    (*mat)->parent = NULL;
    (*mat)->is_slice = 0;

    /** SUCCESS */
    return 0;


}

/*
 * Allocate space for a matrix struct pointed to by `mat` with `rows` rows and `cols` columns.
 * This is equivalent to setting the new matrix to be
 * from[row_offset:row_offset + rows, col_offset:col_offset + cols]
 * If you don't set python error messages here upon failure, then remember to set it in numc.c.
 * Return 0 upon success and non-zero upon failure.
 */
int allocate_matrix_ref(matrix **mat, matrix *from, int row_offset, int col_offset,
                        int rows, int cols) {
    /* TODO: YOUR CODE HERE */

    /** CHECK VALID ROWS AND COLUMNS */
    if (rows <= 0 || cols <= 0) {
        // TODO: ERROR
        return -1;
    }

    /** MAKE SOME SPACE FOR MAT */
    *mat = (matrix *) _mm_malloc(sizeof(matrix), ALIGN);

    /** CHECK IF SUCCESS */
    if (*mat == NULL) {
        // TODO: ERROR
        return -1;
    }

    /** SET THE FIELDS */
    (*mat)->rows = rows;
    (*mat)->cols = cols;
    (*mat)->trueRows = from->trueRows;
    (*mat)->trueCols = from->trueCols;
    (*mat)->data = from->data + (row_offset * from->trueCols) + col_offset;
    (*mat)->is_1d = rows == 1 || cols == 1;
    (*mat)->ref_cnt = 1;
    (*mat)->parent = from;
    (*mat)->is_slice = 1;

    /** INCREMENT FROM REF_CNT FOR PARENT */
    from->ref_cnt++;

    /** SUCCESS */
    return 0;
}

/*
 * This function will be called automatically by Python when a numc matrix loses all of its
 * reference pointers.
 * You need to make sure that you only free `mat->data` if no other existing matrices are also
 * referring this data array.
 * See the spec for more information.
 */
void deallocate_matrix(matrix *mat) {
    /* TODO: YOUR CODE HERE */

    /** NULL CHECK */
    if (mat == NULL)
        return;

    /** SOLITARY MATRIX */
    if (mat->ref_cnt == 1 && mat->parent == NULL) {
        _mm_free(mat->data);
        _mm_free(mat);
        return;
    }

    /** IF I AM IN THE END */
    if (mat->ref_cnt == 1 && mat->parent != NULL) {
        if (mat->parent->ref_cnt == 1) {
            deallocate_matrix(mat->parent);
            _mm_free(mat);
        } else {
            mat->parent->ref_cnt--;
            _mm_free(mat);
        }
        return;
    }

    if (mat->ref_cnt != 1 && mat->parent == NULL) {
        mat->ref_cnt--;
        return;
    }

    /** I CALL DEALLOC ON A SANDWICH MATRIX */
    if (mat->ref_cnt != 1 && mat->parent != NULL) {
        mat->ref_cnt--;
        return;
    }

}

static inline double get_2(matrix *mat, int row, int col) {
    /* TODO: YOUR CODE HERE */
    return mat->data[row * mat->trueCols + col];
}

/*
 * Return the double value of the matrix at the given row and column.
 * You may assume `row` and `col` are valid.
 */
double get(matrix *mat, int row, int col) {
    /* TODO: YOUR CODE HERE */
    return mat->data[row * mat->trueCols + col];
}

/*
 * Set the value at the given row and column to val. You may assume `row` and
 * `col` are valid
 */
void set(matrix *mat, int row, int col, double val) {
    /* TODO: YOUR CODE HERE */
    mat->data[row * mat->trueCols + col] = val;
}

static inline void set_2(matrix *mat, int row, int col, double val) {
    /* TODO: YOUR CODE HERE */
    mat->data[row * mat->trueCols + col] = val;
}


/*
 * Set all entries in mat to val
 */
__attribute__((optimize("unroll-loops")))
void fill_matrix(matrix *mat, double val) {
    /* TODO: YOUR CODE HERE */
#pragma omp parallel for
    for (int i = 0; i < mat->rows; ++i) {
        for (int j = 0; j < mat->cols; ++j) {
            set_2(mat, i, j, val);
        }
    }
}


/** ZERO OUT A MATRIX USING SIMD */
static inline void matrix_zero(matrix *mat) {
    /* TODO: YOUR CODE HERE */
    const int rows = mat->rows;
    const int cols = mat->cols;

//    const int mat_true_rows = mat->trueRows;
    const int mat_true_cols = mat->trueCols;

    const int unroll = 8;
    const int factor = 4;

    double *mat_data = mat->data;

    __m256d V[unroll];
    /** LOAD ZERO VECTORS */
    for (int x = 0; x < unroll; ++x) {
        V[x] = _mm256_setzero_pd();
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; j += factor * unroll) {
            /** UNROLL AND STORE */
            for (int x = 0; x < unroll; ++x) {
                _mm256_storeu_pd(mat_data + i * mat_true_cols + j + x * factor, V[x]);
            }
        }
    }
}

int add_matrix_slice(matrix *__restrict__ result, matrix *__restrict__ mat1, matrix *__restrict__ mat2) {

    /** MAT 1 ROWS AND COLS */
    const int mat1_rows = mat1->rows;
    const int mat1_cols = mat1->cols;
    for (int i = 0; i < mat1_rows; ++i) {
        for (int j = 0; j < mat1_cols; ++j) {
            double sum = get(mat1, i, j) + get(mat2, i, j);
            set(result, i, j, sum);
        }
    }
    return 0;
}

//
///*
// * Store the result of adding mat1 and mat2 to `result`.
// * Return 0 upon success and a nonzero value upon failure.
// */
//int add_matrix2(matrix *result, matrix *mat1, matrix *mat2) {
//    /* TODO: YOUR CODE HERE */
//
//
//    /** IF ONE OF THEM IS A SLICE */
//    if (result->is_slice || mat1->is_slice || mat2->is_slice) {
//        return add_matrix_slice(result, mat1, mat2);
//    }
//
//    /** MAT 1 ROWS AND COLS */
//    int mat1_rows = mat1->rows;
//    int mat1_cols = mat1->cols;
//    /** MAT 2 ROWS AND COLS */
//    int mat2_rows = mat2->rows;
//    int mat2_cols = mat2->cols;
//    /** MAT 1 TRUE ROWS AND COLS */
//    int mat1_true_rows = mat1->trueRows;
//    int mat1_true_cols = mat1->trueCols;
//    /** MAT 2 TRUE ROWS AND COLS */
//    int mat2_true_rows = mat2->trueRows;
//    int mat2_true_cols = mat2->trueCols;
//
//    /** RESULT 2 TRUE ROWS AND COLS */
//    int result_true_rows = result->trueRows;
//    int result_true_cols = result->trueCols;
//
//    /** DATA */
//    double *result_data = result->data;
//    double *mat1_data = mat1->data;
//    double *mat2_data = mat2->data;
//
//    const int unroll = 4;
//    const int factor = 4 * unroll;
//
//
//#pragma omp parallel for
//    for (int i = 0; i < mat1_rows; i += 1) {
//        for (int j = 0; j < mat1_cols; j += factor) {
//            __m256d V[unroll];
//            __m256d M[unroll];
//            for (int x = 0; x < unroll; ++x) {
//                V[x] = _mm256_loadu_pd(mat1_data + x * 4 + i * mat1_true_cols + j);
//                M[x] = _mm256_loadu_pd(mat2_data + x * 4 + i * mat2_true_cols + j);
//            }
//            for (int x = 0; x < unroll; ++x) {
//                __m256d s = _mm256_add_pd(V[x], M[x]);
//                _mm256_storeu_pd(result_data + x * 4 + i * result_true_cols + j, s);
//            }
//        }
//    }
//    return 0;
//}


/*
 * Store the result of adding mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int add_matrix(matrix *__restrict__ result, matrix *__restrict__ mat1, matrix *__restrict__ mat2) {
    /* TODO: YOUR CODE HERE */

    /** IF ONE OF THEM IS A SLICE */
    if (result->is_slice || mat1->is_slice || mat2->is_slice) {
        return add_matrix_slice(result, mat1, mat2);
    }

    /** MAT 1 ROWS AND COLS */
    int mat1_rows = mat1->rows;
    int mat1_cols = mat1->cols;
    /** MAT 2 ROWS AND COLS */
//    int mat2_rows = mat2->rows;
//    int mat2_cols = mat2->cols;
    /** MAT 1 TRUE ROWS AND COLS */
//    int mat1_true_rows = mat1->trueRows;
    int mat1_true_cols = mat1->trueCols;
    /** MAT 2 TRUE ROWS AND COLS */
//    int mat2_true_rows = mat2->trueRows;
    int mat2_true_cols = mat2->trueCols;

    /** RESULT 2 TRUE ROWS AND COLS */
//    int result_true_rows = result->trueRows;
    int result_true_cols = result->trueCols;

    /** DATA */
    double *__restrict__ result_data = result->data;
    double *__restrict__ mat1_data = mat1->data;
    double *__restrict__ mat2_data = mat2->data;

    const int unroll = 8;
    const int factor = 4 * unroll;

    /** DECLARE THE VECTORS */
    __m256d V1;
    __m256d V2;
    __m256d V3;
    __m256d V4;
    __m256d V5;
    __m256d V6;
    __m256d V7;
    __m256d V8;
#pragma omp parallel for private(V1, V2, V3, V4, V5, V6, V7, V8)
    for (int i = 0; i < mat1_rows; ++i) {
        for (int j = 0; j < mat1_cols; j += factor) {
            V1 = _mm256_add_pd(_mm256_loadu_pd(mat1_data + j + i * mat1_true_cols),
                               _mm256_loadu_pd(mat2_data + j + i * mat2_true_cols));
            V2 = _mm256_add_pd(_mm256_loadu_pd(mat1_data + j + i * mat1_true_cols + 4),
                               _mm256_loadu_pd(mat2_data + j + i * mat2_true_cols + 4));
            V3 = _mm256_add_pd(_mm256_loadu_pd(mat1_data + j + i * mat1_true_cols + 8),
                               _mm256_loadu_pd(mat2_data + j + i * mat2_true_cols + 8));
            V4 = _mm256_add_pd(_mm256_loadu_pd(mat1_data + j + i * mat1_true_cols + 12),
                               _mm256_loadu_pd(mat2_data + j + i * mat2_true_cols + 12));
            V5 = _mm256_add_pd(_mm256_loadu_pd(mat1_data + j + i * mat1_true_cols + 16),
                               _mm256_loadu_pd(mat2_data + j + i * mat2_true_cols + 16));
            V6 = _mm256_add_pd(_mm256_loadu_pd(mat1_data + j + i * mat1_true_cols + 20),
                               _mm256_loadu_pd(mat2_data + j + i * mat2_true_cols + 20));
            V7 = _mm256_add_pd(_mm256_loadu_pd(mat1_data + j + i * mat1_true_cols + 24),
                               _mm256_loadu_pd(mat2_data + j + i * mat2_true_cols + 24));
            V8 = _mm256_add_pd(_mm256_loadu_pd(mat1_data + j + i * mat1_true_cols + 28),
                               _mm256_loadu_pd(mat2_data + j + i * mat2_true_cols + 28));

            _mm256_storeu_pd(result_data + j + i * result_true_cols, V1);
            _mm256_storeu_pd(result_data + j + i * result_true_cols + 4, V2);
            _mm256_storeu_pd(result_data + j + i * result_true_cols + 8, V3);
            _mm256_storeu_pd(result_data + j + i * result_true_cols + 12, V4);
            _mm256_storeu_pd(result_data + j + i * result_true_cols + 16, V5);
            _mm256_storeu_pd(result_data + j + i * result_true_cols + 20, V6);
            _mm256_storeu_pd(result_data + j + i * result_true_cols + 24, V7);
            _mm256_storeu_pd(result_data + j + i * result_true_cols + 28, V8);
        }
    }
    return 0;
}


int sub_matrix_slice(matrix *__restrict__ result, matrix *__restrict__ mat1, matrix *__restrict__ mat2) {

    /** MAT 1 ROWS AND COLS */
    const int mat1_rows = mat1->rows;
    const int mat1_cols = mat1->cols;

    for (int i = 0; i < mat1_rows; ++i) {
        for (int j = 0; j < mat1_cols; ++j) {
            double sub = get(mat1, i, j) - get(mat2, i, j);
            set(result, i, j, sub);
        }
    }
    return 0;
}

///*
// * Store the result of subtracting mat2 from mat1 to `result`.
// * Return 0 upon success and a nonzero value upon failure.
// */
//int sub_matrix(matrix *result, matrix *mat1, matrix *mat2) {
//    /* TODO: YOUR CODE HERE */
//
//    /** IF ONE OF THEM IS A SLICE */
//    if (result->is_slice || mat1->is_slice || mat2->is_slice) {
//        return sub_matrix_slice(result, mat1, mat2);
//    }
//    /** MAT 1 ROWS AND COLS */
//    int mat1_rows = mat1->rows;
//    int mat1_cols = mat1->cols;
//    /** MAT 2 ROWS AND COLS */
//    int mat2_rows = mat2->rows;
//    int mat2_cols = mat2->cols;
//    /** MAT 1 TRUE ROWS AND COLS */
//    int mat1_true_rows = mat1->trueRows;
//    int mat1_true_cols = mat1->trueCols;
//    /** MAT 2 TRUE ROWS AND COLS */
//    int mat2_true_rows = mat2->trueRows;
//    int mat2_true_cols = mat2->trueCols;
//
//    /** RESULT 2 TRUE ROWS AND COLS */
//    int result_true_rows = result->trueRows;
//    int result_true_cols = result->trueCols;
//
//    /** DATA */
//    double *result_data = result->data;
//    double *mat1_data = mat1->data;
//    double *mat2_data = mat2->data;
//
//    int unroll = 8;
//    int factor = 4 * unroll;
//    for (int i = 0; i < mat1_rows; i += 1) {
//        for (int j = 0; j < mat1_cols; j += factor) {
//            __m256d V[unroll];
//            __m256d M[unroll];
//            for (int x = 0; x < unroll; ++x) {
//                V[x] = _mm256_loadu_pd(mat1_data + x * 4 + i * mat1_true_cols + j);
//                M[x] = _mm256_loadu_pd(mat2_data + x * 4 + i * mat2_true_cols + j);
//            }
//            for (int x = 0; x < unroll; ++x) {
//                __m256d s = _mm256_sub_pd(V[x], M[x]);
//                _mm256_storeu_pd(result_data + x * 4 + i * result_true_cols + j, s);
//            }
//        }
//    }
//    return 0;
//}

/*
 * Store the result of subtracting mat2 from mat1 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int sub_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */

    /** IF ONE OF THEM IS A SLICE */
    if (result->is_slice || mat1->is_slice || mat2->is_slice) {
        return sub_matrix_slice(result, mat1, mat2);
    }
    /** MAT 1 ROWS AND COLS */
    int mat1_rows = mat1->rows;
    int mat1_cols = mat1->cols;
    /** MAT 2 ROWS AND COLS */
//    int mat2_rows = mat2->rows;
//    int mat2_cols = mat2->cols;
    /** MAT 1 TRUE ROWS AND COLS */
//    int mat1_true_rows = mat1->trueRows;
    int mat1_true_cols = mat1->trueCols;
    /** MAT 2 TRUE ROWS AND COLS */
//    int mat2_true_rows = mat2->trueRows;
    int mat2_true_cols = mat2->trueCols;

    /** RESULT 2 TRUE ROWS AND COLS */
//    int result_true_rows = result->trueRows;
    int result_true_cols = result->trueCols;

    /** DATA */
    double *result_data = result->data;
    double *mat1_data = mat1->data;
    double *mat2_data = mat2->data;

    const int unroll = 8;
    const int factor = 4 * unroll;

    /** DECLARE THE VECTORS */
    __m256d V1;
    __m256d V2;
    __m256d V3;
    __m256d V4;
    __m256d V5;
    __m256d V6;
    __m256d V7;
    __m256d V8;

#pragma omp parallel for private(V1, V2, V3, V4, V5, V6, V7, V8)
    for (int i = 0; i < mat1_rows; ++i) {
        for (int j = 0; j < mat1_cols; j += factor) {

            /** LOAD AND SUB */
            V1 = _mm256_sub_pd(_mm256_loadu_pd(mat1_data + j + i * mat1_true_cols),
                               _mm256_loadu_pd(mat2_data + j + i * mat2_true_cols));

            V2 = _mm256_sub_pd(_mm256_loadu_pd(mat1_data + j + i * mat1_true_cols + 4),
                               _mm256_loadu_pd(mat2_data + j + i * mat2_true_cols + 4));

            V3 = _mm256_sub_pd(_mm256_loadu_pd(mat1_data + j + i * mat1_true_cols + 8),
                               _mm256_loadu_pd(mat2_data + j + i * mat2_true_cols + 8));

            V4 = _mm256_sub_pd(_mm256_loadu_pd(mat1_data + j + i * mat1_true_cols + 12),
                               _mm256_loadu_pd(mat2_data + j + i * mat2_true_cols + 12));

            V5 = _mm256_sub_pd(_mm256_loadu_pd(mat1_data + j + i * mat1_true_cols + 16),
                               _mm256_loadu_pd(mat2_data + j + i * mat2_true_cols + 16));

            V6 = _mm256_sub_pd(_mm256_loadu_pd(mat1_data + j + i * mat1_true_cols + 20),
                               _mm256_loadu_pd(mat2_data + j + i * mat2_true_cols + 20));

            V7 = _mm256_sub_pd(_mm256_loadu_pd(mat1_data + j + i * mat1_true_cols + 24),
                               _mm256_loadu_pd(mat2_data + j + i * mat2_true_cols + 24));

            V8 = _mm256_sub_pd(_mm256_loadu_pd(mat1_data + j + i * mat1_true_cols + 28),
                               _mm256_loadu_pd(mat2_data + j + i * mat2_true_cols + 28));

            /** STORE */
            _mm256_storeu_pd(result_data + j + i * result_true_cols, V1);
            _mm256_storeu_pd(result_data + j + i * result_true_cols + 4, V2);
            _mm256_storeu_pd(result_data + j + i * result_true_cols + 8, V3);
            _mm256_storeu_pd(result_data + j + i * result_true_cols + 12, V4);
            _mm256_storeu_pd(result_data + j + i * result_true_cols + 16, V5);
            _mm256_storeu_pd(result_data + j + i * result_true_cols + 20, V6);
            _mm256_storeu_pd(result_data + j + i * result_true_cols + 24, V7);
            _mm256_storeu_pd(result_data + j + i * result_true_cols + 28, V8);
        }
    }

    return 0;
}

/** MUL WITH SLICE */
int mul_matrix_slice(matrix *result, matrix *mat1, matrix *mat2) {

    /** MAT 1 AND MAT 2 AND RESULT ROWS AND COLS */
    int mat1_rows = mat1->rows;
    int mat1_cols = mat1->cols;
//    int mat2_rows = mat2->rows;
    int mat2_cols = mat2->cols;

    /** MAT 1 AND MAT 2 TRUE ROWS AND COLS */
//    int mat1_true_rows = mat1->trueRows;
    int mat1_true_cols = mat1->trueCols;

//    int mat2_true_rows = mat2->trueRows;
    int mat2_true_cols = mat2->trueCols;

    /** RESULT TRUE ROWS AND COLS */
//    int result_true_rows = result->trueRows;
    int result_true_cols = result->trueCols;

    /** DATA */
    double *mat1_data = mat1->data;
    double *mat2_data = mat2->data;
    double *result_data = result->data;

    for (int i = 0; i < mat1_rows; ++i) {
        for (int j = 0; j < mat2_cols; ++j) {
            result_data[i * result_true_cols + j] = 0;
            for (int k = 0; k < mat1_cols; ++k) {
                result_data[i * result_true_cols + j] +=
                        mat1_data[i * mat1_true_cols + k] * mat2_data[k * mat2_true_cols + j];
            }
        }
    }
    return 0;
}


int mul_matrix(matrix *result, matrix *mat1, matrix *mat2) {

    /** IF IT IS A SLICE */
    if (result->is_slice || mat1->is_slice || mat2->is_slice) {
        return mul_matrix_slice(result, mat1, mat2);
    }

    /** MAT 1 AND MAT 2 AND RESULT ROWS AND COLS */
    int mat1_rows = mat1->rows;
    int mat1_cols = mat1->cols;
//    int mat2_rows = mat2->rows;
    int mat2_cols = mat2->cols;

    /** MAT 1 AND MAT 2 TRUE ROWS AND COLS */
//    int mat1_true_rows = mat1->trueRows;
    int mat1_true_cols = mat1->trueCols;

//    int mat2_true_rows = mat2->trueRows;
    int mat2_true_cols = mat2->trueCols;

    /** RESULT TRUE ROWS AND COLS */
//    int result_true_rows = result->trueRows;
    int result_true_cols = result->trueCols;

    /** DATA */
    double *mat1_data = mat1->data;
    double *mat2_data = mat2->data;
    double *result_data = result->data;

    const int unroll = 8;
    const int factor = unroll * 4;
#pragma omp parallel for
    for (int sj = 0; sj < mat1_rows; sj += BLOCKSIZE) {
        for (int si = 0; si < mat2_cols; si += BLOCKSIZE) {
            for (int sk = 0; sk < mat1_cols; sk += BLOCKSIZE) {
                /** TEMP VAR 1 */
                int up_i = MIN(si + BLOCKSIZE, mat2_cols);
                int up_j = MIN(sj + BLOCKSIZE, mat1_rows);
                int up_k = MIN(sk + BLOCKSIZE, mat1_cols);
                /** FOR SHIFTING RIGHT THE ROW VECTOR POINTER */
                for (int i = si; i < up_i; i += factor) {

                    /** FOR INCREMENTING THE VECTOR BROADCAST */
                    for (int j = sj; j < up_j; j += 1) {
                        __m256d rv[unroll];

                        /** UNROLLING TO PREVENT STALLED PIPELINE */
                        for (int x = 0; x < unroll; ++x) {
                            /** LOAD THE RESULT VECTOR FOR SUM */
                            rv[x] = _mm256_loadu_pd(result_data + i + j * result_true_cols + x * 4);
                        }

                        /** DO WORK ON THE BLOCK AND VECTOR */
                        for (int k = sk; k < up_k; ++k) {
                            /** BROADCAST THE FIRST ELEMENT TO A 4-1 VECTOR */
                            __m256d a = _mm256_broadcast_sd(mat1_data + k + j * mat1_true_cols);

                            /** DO THE SEQUENTIAL MULTIPLY AND ADD OPERATION */
                            for (int x = 0; x < unroll; x++) {
                                __m256d b = _mm256_loadu_pd(mat2_data + k * mat2_true_cols + i + x * 4);
                                rv[x] = _mm256_fmadd_pd(a, b, rv[x]);
                            }
                        }
                        /** REVERSE THE UNROLL */
                        for (int x = 0; x < unroll; ++x) {
                            _mm256_storeu_pd(result_data + i + j * result_true_cols + x * 4, rv[x]);
                        }
                    }
                }
            }
        }
    }
    return 0;
}

int mul_matrix_no_slice(matrix *result, matrix *mat1, matrix *mat2) {
    /** MAT 1 AND MAT 2 AND RESULT ROWS AND COLS */
    int mat1_rows = mat1->rows;
    int mat1_cols = mat1->cols;
//    int mat2_rows = mat2->rows;
    int mat2_cols = mat2->cols;

    /** MAT 1 AND MAT 2 TRUE ROWS AND COLS */
//    int mat1_true_rows = mat1->trueRows;
    int mat1_true_cols = mat1->trueCols;

//    int mat2_true_rows = mat2->trueRows;
    int mat2_true_cols = mat2->trueCols;

    /** RESULT TRUE ROWS AND COLS */
//    int result_true_rows = result->trueRows;
    int result_true_cols = result->trueCols;

    /** DATA */
    double *mat1_data = mat1->data;
    double *mat2_data = mat2->data;
    double *result_data = result->data;

    const int unroll = 8;
    const int factor = unroll * 4;
#pragma omp parallel for
    for (int sj = 0; sj < mat1_rows; sj += BLOCKSIZE) {
        for (int si = 0; si < mat2_cols; si += BLOCKSIZE) {
            for (int sk = 0; sk < mat1_cols; sk += BLOCKSIZE) {
                /** TEMP VAR 1 */
                int up_i = MIN(si + BLOCKSIZE, mat2_cols);
                int up_j = MIN(sj + BLOCKSIZE, mat1_rows);
                int up_k = MIN(sk + BLOCKSIZE, mat1_cols);
                /** FOR SHIFTING RIGHT THE ROW VECTOR POINTER */
                for (int i = si; i < up_i; i += factor) {

                    /** FOR INCREMENTING THE VECTOR BROADCAST */
                    for (int j = sj; j < up_j; j += 1) {
                        __m256d rv[unroll];

                        int r = 1;
                        /** UNROLLING TO PREVENT STALLED PIPELINE */
                        for (int x = 0; x < unroll; ++x) {
                            /** LOAD THE RESULT VECTOR FOR SUM */
                            rv[x] = _mm256_loadu_pd(result_data + i + j * result_true_cols + x * 4);
                            r = r & isInfinity(rv[x]);
                        }

                        if (r == 1)
                            continue;

                        /** DO WORK ON THE BLOCK AND VECTOR */
                        for (int k = sk; k < up_k; ++k) {
                            /** BROADCAST THE FIRST ELEMENT TO A 4-1 VECTOR */
                            __m256d a = _mm256_broadcast_sd(mat1_data + k + j * mat1_true_cols);

                            /** DO THE SEQUENTIAL MULTIPLY AND ADD OPERATION */
                            for (int x = 0; x < unroll; x++) {
                                __m256d b = _mm256_loadu_pd(mat2_data + k * mat2_true_cols + i + x * 4);
                                rv[x] = _mm256_fmadd_pd(a, b, rv[x]);
                            }
                        }
                        /** REVERSE THE UNROLL */
                        for (int x = 0; x < unroll; ++x) {
                            _mm256_storeu_pd(result_data + i + j * result_true_cols + x * 4, rv[x]);
                        }
                    }
                }
            }
        }
    }
    return 0;
}

/** NEED TO HAVE SAME TRUE ROWS AND SAME TRUE COLS */
static inline void cpy_exchange(matrix *one, matrix *two) {
    double *temp = one->data;
    one->data = two->data;
    two->data = temp;
}


static inline void make_identity(matrix *mat) {
    for (int i = 0; i < mat->rows; ++i) {
        for (int j = 0; j < mat->cols; ++j) {
            if (i == j)
                set_2(mat, i, j, 1.0);
        }
    }
}


static inline void cpy_easy(matrix *from, matrix *to) {

    /** ROWS AND COLS OF FROM */
    int rows = from->rows;
    int cols = from->cols;

    /** DATA */
    double *from_data = from->data;
    double *to_data = to->data;

//    int from_true_rows = from->trueRows;
    int from_true_cols = from->trueCols;

//    int to_true_rows = to->trueRows;
    int to_true_cols = to->trueCols;

    const int unroll = 8;
    const int factor = 4;

    for (int i = 0; i < rows; i++) {
        __m256d V[unroll];
        for (int j = 0; j < cols; j += factor * unroll) {
            for (int x = 0; x < unroll; ++x) {
                V[x] = _mm256_loadu_pd(from_data + j + i * from_true_cols + x * factor);
            }
            for (int x = 0; x < unroll; ++x) {
                _mm256_storeu_pd(to_data + j + i * to_true_cols + x * factor, V[x]);
            }
        }
    }

}

void cpy_slice(matrix *from, matrix *to) {
    for (int i = 0; i < from->rows; ++i) {
        for (int j = 0; j < from->cols; ++j) {
            set_2(to, i, j, get_2(from, i, j));
        }
    }
}


int pow_matrix_slice(matrix *result, matrix *mat, int pow) {
    /** MAT, RESULT ROWS AND COLS */
    const int mat_rows = mat->rows;
    const int mat_cols = mat->cols;
//    const int result_rows = result->rows;
//    const int result_cols = result->cols;

    /** MAT, RESULT TRUE ROWS AND COLS */
//    const int mat_true_rows = mat->trueRows;
//    const int mat_true_cols = mat->trueCols;
//    const int result_true_rows = result->trueRows;
//    const int result_true_cols = result->trueCols;

    /** ALLOCATE SPACE FOR A TEMPORARY MATRIX */
    matrix *temp;
    matrix *mat_temp;
    if (allocate_matrix(&temp, mat_rows, mat_cols)) {
        return -1;
    }

    if (allocate_matrix(&mat_temp, mat_rows, mat_cols)) {
        deallocate_matrix(temp);
        return -1;
    }

    /** MAKE IDENTITY */
    make_identity(result);
    int i = 1;
    cpy_slice(mat, mat_temp);

    while (pow > 0) {
        if (pow & 1) {
            if (i) {
                cpy_slice(mat_temp, result);
                i--;
            } else {
                fill_matrix(temp, 0.0);
                mul_matrix_slice(temp, result, mat_temp);
                cpy_slice(temp, result);
            }
        }
        fill_matrix(temp, 0.0);
        mul_matrix_slice(temp, mat_temp, mat_temp);
        cpy_slice(temp, mat_temp);
        pow >>= 1;
    }

    deallocate_matrix(temp);
    deallocate_matrix(mat_temp);

    return 0;
}

/*
 * Store the result of raising mat to the (pow)th power to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that pow is defined with matrix multiplication, not element-wise multiplication.
 */
int pow_matrix(matrix *result, matrix *mat, int pow) {
    /* TODO: YOUR CODE HERE */

    if (result->is_slice || mat->is_slice) {
        return pow_matrix_slice(result, mat, pow);
    }

    /** MAT, RESULT ROWS AND COLS */
    const int mat_rows = mat->rows;
    const int mat_cols = mat->cols;
//    const int result_rows = result->rows;
//    const int result_cols = result->cols;

    /** MAT, RESULT TRUE ROWS AND COLS */
//    const int mat_true_rows = mat->trueRows;
//    const int mat_true_cols = mat->trueCols;
//    const int result_true_rows = result->trueRows;
//    const int result_true_cols = result->trueCols;

    /** ALLOCATE SPACE FOR A TEMPORARY MATRIX */
    matrix *temp;
    matrix *mat_temp;
    if (allocate_matrix(&temp, mat_rows, mat_cols)) {
        return -1;
    }

    if (allocate_matrix(&mat_temp, mat_rows, mat_cols)) {
        deallocate_matrix(temp);
        return -1;
    }

    /** MAKE IDENTITY */
    make_identity(result);
    int i = 1;
    cpy_easy(mat, mat_temp);

    while (pow > 0) {
        if (pow & 1) {
            if (i) {
                cpy_easy(mat_temp, result);
                i--;
            } else {
                matrix_zero(temp);
                mul_matrix_no_slice(temp, result, mat_temp);
                cpy_exchange(temp, result);
            }
        }
        matrix_zero(temp);
        mul_matrix_no_slice(temp, mat_temp, mat_temp);
        cpy_exchange(temp, mat_temp);
        pow >>= 1;
    }

    deallocate_matrix(temp);
    deallocate_matrix(mat_temp);

    return 0;
}

int neg_matrix_slice(matrix *result, matrix *mat) {
    /** MAT ROWS AND COLS */
    int mat_rows = mat->rows;
    int mat_cols = mat->cols;

    for (int i = 0; i < mat_rows; ++i) {
        for (int j = 0; j < mat_cols; ++j) {
            set_2(result, i, j, -get_2(mat, i, j));
        }
    }

    return 0;
}
//
///*
// * Store the result of element-wise negating mat's entries to `result`.
// * Return 0 upon success and a nonzero value upon failure.
// */
//int neg_matrix(matrix *result, matrix *mat) {
//    /* TODO: YOUR CODE HERE  */
//
//    /** IF EITHER ONE OF THEM IS A SLICE */
//    if (result->is_slice || mat->is_slice) {
//        return neg_matrix_slice(result, mat);
//    }
//
//    /** MAT ROWS AND COLS */
//    int mat_rows = mat->rows;
//    int mat_cols = mat->cols;
//    /** RESULT ROWS AND COLS */
//    int result_rows = result->rows;
//    int result_cols = result->cols;
//    /** MAT TRUE ROWS AND TRUE COLS */
//    int mat_true_rows = mat->trueRows;
//    int mat_true_cols = mat->trueCols;
//    /** RESULT TRUE ROWS AND COLS */
//    int result_true_rows = result->trueRows;
//    int result_true_cols = result->trueCols;
//    /** RESULT DATA AND MAT DATA */
//    double *mat_data = mat->data;
//    double *result_data = result->data;
//
//    const int unroll = 8;
//    const int factor = 4 * unroll;
//    __m256d XOR__0_0 = _mm256_set1_pd(-0.0);
//#pragma omp parallel for
//    for (int i = 0; i < mat_rows; i += 1) {
//        for (int j = 0; j < mat_cols; j += factor) {
//            __m256d V[unroll];
//            for (int x = 0; x < unroll; ++x) {
//                V[x] = _mm256_loadu_pd(mat_data + x * 4 + i * mat_true_cols + j);
//            }
//            for (int x = 0; x < unroll; ++x) {
//                V[x] = _mm256_xor_pd(V[x], XOR__0_0);
//            }
//            for (int x = 0; x < unroll; ++x) {
//                _mm256_storeu_pd(result_data + x * 4 + i * result_true_cols + j, V[x]);
//            }
//
//        }
//    }
//
//    return 0;
//}
//
///*
// * Store the result of taking the absolute value element-wise to `result`.
// * Return 0 upon success and a nonzero value upon failure.
// */
//int abs_matrix(matrix *result, matrix *mat) {
//    /* TODO: YOUR CODE HERE */
//    /** MAT ROWS AND COLS */
//    int mat_rows = mat->rows;
//    int mat_cols = mat->cols;
//    /** RESULT ROWS AND COLS */
//    int result_rows = result->rows;
//    int result_cols = result->cols;
//    /** MAT TRUE ROWS AND TRUE COLS */
//    int mat_true_rows = mat->trueRows;
//    int mat_true_cols = mat->trueCols;
//    /** RESULT TRUE ROWS AND COLS */
//    int result_true_rows = result->trueRows;
//    int result_true_cols = result->trueCols;
//    /** RESULT DATA AND MAT DATA */
//    double *mat_data = mat->data;
//    double *result_data = result->data;
//
//    const int unroll = 8;
//    const int factor = 4 * unroll;
//    const int upj = mat_cols;
//    __m256d MASK = _mm256_set1_pd(-0.0);
//
//#pragma omp parallel for
//    for (int i = 0; i < mat_rows; i += 1) {
//        for (int j = 0; j < upj; j += factor) {
//            __m256d V[unroll];
//            for (int x = 0; x < unroll; ++x) {
//                V[x] = _mm256_loadu_pd(mat_data + x * 4 + i * mat_true_cols + j);
//            }
//            for (int x = 0; x < unroll; ++x) {
//                V[x] = _mm256_andnot_pd(MASK, V[x]);
//            }
//            for (int x = 0; x < unroll; ++x) {
//                _mm256_storeu_pd(result_data + x * 4 + i * result_true_cols + j, V[x]);
//            }
//        }
//    }
//    return 0;
//}


/*
 * Store the result of element-wise negating mat's entries to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int neg_matrix(matrix *result, matrix *mat) {
    /* TODO: YOUR CODE HERE  */

    /** IF EITHER ONE OF THEM IS A SLICE */
    if (result->is_slice || mat->is_slice) {
        return neg_matrix_slice(result, mat);
    }

    /** MAT ROWS AND COLS */
    int mat_rows = mat->rows;
    int mat_cols = mat->cols;
    /** RESULT ROWS AND COLS */
//    int result_rows = result->rows;
//    int result_cols = result->cols;
    /** MAT TRUE ROWS AND TRUE COLS */
//    int mat_true_rows = mat->trueRows;
    int mat_true_cols = mat->trueCols;
    /** RESULT TRUE ROWS AND COLS */
//    int result_true_rows = result->trueRows;
    int result_true_cols = result->trueCols;
    /** RESULT DATA AND MAT DATA */
    double *mat_data = mat->data;
    double *result_data = result->data;

    const int unroll = 8;
    const int factor = 4 * unroll;
    __m256d XOR__0_0 = _mm256_set1_pd(-0.0);


    /** DECLARE THE VECTORS */
    __m256d V0;
    __m256d V1;
    __m256d V2;
    __m256d V3;
    __m256d V4;
    __m256d V5;
    __m256d V6;
    __m256d V7;


#pragma omp parallel for private(V0, V1, V2, V3, V4, V5, V6, V7)
    for (int i = 0; i < mat_rows; i += 1) {
        for (int j = 0; j < mat_cols; j += factor) {

            /** LOAD THE DATA */
            V0 = _mm256_loadu_pd(mat_data + i * mat_true_cols + j);
            V1 = _mm256_loadu_pd(mat_data + i * mat_true_cols + j + 4);
            V2 = _mm256_loadu_pd(mat_data + i * mat_true_cols + j + 8);
            V3 = _mm256_loadu_pd(mat_data + i * mat_true_cols + j + 12);
            V4 = _mm256_loadu_pd(mat_data + i * mat_true_cols + j + 16);
            V5 = _mm256_loadu_pd(mat_data + i * mat_true_cols + j + 20);
            V6 = _mm256_loadu_pd(mat_data + i * mat_true_cols + j + 24);
            V7 = _mm256_loadu_pd(mat_data + i * mat_true_cols + j + 28);

            /** DO THE XOR */
            V0 = _mm256_xor_pd(V0, XOR__0_0);
            V1 = _mm256_xor_pd(V1, XOR__0_0);
            V2 = _mm256_xor_pd(V2, XOR__0_0);
            V3 = _mm256_xor_pd(V3, XOR__0_0);
            V4 = _mm256_xor_pd(V4, XOR__0_0);
            V5 = _mm256_xor_pd(V5, XOR__0_0);
            V6 = _mm256_xor_pd(V6, XOR__0_0);
            V7 = _mm256_xor_pd(V7, XOR__0_0);

            /** STORE THE RESULT */
            _mm256_storeu_pd(result_data + i * result_true_cols + j, V0);
            _mm256_storeu_pd(result_data + i * result_true_cols + j + 4, V1);
            _mm256_storeu_pd(result_data + i * result_true_cols + j + 8, V2);
            _mm256_storeu_pd(result_data + i * result_true_cols + j + 12, V3);
            _mm256_storeu_pd(result_data + i * result_true_cols + j + 16, V4);
            _mm256_storeu_pd(result_data + i * result_true_cols + j + 20, V5);
            _mm256_storeu_pd(result_data + i * result_true_cols + j + 24, V6);
            _mm256_storeu_pd(result_data + i * result_true_cols + j + 28, V7);

        }
    }

    return 0;
}


int abs_slice(matrix *result, matrix *mat) {
    /** MAT ROWS AND COLS */
    int mat_rows = mat->rows;
    int mat_cols = mat->cols;
    for (int i = 0; i < mat_rows; ++i) {
        for (int j = 0; j < mat_cols; ++j) {
            double val = get_2(mat, i, j);
            if (val < 0)
                val = -val;
            set_2(result, i, j, val);
        }
    }

    return 0;
}


/*
 * Store the result of taking the absolute value element-wise to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int abs_matrix(matrix *result, matrix *mat) {
    /* TODO: YOUR CODE HERE */

    /** IF EITHER OF THEM IS A SLICE */
    if (result->is_slice || mat->is_slice) {
        return abs_slice(result, mat);
    }

    /** MAT ROWS AND COLS */
    int mat_rows = mat->rows;
    int mat_cols = mat->cols;
    /** RESULT ROWS AND COLS */
//    int result_rows = result->rows;
//    int result_cols = result->cols;
    /** MAT TRUE ROWS AND TRUE COLS */
//    int mat_true_rows = mat->trueRows;
    int mat_true_cols = mat->trueCols;
    /** RESULT TRUE ROWS AND COLS */
//    int result_true_rows = result->trueRows;
    int result_true_cols = result->trueCols;
    /** RESULT DATA AND MAT DATA */
    double *mat_data = mat->data;
    double *result_data = result->data;

    int unroll = 8;
    int factor = 4 * unroll;


    __m256d MASK = _mm256_set1_pd(-0.0);

    /** DECLARE THE VECTORS */
    __m256d V0;
    __m256d V1;
    __m256d V2;
    __m256d V3;
    __m256d V4;
    __m256d V5;
    __m256d V6;
    __m256d V7;

#pragma omp parallel for private(V0, V1, V2, V3, V4, V5, V6, V7)
    for (int i = 0; i < mat_rows; i++) {
        for (int j = 0; j < mat_cols; j += factor) {

            /** LOAD THE DATA */
            V0 = _mm256_loadu_pd(mat_data + i * mat_true_cols + j);
            V1 = _mm256_loadu_pd(mat_data + (i) * mat_true_cols + j + 4);
            V2 = _mm256_loadu_pd(mat_data + (i) * mat_true_cols + j + 8);
            V3 = _mm256_loadu_pd(mat_data + (i) * mat_true_cols + j + 12);
            V4 = _mm256_loadu_pd(mat_data + (i) * mat_true_cols + j + 16);
            V5 = _mm256_loadu_pd(mat_data + (i) * mat_true_cols + j + 20);
            V6 = _mm256_loadu_pd(mat_data + (i) * mat_true_cols + j + 24);
            V7 = _mm256_loadu_pd(mat_data + (i) * mat_true_cols + j + 28);

            /** PERFORM AND NOT */
            V0 = _mm256_andnot_pd(MASK, V0);
            V1 = _mm256_andnot_pd(MASK, V1);
            V2 = _mm256_andnot_pd(MASK, V2);
            V3 = _mm256_andnot_pd(MASK, V3);
            V4 = _mm256_andnot_pd(MASK, V4);
            V5 = _mm256_andnot_pd(MASK, V5);
            V6 = _mm256_andnot_pd(MASK, V6);
            V7 = _mm256_andnot_pd(MASK, V7);

            /** PERFORM STORE */
            _mm256_storeu_pd(result_data + i * result_true_cols + j, V0);
            _mm256_storeu_pd(result_data + i * result_true_cols + j + 4, V1);
            _mm256_storeu_pd(result_data + i * result_true_cols + j + 8, V2);
            _mm256_storeu_pd(result_data + i * result_true_cols + j + 12, V3);
            _mm256_storeu_pd(result_data + i * result_true_cols + j + 16, V4);
            _mm256_storeu_pd(result_data + i * result_true_cols + j + 20, V5);
            _mm256_storeu_pd(result_data + i * result_true_cols + j + 24, V6);
            _mm256_storeu_pd(result_data + i * result_true_cols + j + 28, V7);

        }
    }
    return 0;
}
