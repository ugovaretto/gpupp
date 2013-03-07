#ifdef DOUBLE
#pragma OPENCL EXTENSION cl_khr_fp64: enable
typedef double real_t;
#else
typedef float real_t;
#endif

// return matrix element given block and indices of element in block
inline real_t get_matrix_element( __global const real_t* restrict m, //matrix
                             int blockCol,    //column index of output block 
                             int blockRow,    //row index of output row
                             int col,         //local column index of block element
                             int row,         //local row index of block element 
                             int num_columns,  //number of columns of matrix 'm'
                             int blockDimx,
                             int blockDimy
                           ) {                                           
  
    return m[ ( blockRow * blockDimy + row ) * num_columns + blockCol * blockDimx + col ];

}

#define TILE_SIZE 16
__kernel void
MatMul( __global const real_t* restrict A, 
        __global const real_t* restrict B,
        __global real_t* restrict C,
        int width,
        int height ) {

    __local real_t M1[TILE_SIZE][TILE_SIZE];
    __local real_t M2[TILE_SIZE][TILE_SIZE];
    const int blockRow = get_group_id(1); 
    const int blockCol = get_group_id(0);
    const int row = get_local_id(1);
    const int col = get_local_id(0);
    const int blockDimx = get_local_size(0);
    const int blockDimy = get_local_size(1);
    //printf("local size 0, 1: %d, %d\n", blockDimx, blockDimy);
    real_t out = 0;
    for( int b = 0; b < width / TILE_SIZE; ++b ) {
          //copy data into shared memory
        M1[ row ][ col ] = get_matrix_element( A, b, blockRow, col, row, width,
                                               blockDimx, blockDimy );
        M2[ row ][ col ] = get_matrix_element( B, blockCol, b, col, row, width,
                                               blockDimx, blockDimy );
        barrier(CLK_LOCAL_MEM_FENCE); // required to guarantee that data are computed before next step
                                      // where a thread accesses data computed by other threads
        for( int c = 0; c != TILE_SIZE; ++c ) {
            out += M1[ row ][ c ] * M2[ c ][ col ];           
        }
        barrier(CLK_LOCAL_MEM_FENCE); // required to avoid that some threads start modifying
                         // data in cache before all threads have exited for loop    
    }
    const int idx = ( blockRow * blockDimy + row ) * width + blockCol * blockDimx + col;
    C[ idx ] = out;     

}
                         
                                      
  