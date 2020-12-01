# Moments
Statistical moments are the quantitative measures of a probability
function.

- First moment - mean or the expected value
- Second moment - variance
- Third moment - skewness
- Fourth moment - kurtosis


# Inputs and outputs

- $`n`$ (in: integer) the number of elements
- $`X`$ (in: *vector of reals*) a vector of $`n`$ elements.


# Non-Secure Implementation


Let n be size of the vector x, then the mean is the ratio between
  sum of elements and the size of the data set[1]

```math
    mean = E(X) = \frac{\sum_{i=1}^nx[i]}{n}
```

The variance is the ratio between squared deviation from the mean and
the size of the data set[2]

```math
           variance = \sigma^2 =  \frac{\sum_{i=1}^n(x[i]-mean)^2}{n}
```
Skewness measures the asymmetry of the distribution (tells how the mass is distributed). For a sample of n values, it is calculated as the ratio between average cubed deviation to  cubed sample standard deviation[3]
```math
    skewness =  \frac{(\sum_{i=1}^n(x[i]-mean)^3) \div n}{ [\sum_{i=1}^n(x[i]-mean)^2 \div (n-1)]^{3/2}}
```

Kurtosis is the fourth moment of the distribution and it is a measure of tailedness; kurtosis of normal distribution is 3. For a sample of n data points, excess kurtosis is calculated as below[4]
```math
    kurtosis = \frac{ (\sum_{i=1}^n(x[i]-mean)^4  \div n}{[(\sum_{i=1}^n(x[i]-mean)^2 \div n]^2} - 3
```

### Normalized central moments
Normalized n-th central moment or standardized moment[5] is the n-th central moment divided by $`\sigma^n`$

```math
\frac{\mu_n}{\sigma^n} = \frac{\operatorname{E}\left[(X - \mu)^n\right]}{\sigma^n}
```
```math
\tilde{\mu}_1 = \frac{\mu_1}{\sigma^1} = \frac{\operatorname{E} \left[ ( X - \mu )^1 \right]}{( \operatorname{E} \left[ ( X - \mu )^2 \right])^{1/2}} = \frac{\mu - \mu}{\sqrt{ \operatorname{E} \left[ ( X - \mu )^2 \right]}} = 0

```

```math
\tilde{\mu}_2 = \frac{\mu_2}{\sigma^2} = \frac{\operatorname{E} \left[ ( X - \mu )^2 \right]}{( \operatorname{E} \left[ ( X - \mu )^2 \right])^{2/2}} = 1
```
```math
\tilde{\mu}_3 = \frac{\mu_3}{\sigma^3} = \frac{\operatorname{E} \left[ ( X - \mu )^3 \right]}{( \operatorname{E} \left[ ( X - \mu )^2 \right])^{3/2}}

```

```math

\tilde{\mu}_4 = \frac{\mu_4}{\sigma^4} = \frac{\operatorname{E} \left[ ( X - \mu )^4 \right]}{( \operatorname{E} \left[ ( X - \mu )^2 \right])^{4/2}}
```

### Alternate expressions

The $`k`$-th moment and related quantities can be expressed in terms of $`\operatorname{E}[X^k]`$ and lower-order moments, e.g.

```math
\sigma^2 = \operatorname{E}[X^2] - \mu^2
```
and
```math
skewness = \frac{\operatorname{E}[X^3] - 3\mu \sigma^2 - \mu^3}{\sigma^3}
```

# Secure Implementation

We describe how to compute $`E[X^k]`$ for any $`k`$. As shown above, this is sufficient to compute all higher order moments. In turn, to compute $`E[X^k]`$, we need to compute $`n`$, the count, and the sum of $`x^k`$ over all elements $`x`$.

One vertical: Each party computes their count and sum of $`x^k`$ locally, then secret shares and sums.

Two verticals: For two verticals $P$ and $Q$ divided horizontally into parties $P_i$ and $Q_j$, respectively, we run for every pair $`(P_i,Q_j)`$ a PSI-COUNT and PSI-SUM, where the payload for the PSI-SUM is $`x^k`$. Then we again secret share and sum the results.

### Complexity

With only one vertical, the communication complexity is $`O(m)`$. For two verticals, it is O(PSI-SUM).

# References
1 https://en.wikipedia.org/wiki/Moment_(mathematics)
2 https://en.wikipedia.org/wiki/Variance
3 https://en.wikipedia.org/wiki/Skewness
4 https://en.wikipedia.org/wiki/Kurtosis
5 https://en.wikipedia.org/wiki/Standardized_moment
