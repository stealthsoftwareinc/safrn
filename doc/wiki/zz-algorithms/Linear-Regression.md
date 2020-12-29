# Linear Regression

Given a set of observations $`(X_0, Y_0),...,(X_n, Y_n)`$, we attempt to
fit a linear model relating $`X_i`$ and $`Y_i`$, , i.e there exists constants
$`w_0, w_1`$ such that
```math
Y_i = w_1X_i + w_0 + \varepsilon_i,
```

for $`i=0,...n`$. The goal is to find constants $`w_0, w_1`$ that
minimize the error terms $`\varepsilon_i`$.

## Inputs and outputs
- $`n`$ (in: _integer_) The number of records.
- $`p`$ (in: _integer_) The number of independent variables.
- $`Y`$ (in: _vector of floats_) $`n`$ many values of the dependent
  variable.
- $`X`$ (in: _matrix of floats_) Each of the $`n`$ many rows in $`X`$
  corresponds to a value in $`Y`$ and is a vector of $`p`$ many
  independent variable values which are presumed to determine the
  corresponding value in $`Y`$. It is common for $`X_{i,0}`$ to be
  given a value of 1, representing a constant term.
- $`w`$ (out: _vector of floats_) Each element of $`w`$ is a
  coefficient for one of the $`p`$ many independent variables.
- $`\varepsilon`$ (out: _vector of floats_) $`n`$ error values
  denoting the difference between each value of $`Y`$ and the value
  predicted by the regression model.

## Non-Secure Implementation

The method of Ordinary Least Squares seeks to find a vector $`w`$ minimizing the $`l^2`$-norm of the error in the over-determined expression:
```math
Xw=Y
```

If $`(X^TX)`$ is non-singular, there
is closed-form solution $`w = (X^TX)^{-1}X^Ty`$.

Ridge Regression is similar to Ordinary Least Squares, but we add a
penalty to the error term to shrink the coefficients $`w`$, via $`\lambda`$, an adjustable, non-negative parameter.

As in the OLS case, there is a closed form solution:
```math
    w = (X^TX + \lambda I)^{-1}X^TY.
```

A more detailed overview can be found in many sources, e.g. [Wikipedia][1].

Many tools exist for approximating the solution to a linear equation. [Scikit-learn][5] has, e.g., SVD and stochastic gradient descent. However, in SAFRN $`d`$ will be sufficiently small that we can compute the solution exactly via Cholesky decomposition (up to the rounding error already introduced by using fixed point arithmetic).

## Secure Implementation

