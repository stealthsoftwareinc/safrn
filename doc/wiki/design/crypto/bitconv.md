

## Bit conversion protocols

### Introduction
  
The following protocols are used for PSO, as well as in our ridge regression protocol. This is also described in [#72](https://gitlab.stealthsoftwareinc.com/stealth/safrn/-/issues/72).
  
The cost of all protocols are described for a computation done by $`m`$ parties with a prime $`p`$ that is $`\ell`$ bits long.
  
See also the related [comparison protocols](compare).  
  
### Protocols

#### mod 2 to mod p

[This is due to @steve in CARMA]

Begin with XOR shares $`[x_j]`$ of the bits $`x_j`$ of some integer $`x`$ satisfying $`0 \leq x < 2^c < p`$.

Correlated Randomness: For $`j=0..c`$, let $`z_j`$ be a "flipper" bit.  Let $`y_{j0} = 0`$ and $`y_{j1} = 2^j`$ if $`z_j = 0`$, otherwise flip them: $`y_{j0}=2^j`$ , $`y_{j1} = 0`$ if $`z_j=1`$.  Share $`[z_j]`$ over XOR sharing and $`[y_{j0}]`$, $`[y_{j1}] \mod p`$.

Computation: Each party computes $`[x_j]`$ XOR $`[z_j]`$ locally, then open this value (e.g. send to 1 party, that 1 party broadcasts results).  If we call $`b_j`$ the opened value, then each party locally adds up $`[y_{j{b_j}}]`$ for all $`j`$.

Complexity: Open $`c`$ bit values, for total of $`O(cm)`$ communication and 1 round.

(This corresponds to $`O(cm^2n)`$ communication and 1 round for doing mod-2 to mod-p conversion for all elements in an m-party array)


#### $`\{0,1\}_p`$ shares to XOR shares

Given shares $`[x]_p`$ of an integer known to be $`0`$ or $`1`$, we can convert this to XOR shares as follows:  
  
Generate shares of a random tuple $`([r_1]_p,[r_2]_p,[r_3]_{XOR})`$ equal to either $`(1,0,0)`$ or $`(-1,1,1)`$.  
  
Compute and reveal $`t=[r_1]\cdot[x]+[r_2]`$. Since $`t`$ is public, we can convert it to a bit. Then $`t\ \textrm{XOR}\ [r_3]`$ is the desired secret sharing.

COST: One multiplication mod p: i.e. $`O(m \log p)`$ communication and 2 rounds. 

#### mod p to mod q (with $`p \ll q`$)

We require only $`q > mp`$, where $`m`$ is the number of players, and $`p`$ is stored with $`\ell`$ bits.

Denote by $`x_i`$ the $`i`$th player's share. Then over $`\mathbb{Z}_q`$ we have  $`\sum x_i = kp + x`$, for some integer $`k`$. 
  
Through shared randomness, distribute shares of $`r,\tilde{x} \mod q`$ with $`r = \tilde{k}p + \tilde{x}`$, and also bitwise shares of $`\tilde{x}`$ and $`\textrm{LSB}(r)`$. 

Compute $`\tilde{q} = q \mod p`$ (in the clear)
 
Compute and reveal $`\sum x_i + r \mod q`$, then compute $`t = \sum x_i + r \mod p`$ (reduce in the clear).

We then have:

$`[x]_q = t - [\tilde{x}]_q + p[(\tilde{x}>t)] + \tilde{q}[(\sum x_i + r \geq q)] - p([(t+\tilde{q}-\tilde{x}) > p]\ \textrm{AND}\ [(\sum x_i + r \geq q)]) - p(([(t+\tilde{q}-\tilde{x}) > 0]\ \textrm{AND}\ [(\tilde{x}>t)]\ \textrm{AND}\ [(\sum x_i + r \geq q)]))`$

(this expression simplifies to $`t-[\tilde{x}]_q`$ if no carries occur `mod p` or `mod q`, and all other terms account for carrying)

Cost: $`(\sum x_i + r >= q) = \textrm{XOR}_{i=1^n}(\textrm{LSB}(x_i))\ \textrm{XOR}\ \textrm{LSB}(r)\ \textrm{XOR}\ \textrm{LSB}(\sum x_i + r)`$ i.e. this is a local computation!

$`[(\tilde{x}>t)]`$ and $`[(t+\tilde{q}-\tilde{x} > p)]`$ are bit-wise comparisons with one side fixed. In addition we have three ANDs to compute (on bit shares) and five conversions from an XOR sharing to a `mod q` sharing of $`[0]_q`$ or $`[1]_q`$. Each of these operations require a single bit of communication, for a total cost of:

$`6m\ell + o(m\ell)`$ communication with 10 rounds.

[1]: https://www.iacr.org/archive/pkc2007/44500343/44500343.pdf