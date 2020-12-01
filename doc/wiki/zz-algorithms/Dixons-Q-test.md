**NOT TO BE IMPLEMENTED IN SAFRN1**

# Dixon's Q test
It is used for identification and rejection of outliers in a given data set.

# Inputs and outputs
* **X**(in: *vector of reals*) data set of real numbers
* **Q**(out: *real number*) Dixon's score

# Non-Secure Implementation
Order the input data in ascending order and apply the following formula to detect if the lowest element is the outlier[1]

```math
    Q = \frac{gap}{range} = \frac{absolute(lowestElement - secondLowestElement)}{(highestElement - lowestElement)}
```


Above Q value is compared with the reference value obtained from Q table in accepting or rejecting the outlier. Also, Dixon provided tests for identifying more than 1 outlier but they are less frequently used.

# References
https://en.wikipedia.org/wiki/Dixon%27s_Q_test
