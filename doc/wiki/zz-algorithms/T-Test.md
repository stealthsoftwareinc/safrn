# T test
T test is an inferential static used to test the hypothesis of
population, based on a given sample. It can be used as
- One sample test to test if the mean of the population is a specific
  value.
- Two sample test to test if the means of 2 populations are the same.

# Inputs and outputs
-  $`\overline X`$ (in: _real value_) represents the sample mean.
- $`\mu_0`$ (in: _real value_) hypothesized mean value of the population
- $`s`$ (in: _sample standard deviation_)
- $`n`$ (in: _real value_) sample size
- $`\overline {X1}`$(in: _real value_) represents the sample mean of first sample
- $`\overline {X2}`$(in: _real value_) represents the sample mean of second sample
- $`s_p`$  (in: _real value_) represents the pooled standard deviation of first and second sample
- $`t`$ (out: _real value_) t score


# Non-Secure Implementation
One sample test to check if population mean is $`\mu_0`$, [is calculated as follows](1)
```math
     t = \frac{(\overline X - \mu_0)} {s/\sqrt{\smash[b]{n}}}.
```

Two sample test to check if means of 2 populations is same, [is calculated as follows][1]
```math
     t =  \frac{(\overline {X1} - \overline {X2})}{ s_p*\sqrt{\smash[b]{\frac{2}{n}}}  }
```
where pooled standard deviation is derived from unbiased variance of the 2 samples as follows
```math
  s_p = \sqrt{\frac{s_{X_1}^2+s_{X_2}^2}{2}}
```
Above t value is used to look up t-distribution table in determining if hypothesis can be rejected or not.

# Secure Implementation

This [paper][2] describes an implementation of Student's t-test using
partially homomorphic encryption plus MPC for division.

Some observations:
- If you report the square of the test statistic t, you don't need to compute square roots.
- The numerator and denominator each have multiplicative depth of at
  most 3, which is good for PHE.

[1]: https://en.wikipedia.org/wiki/Student's_t-test
[2]: https://pdfs.semanticscholar.org/dea8/4182dd302a3ae97e0f577c0e7f9a1fe38a7d.pdf
