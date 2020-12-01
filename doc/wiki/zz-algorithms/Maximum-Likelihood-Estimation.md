**NOT TO BE IMPLEMENTED IN SAFRN1**

# Maximum Likelihood Estimation (MLE)
Maximum likelihood estimate is a statistical inference technique where we calculate the parameters of a probability distribution for a given a sample of input data and for an assumed model. The goal of MLE is to make inference about the population that is most likely to have generated the sample[1]

# Inputs and outputs
* **X**(in: *vector of reals*) data set of real numbers
* **m**(in: *model*) assumed model

# Non-Secure Implementation
Let _n_ be the size of vector X.

For normal distribution,  parameters are[2]
```math
    mean = \frac{\sum_{i=1}^nx[i]}{n}
```

```math
   \hspace{4em} variance =  \frac{\sum_{i=1}^n(x[i]-mean)^2}{n}
```

For Bernoulli distribution[2],
```math
    probability =  \frac{\sum_{i=1}^nx[i]}{n}
```

For Poisson distribution[2],
```math
     \lambda =  \frac{\sum_{i=1}^nx[i]}{n}
```

### Iterative/Numerical Methods
Newton-Raphson  method and Fisher's scoring methods are widely used for solving the likelihood equations[4].

Newton-Raphson method assigns initial values as approximation to the roots and expanded using Taylor's theorem; these are solved using Cramer's rule to derive delta of approximations. It is repeated until the desired level of accuracy.

Fisher's scoring method is modified version of Newton-Raphson's method where observed information is replaced with expected Fisher Information matrix.

Convergence of both the above methods depends on selection of initial estimates; they may never converge if the matrix ever gets closer to singular.


# References
1 https://en.wikipedia.org/wiki/Maximum_likelihood_estimation
2 http://mathworld.wolfram.com/MaximumLikelihood.html
3 http://galton.uchicago.edu/~eichler/stat24600/Handouts/l02.pdf
4 https://www.isid.ac.in/~deepayan/SC2010/project-sub/ABO_NR_report.pdf
