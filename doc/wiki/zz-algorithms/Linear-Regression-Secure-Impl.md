# Linear Regression Secure Implementation

[This paper][1] describes a solution for secure ridge regression,
based on Shamir secret sharing over finite fields. Here is a link to a
[code snippet][2]. We adapt the algorithm to the (semihonest) dishonest majority
setting. See [#71][3] for more details.

We describe the details of secure implementation for ridge regression,
treating OLS as a special case. When we treat OLS, the parameter
$`\lambda`$ cannot be tuned, and so we warn the output recipient if
the matrix $`X^TX`$ is singular or nearly singular.

We use fixed-point, rather than floating point arithmetic. This allows
us to work over the integers, by multiplying all matrices by
$`2^{\alpha}`$, for some constant $`\alpha`$, and rounding to the
nearest integer before doing any MPC. Our initial shares are over
$`\mathbb{Z}_p`$, for some relatively small prime $`p`$ (details
below).

### Algorithm overview

The algorithm can be divided up into two steps. First, we compute
$`X^TX + \lambda I`$ and $`X^TY`$. Then we have an equation of the
form $`Ax=b`$ to solve.

When there is one vertical, the first step requires only local
computation. When we write $`X`$ as a column of blocks $`X_i`$, we
have $`X^TX = \sum_{i} X_i^T X_i`$ and $`X^TY = \sum_i X_i^T Y`$.

When there are multiple verticals, we have to compute products of the
form $`X_{ij}^T X_{ik}`$. This is a secure dot product, which we can
execute through a PSI-SUM over pairwise products.

Second, we must solve an equation of the form $`Ax=b`$, with $`A`$ a
$`d\times d`$ and $`b`$ a $`d \times 1`$ column vector. We transform
our shares over $`\mathbb{Z}_p`$ to shares over $`\mathbb{Z}_q`$, for
some much larger prime $`q`$. We generate shares of a random matrix
$`[R]=[L][U]`$ for lower and upper triangular matrices, respectively,
with $`1`$'s on $`L`$'s diagonal. We compute shares of the determinant
of $`R^{-1}`$ (mod $`q`$) by multiplying together the diagonal entries
of $`U`$ and taking the inverse.

We then compute $`[RA]=[R][A]`$ and open the result. From here we can
compute $`[x] = (RA)^{-1} [Rb]`$ and $`[\det A] = \det RA
\det(R^{-1})`$ locally and return $`([(Adj\ A) x], [\det(A)])`$. We
then use MPC to compute and open $`(\det A)^{-1} (Adj\ A) x)`$ (over
$`\mathbb{Q}`$, not over $`\mathbb{Z}_q`$).

### Complexity

We go through the above algorithm in detail, tracking the
communication complexity for ridge regression on $`(X,Y)`$ with
$`m=m_1+m_2`$ parties ($`m_1`$ from first vertical, $`m_2`$ from
second), with $`X`$ a $`n \times d`$ matrix, $`Y`$ a $`n \times 1`$
column. Individual horizontal record size bounded in size by $`U`$.

Assume WLOG that $`(X,Y)`$ are $`\doubles`$ in $`[0,1]`$. Choose some
parameter $`\alpha`$ for bits of rounding (see Step 2) and $`\kappa`$
for a security parameter (e.g. 40, not $`2^{-40}`$).

Step 1:

Choose prime $`p`$ with $`2\alpha + log n + \kappa`$ bits and prime $`q`$
with $`d(\frac{log d}{2} + 2\alpha+ \log n)`$ bits.

Step 2:

Convert $`X`$ and $`Y`$ to integers by multiplying by $`2^{\alpha}`$
and rounding (add $`0.5`$ and truncate).

Step 3:

Compute $`X^T X  + \lambda I`$ and $`X^TY`$:

$`O(m_1m_2 d^2 (\alpha + \log n + \kappa) U log U)`$ communication
complexity from dot product via pairwise PSI + $`O(m d^2(\alpha + \log
n + \kappa))`$ communication complexity to share additive shares of
pairwise results (all shares $`\mod p`$)

Step 4:

Convert shares $`\mod p`$ to shares $`\mod q`$

Step 5:

$`m`$-party MPC protocol to solve $`Ax=b \mod q`$:

Compute $`[R] = [L][U]`$ together with $`[det R^{-1}]`$

Compute $`[RA]=[R][A]`$ and open $`RA`$ (with $`d^3`$ multiplications)

(Implicitly) Replace $`Ax=b`$ with $`RAx=Rb`$

Solve to obtain $`[A^{-1}b]`$ (No communication required)

Compute $`[det A] = det RA [det R^{-1}]`$ (No communication required)

Compute $`[(adj A)b] = [det A][A^{-1} b]`$

Return $`([(adj A)b], [det A])`$

Communication complexity: $`O(d^3)`$ operations $`\mod q`$ to compute
the random matrix product -> $`O(m d^4 (\frac{log d}{2} + 2\alpha+
\log n))`$.

NOTE: We could compute $`[RA]=[R][A]`$ using some partial homomorphic
encryption that does secure dot product in $`O(m)`$ time so
$`O(md^2)`$ total communication for the entire matrix
multiplication. But the overhead here is probably not worth the factor
of $`d`$ we save.

#### Choice of constants

For default values of constants, we propose $`\alpha = 64`$, $`\kappa =
40`$, and $`\lambda=1`$. As discussed in [#71][4], taking $`\lambda=1`$
gives relative error of at most $`2^{-\alpha}d`$.

[1]: https://eprint.iacr.org/2019/773
[2]: https://github.com/lschoe/mpyc/blob/master/demos/ridgeregression.py
[3]: https://gitlab.stealthsoftwareinc.com/stealth/safrn/-/issues/71