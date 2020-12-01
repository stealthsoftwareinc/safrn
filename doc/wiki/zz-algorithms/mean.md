# Mean
The mean is one measure of the "middle" of a distribution.
It is an easy to calculate, and often  is either a predictable expected value, or as a baseline for comparison.
A synonym of mean is average.

## Inputs and outputs
 - $`D`$ (in: _vector of numbers_) All values in the given dataset.
 - $`n`$ (in: _integer_) Cardinality of length of $`D`$.
 - $`m`$ (out: _number_) The mean value of the dataset.

## Cleartext Algorithm
The Mean is calculated as follows:

```math
  m = \frac{\sum_{i=1}^{n} D_i}{n}
```

## Secure Implementation
 - PSI Requirements: PSI-Function
 - Randomness Requirements: depends on PSI implementation

The mean can most efficiently be calculated as a function during PSI, in which the PSI implementation sums and counts elements in the intersection, instead of emitting them.

## Implementation Status
Mean is not currently implemented.
