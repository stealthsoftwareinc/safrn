To be implemented in SAFRN 1.0

# Ranking
Ranking is a relationship among elements of a data set. For two elements $`x`$ and $`y`$, either $`x`$ is 'ranked higher than' $`y`$, or $`x`$ is 'ranked lower than' $`y`$, or $`x`$ is 'ranked equal to' $`y`$[1].
We typically use the standard numerical ordering as our ranking.

One desirable functionality in the system is element retrieval by rank. Allowing retrieval of an arbitrarily ranked element would sacrifice the privacy of the data set, as querying every rank in increasing order would reveal the entire set.
However, there are a few ranks, which are considered on their own not to reveal the set, because their ranks are fixed. Though this does reveal some information about the set, it is agreed in advance that the analyst should learn this information.
We allow the following ranks to be revealed:

 - **Minimum**: The lowest ranked element.
 - **Maximum**: The highest ranked element.
 - **Median**: The element ranked at the very center. That is there are as many higher ranked elements as there are lower ranked elements. If there are two central elements, we average them. (And if the array is integer-valued, we round to avoid leaking the parity of the array size).

# Inputs and outputs
 - **Xin**(in: *vector of reals*) data set of real numbers
 - **Xout**(out: *vector of integers*) corresponding ranks for each element of **Xin**.

SAFRN will not reveal the entirety of the ranking, as that would reveal the entire dataset. Instead, only the minimum, maximum, and median values will be reveal.

# Non-Secure Implementation
Order the input data set in ascending order and assign ordinal value to each element in the increasing order. If there are duplicates in the data set, assign the same ordinal value to each duplicate but skip one value for each duplicate when assigning the next ordinal value.

# Secure Implementation

## Minimum and maximum

### One Vertical

Each party computes their minimum and maximum locally. Then, they use comparison circuits among each other to compute the global minimum and maximum.
  
This requires running $`m`$ comparison circuits, which requires $`O(m^2b)`$ bits of communication, in $`O(\log m)`$ rounds.
    
### Two Verticals
  
We implement the two vertical minimum and maximum protocol the same way we implement the other ranking protocols (below).

As an alternative, we could run PSO+MIN via the ranking protocol pairwise, and then run the 
single vertical MPC (above) to compare the $`\ell m`$ pairwise minimums. This would require $`O(m\ell(m+\ell)b)`$ communication 
and $`O(\log ell + \log m)`$ rounds, in addition to the cost of the pairwise PSO+MIN protocols. 
This would probably be faster than the general ranking protocol, 
but only if we aren't already computing other rankings, 
and so might be too much of a headache on the engineering end.

#### Complexity

Communication complexity is $`O(m^2b)`$ bits of communication, in $`O(\log m)`$ rounds for the case 
with one vertical and $`m`$ parties. For two verticals, 
with $`m`$ parties total, communication complexity is equal to the cost of running two $`m`$ party Shuffle-Sorts, 
i.e. $`O(m^2 n \log(mn)b)`$ communication with $`O(m+\log n)`$ rounds (where $`b`$ is bits per packet).


## Median, quartiles, and other rankings


### One Vertical

We use a PISO secure sort, sorting by payloads, 
on all horizontal data sets. We then get a secret 
sharing of the total count, which we compute by 
running a parallel computation of the indicator function 
testing for equality to $`\bot`$, and summing the 
shared results. We then compute the desired index 
to extract via lightweight MPC (for arbitrary rankings, this requires a floor function of a quotient, 
which we can compute by subtracting off the residue $`\mod q`$ 
and then computing the quotient over $`\mathbb{F}_p`$ via 
local scalar multiplication). To extract the element 
in the desired position, we run a parallel computation 
of the indicator function testing whether the index 
is equal to the secret desired index. We then compute 
the secure dot product of this indicator function with the payload vector.

### Two Verticals
We begin with a PISO secure sort, sorting by keys, 
and run the compare step to remove all payloads that 
do not meet the PSO condition. We then run a second 
SISO sort on the resulting list, 
and proceed afterwards as in the one vertical case.

#### Complexity

The communication complexity in the one vertical case is 
the cost of a PISO secure sort plus three 
additional parallel computations of lightweight MPC, 
for $`O(m^2n \log \log n b)`$ bits of communication, 
with $`O(m)`$ rounds, assuming $`m = \Omega(\log n)`$ (see REF), 
or $`O(m^2 n \log(n)b)`$ communication without secure merge. 

The two vertical case requires an additional SISO secure sort, 
which increases the cost to $`O(m^2 n \log(n)b)`$ communication 
with $`O(m+\log n)`$ rounds (where $`b`$ is bits per packet).

# References
 - https://en.wikipedia.org/wiki/Ranking
 - https://en.wikipedia.org/wiki/Median
