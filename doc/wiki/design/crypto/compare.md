

## Comparison protocols

### Introduction
  
The following protocols are used for PSO, as well as in our ridge regression protocol. This is also described in [#72](https://gitlab.stealthsoftwareinc.com/stealth/safrn/-/issues/72).
  
The cost of all protocols are described for a computation done by $`m`$ parties with a prime $`p`$ that is $`\ell`$ bits long.
  
See also the related [modular conversion protocols](bitconv).  

### Protocols

#### mod p comparison without BitDecomp

Due to [1]

Choose $`p`$ sufficiently large that all blocks of data are represented as an integer $`x`$ satisfying $`0 \leq x < p/2`$.

Begin with mod-p shares $`[x]`$ and $`[y]`$ of integers x and y.

Compute $`[x-y] \mod p`$. Then test whether $`[x-y] \mod p`$ is in the interval $`[0,p/2]`$, which is equal to $`1 - [\textrm{LSB}([2x-2y]) \mod p]`$

Cost: 1 LSB computation $`= 4*m*ell + o(m*ell)`$ communication and 11 rounds.



#### LSB computation
  
Due to [1].

Generate random $`r`$ together with shares of $`[r] \mod p`$ and bitwise shares of $`r`$. Compute and reveal $`c = x+r`$.

We have either $`\textrm{LSB}(x)=\textrm{LSB}(c)\ \textrm{XOR LSB}(r)` if no carry occurs in the addition $`c=x+r`$, or $`\textrm{LSB}(x) = 1 - 
\textrm{LSB}(c)\textrm{ XOR LSB}(r)` otherwise.

The parties compute $`[\textrm{LSB}(x)]`$ as
$`[\textrm{LSB}(x)]= {[c <_B r] \textrm{ AND } (1 − (\textrm{LSB}(c) \textrm{ XOR LSB}(r))} \textrm{ XOR } {(1 − [c <_B r]) \textrm{ AND } (\textrm{LSB}(c) \textrm{ XOR LSB}(r))}`$.

Cost: to reveal $`c`$: $`O(\log p)`$ communication

Cost of remaining computation: one comparison + 2 ANDs

Total cost: $`4*m*ell + o(m*ell)`$ communication and 11 rounds.
  

#### Bitwise comparison (one-sided)

Due to [1] with modifications.


Given two bitwise sharings $`[a]_B`$ and $`[b]_B`$, the parties can compute $`[a < b]_p`$ without revealing $`(a < b)`$ itself.

Assume WLOG that $`b`$ is known and $`a`$ is shared. If $`a`$ is known and $`b`$ is shared, scalar multiplying by $`-1`$ reverses the inequality.

Choose some prime $`s`$ with $`s > log p`$.

For $`0 ≤ i ≤ \ell − 1`$, the parties compute $`[c_i] = [a_i\ \textrm{XOR}\ b_i]`$ locally and then compute $`[c_i]_s`$ and then compute $`[d_i]_s = \textrm{OR}_{j=i}^{\ell−1} [c_j]_s`$ by using Prefix-Or, and set $`[e_i]_s = [d_i − d_{i+1}]_s`$ where $`[e_{\ell}−1]_s = [d_{\ell}−1]_s`$. The parties compute $`[a < b]_s = \sum_{i=0}^{\ell−1} ([e_i]_s × [b_i]_s)`$ in parallel.

We then convert from $`[a<b]_s`$ to $`[a<b]_{\textrm{XOR}}`$ with the `{0,1}_s to XOR` protocol below.

Cost: mod 2 to mod s conversion of $`[c_i]`$ + 1 Prefix-Or + 1 {0,1}_s to XOR  = $`3*m*ell + o(m*ell)`$ communication with 9 rounds.

#### Prefix-Or

Due to [1].

Input: $`[a_i]_s`$, shares $`\mod s`$, for $`i = 1..\ell`$.

Output: $`[b_i]_s`$, where $`b_i = \textrm{OR}_{j=i}^{\ell} a_j`$.

Write $`\ell = \lambda^2`$, and re-index so $`a_{i,j} = a_{\lambda * (i-1)+j}`$, for $`i,j=1..\lambda`$.

For each $`i`$ in parallel, compute $`[x_i] = \textrm{OR}_j [a_{i,j}]`$ with unbounded fan-in or.

For each $`i`$ in parallel, compute $`[y_i] = \textrm{OR}_{k=1}^{i} [x_k]`$ with unbounded fan-in or.

NOTE: These two rounds can be combined, i.e. we can compute only the $`[y_i]'s`$ without the $`[x_i]`$'s, since we incur no penalty for the number of elements we input to `unbounded fan-in or`.

Set $`y_0=0`$ and set $`[z_i]=[y_i]-[y_{i-1}]`$.

Set $`[w_j] = \sum_{i=1}^{\lambda} [z_i]\cdot[a_{i,j}]`$.

Compute $`[v_i] = \textrm{OR}_{k=1}^{i} [w_k]`$.

Set $`[s_i] = [y_i] - [z_i]`$.

Set $`[b_{i,j}] = [z_i]\cdot[v_j] + [s_i]`$.

Cost: 2 rounds of fan-in OR + 2 rounds of multiplication:

$`2m\lambda\log s + 2m\ell = 2m\ell+o(m\ell)`$ communication with 6 rounds.


#### Unbounded Fan-In Or

Due to [2], an improvement on [1].

To compute $`[\textrm{OR}_{i=1}^{\ell}(x_i)]`$, we require `mod s` shares of each $`x_i`$, for some prime $`s > \ell+1`$.

We choose a polynomial $`f(x)`$ of degree $`\ell`$ with $`f(1)=0`$ and $`f(i)=1`$ for $`i=1,2,..ell`$ (publicly, with lagrange interpolation).

Parties compute $`[A] = 1 + \sum[x_i]`$ locally. Then parties compute powers of $`A`$ with shared randomness. We generate a random $`r`$ and pre-compute shares $`[r], [r^2], \dots [r^\ell], [r^{-1}]`$. We then compute and reveal $`[A]\cdot[r^{-1}]`$.

Now we can compute $`[A^k] = (Ar^{-1})^k \cdot [r^k]`$ for free, so shares of $`[f(A)]`$ can be computed locally.

Cost: 1 multiplication, for $`m \log s`$ bits of communication with `2` rounds.


[1]: https://www.iacr.org/archive/pkc2007/44500343/44500343.pdf
[2]: http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.917.6433&rep=rep1&type=pdf
[3]: https://eprint.iacr.org/2019/773.pdf