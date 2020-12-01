NOT TO BE IMPLEMENTED IN SAFRN 1.0

# Lasso Regression

Lasso regression, like [Linear Regression](./Linear-Regression)
seeks to fit a linear model, but uses a different error function. The
resulting solution often has fewer non-zero coefficients, which in
effect reduces the number of features of the model.

# Inputs and outputs
- $`n`$ (in: _integer_) The number of records.
- $`p`$ (in: _integer_) The number of independent variables.
- $`Y`$ (in: _vector of floats_) $`n`$ many values of the dependent
  variable.
- $`X`$ (in: _matrix of floats_) Each of the $`n`$ many rows in $`X`$
  corresponds to a value in $`Y`$ and is a vector of $`p`$ many
  independent variable values which are presumed to determine the
  corresponding value in $`Y`$. It is common for $`X_{i,0}`$ to be
  given a value of 1, representing a constant term.
- $`\lambda`$ (in: _float_) a non-negative number determining the
  amount of regularization.
- $`w`$ (out: _vector of floats_) Each element of $`w`$ is a
  coefficient for one of the $`p`$ many independent variables.
- $`\varepsilon`$ (out: _vector of floats_) $`n`$ error values
  denoting the difference between each value of $`Y`$ and the value
  predicted by the regression model.

# Non-Secure Implementation

The error function is very similar to [Ridge
Regression](./Linear-Regression#Ridge-Regression), but we use the
$`l^1`$-norm on the regularization term.

$`Error(w) := \frac{1}{2n} \| \varepsilon \|_2^2 +  \lambda\| w \|_1,`$

Unlike OLS and Ridge Regression, there is no closed form solution. We
rely instead on [gradient descent](./Gradient-descent).

# Secure Implementation

See [gradient descent](./Gradient-descent).