The secure implementation is also computed via Cholesky decomposition,
closely following [this paper](#2). The full details are given
[here](/doc/wiki/zz-algorithms/Linear-Regression-Secure-Impl.md)

## Root Mean Square Error
The Root Mean Square Error (commonly referred to as the "standard error of the regression," the "standard error of the estimate," or (in the summary output of the R `lm` function) the "residual standard error") is an estimate of the standard deviation of the error $`\varepsilon`$.  For a model with an intercept, it is computed as
```math
\widehat \sigma := \sqrt{\frac{1}{n - p - 1} \sum_{j=1}^n (y_j - \widehat y_j)^2 },
```
and for a model with no intercept as
```math
\widehat \sigma := \sqrt{\frac{1}{n - p} \sum_{j=1}^n (y_j - \widehat y_j)^2 }.
```

## Standard Errors of the Coefficients
The standard error $`\operatorname{se}(\widehat w_j)`$ of the $`j`$th regression coefficient is an estimate of the standard deviation of the sampling distribution of $`w_j`$ (i.e., the distribution obtained by repeatedly computing $`w_j`$ as above using random samples of size $`n`$ from the population).

To compute the standard errors, first form the matrix $`A = (X^T X)^{-1}`$ (we address here only the case in which $`X^TX`$ is nonsingular).  Then the standard error of the $`j`$th coefficient is computed as
```math
\operatorname{se}(\widehat w_j) = \widehat \sigma \sqrt{A_{jj}},
```
where $`\widehat \sigma`$ is the standard error of the regression defined above.

Note 1:  In the case of a regression model with an intercept term, we assume that the "zeroth column" of the design matrix $`X`$ has been set to all 1s, as described above.

Note 2:  The formula for $`\operatorname{se} (\widehat w_j)`$ implicitly depends on whether or not the model has a constant term, due to the different "degrees of freedom" corrections appearing in the formulas for $`\widehat \sigma`$.

Note 3:  The validity of $`\operatorname{se}(\widehat w_j)`$ as an estimate of the sampling standard deviation of the regression coefficients relies on an assumption of "homoskedasticity," i.e. that the conditional population variance $`\operatorname{Var}(\varepsilon | X = x)`$ of the error term $`\varepsilon`$ does not depend on $`x`$.

## Tests

The F-test and t-test are two hypothesis tests to help you determine
the significance of your linear model.

### F test

The F-test determines if the entire model is statistically significant
by comparing the model to the *constant* model, where all of the
non-constant components of $`w_i, i \neq 0`$ are zero.

In the following formulas for the $`F`$ statistic associated to a regression model, $`\hat y_j`$ is the predicted value of $`Y`$ when the model is tested on the independent variables from the $`j`$-th sample, and $`\overline y := \frac{1}{n}\sum_{j=1}^n y_j`$ is the sample mean of the dependent variable.

**Model with intercept ("affine" case)**
```math
F = \frac{(\operatorname{SSR}_{\operatorname{const}} - \operatorname{SSR}_{\operatorname{model}}) / p}{\operatorname{SSR}_{\operatorname{model}} / (n - p - 1) } = \frac{ \Big( \sum_1^n (y_j - \overline y)^2 - \sum_1^n (y_j - \widehat y_j)^2\Big) / p}{ \Big( \sum_1^n (y_j - \widehat y_j)^2 \Big) / (n - p - 1) }.
```
This $`F`$ statistic follows an $`F_{(p, n-p-1)}`$ distribution, from which one can compute an associated $`p`$-value to assess the overall statistical significance of the model.

**Model without intercept ("linear" case)**
```math
	F = \frac{(\operatorname{SSR}_{0} - \operatorname{SSR}_{\operatorname{model}}) / p}{\operatorname{SSR}_{\operatorname{model}} / (n-p)} = \frac{ \Big(\sum_1^n y_j^2 - \sum_1^n (y_j - \widehat y_j)^2 \Big) / p}{ \Big( \sum_1^n (y_j - \widehat y_j)^2 \Big) / (n-p) }.
```

This $`F`$ statistic follows an $`F_{(p, n-p)}`$ distribution, from which one can compute an associated $`p`$-value to assess the overall statistical significance of the model.

Note that there are two differences in the computation of the $`F`$ statistic between the with-intercept and no-intercept cases:
- The sample mean $`\overline{y}`$ is _not_ subtracted from the $`y_j`$ in the first sum of squared residuals in the numerator for the no-intercept case.
- The "denominator degrees of freedom" in the $`F`$ statistic (and the corresponding $`F`$-distribution) is $`n-p`$ in the no-intercept case and $`n-p-1`$ in the with-intercept case.


### t test

For any coefficient $`w_i`$, the t-test tests the assumption that that
coefficient is not significant versus the alternative.

The $`t`$ statistic associated to the $`j`$th regression coefficient is computed as
```math
t_j = \frac{w_j}{\operatorname{se}(\widehat w_j)},
```
where the denominator is the standard error of the coefficient as defined above.

This $`t`$ statistic follows a Student's $`t`$ distribution with $`n - p - 1`$ degrees of freedom if the model includes an intercept term and $`n - p`$ degrees of freedom if the model has no intercept term; one can use this fact to compute an associated $`p`$-value for each coefficient's $`t`$ statistic.


[1]: https://en.wikipedia.org/wiki/Ordinary_least_squares
[5]: https://scikit-learn.org/stable/modules/generated/sklearn.linear_model.Ridge.html
[2]: https://eprint.iacr.org/2019/773
