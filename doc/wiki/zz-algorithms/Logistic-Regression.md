# Logistic Regression

Logistic Regression is a popular classification algorithm where we
assume (in the binary case) that the `log-odds` of the dependent
variable is linear in the (possibly many) independent variables $`X`$.

```math
\log \frac{Y}{1-Y} = w^T X \\
\Rightarrow Y = \frac{e^{w^T X}}{1 + e^{w^T X}}
```

# Inputs and outputs
- $`n`$ (in: _integer_) The number of records.
- $`p`$ (in: _integer_) The number of independent variables.
- $`Y`$ (in: _vector of floats_) The vector of values of the dependent
  variable.
- $`X`$ (in: _matrix of floats_) Each of the $`n`$ many rows in $`X`$
  corresponds to a value in $`Y`$ and is a vector of $`p`$ many
  independent variable values which are presumed to determine the
  corresponding value in $`Y`$. It is common for $`X_{i,0}`$ to be
  given a value of 1, representing a constant term.
- $`w`$ (out: _vector of floats_) Each element of $`w`$ is a
  coefficient for one of the $`p`$ many independent variables.

# Non-Secure Implementation

Use [gradient descent](./Gradient-descent) to minimize the negative
of the log-likelihood error:
```math
    \min_{w} \sum_{i=1}^n (\log(1 + e^{w^TX_i}) - Y_iw^Tx_i).
```

As with Linear Regression, there are also regularized versions, where
we had a penalty on the size of the coefficients $`w`$.

# Secure Implementation
This [paper](https://eprint.iacr.org/2018/074)\[Kim et al. 2018\]
gives an algorithm for logistic regression using homomorphic
encryption to do gradient descent. It claims to be better than this
[paper](https://eprint.iacr.org/2017/396), \[Mohassel Zhang 2017\]
which uses garbled circuits to do stochastic gradient descent. The
speed of the Mohassel Zhang paper is partly due to some arithmetic
approximations which (according to the paper) have a negligible effect
on accuracy.  Also, see [tf-encrypted][1] (uses SPDZ) and [EzPC][2]
for potential implementations, both in Python.


# References
[1]: https://github.com/tf-encrypted/tf-encrypted
[2]: https://github.com/mpc-msri/EzPC
