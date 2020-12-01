To be implemented in SAFRN 1.0 (for Ridge Regression.)

# Matrix Inverse
B is the matrix inverse of A, if BA = AB = Identity Matrix.

```math
  AA^T=I
```

# Inputs and outputs
* **A**(in: *Matrix*) n-by-n square matrix
* **B**(out: *Matrix*) n-by-n square matrix

# Non-Secure Implementation
Preconditions:
     a) Matrix A is non-singular (that is determinant is not zero),
     b) Matrix A is square matrix.

Inverse is calculated by dividing the cofactor[2] matrix of A with its determinant[2]


```math
    A^{-1} = \frac{(cofactorsMatrix)^T}{determinant}
```

### Cholesky Decomposition
Alternatively, matrix inverse can also be calculated by Cholesky decomposition[3], by back substituting twice as follows.

If L is lower triangular matrix and U is upper triangular matrix, then

```math
  A = LL^T = U^TU
```
```math
  A^{-1} =   U^{-1}U^{{-1}T}
```
But  $`UU^{-1}=I`$, hence $`U^{-1}`$ can be calculated by back substitution.

Finally, $`A^{-1}`$ can be calculated by  back substituting $`U^{-1}`$ into
```math
  A^{-1} =   U^{-1}U^{{-1}T}
```

# References
https://en.wikipedia.org/wiki/Invertible_matrix
https://en.wikipedia.org/wiki/Minor_(linear_algebra)#Inverse_of_a_matrix
https://arxiv.org/ftp/arxiv/papers/1111/1111.4144.pdf
