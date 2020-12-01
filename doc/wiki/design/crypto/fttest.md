# F-test and t-test secure implementation

## t-test

To compute the t-test, we need to compute the t-score as follows:

```math  
\frac{(\hat{\beta} - \beta_0)\sqrt{(n-2) \sum (x_i - \overline{x})^2} }{\sqrt{\sum (y_i - \hat{\alpha} - \hat{\beta} x_i)^2}}
```

To avoid taking secure square roots, we instead compute the square of the t-score.

The sums we expand as follows:  
```math
\sum (x_i - \overline{x})^2 = \sum x_i^2 - 2\overline{x} \sum x_i + n \overline{x}^2
``` 
and
```math
\sum (y_i - \hat{\alpha} - \hat{\beta} x_i)^2 = \sum y_i^2 + \hat{\alpha^2} + \hat{\beta^2} \sum x_i^2 - 2 \hat{\alpha} \sum y_i - 2 \hat{\beta} \sum x_i y_i - 2 \hat{\alpha \beta} \sum x_i
```

Thus while computing the regression we need to compute shares of the additional terms $`n`$ (i.e. the count), $`\sum x_i`$, $`\sum x_i^2`$, $`\sum y_i`$, $`\sum y_i^2`$, and $`\sum x_i y_i`$.

## F-test

This is similar.

For the F-test on a dataset divided by a categorical variable, we need to compute the quotient of the "explained variance", or "between-group variability":

```math
\sum _{i=1}^{K}n_{i}({\bar {Y}}_{i\cdot }-{\bar {Y}})^{2}/(K-1)}{\displaystyle \sum _{i=1}^{K}n_{i}({\bar {Y}}_{i\cdot }-{\bar {Y}})^{2}/(K-1)}
```
where $`\bar  {Y}_{i\cdot }`$ denotes the sample mean in the i-th group, $`n_{i}`$ is the number of observations in the i-th group, $`\bar {Y}`$ denotes the overall mean of the data, and $`K`$ denotes the number of groups;

And the "unexplained variance", or "within-group variability":

```math
\sum _{i=1}^{K}\sum _{j=1}^{n_{i}}\left(Y_{ij}-{\bar {Y}}_{i\cdot }\right)^{2}/(N-K)
```

These terms can each be computed by the appropriate linear combination of $`\sum_{j} Y_{ij}^2`$ and $`\sum_{j} Y_{ij}`$ over all choices of $`i`$.
