# Division algorithm
  
Given arithmetic shares $`[x]_p`$ and $`[y]_p`$ over some prime $`p`$, we desire to compute shares of the quotient $`[x/y]_p`$, i.e. we compute $`[q]_p`$ such that $`x = qy + r`$ for some $`r`$ satisfying $`0 \leq r < y`$. Let $`\ell`$ be the smallest integer satisfying $`2^{\ell} > p`$.

## The protocol

1. For $`i=0..\ell-1`$, compute $`[z_i] = 2^i[y]`$ (locally). By convention, set $`z_{-1} := 0`$.
1. For $`i=0..\ell-1`$, compute shares of the comparison $`[a_i] = (z_i < z_{i-1})`$, where the comparison is evaluated on the mod $`p`$ residues. Note that $`a_0 := 0`$ always, and requires no computation.
2. Using prefix-or, compute $`[b_i] = 1 - (OR_{j \leq i} a_i)`$, so that $`b_i = 1`$ if and only if $`2^i y < p`$. Compute $`[b_i]`$ for $`i=0..ell-1`$.
3. Set $`[d_i] = [z_{\ell-i}] \cdot [b_{\ell-i}]`$, for $`i=1..\ell`$.
3. Set $`[w_0] := [x]`$, $`c_0 := 0`$, and $`d_0 := 0`$.
4. For $`i = 1..\ell`$:
  - Set $`[w_i] = [w_{i-1}] - [c_{i-1}] \cdot [d_{i-1}]`$
  - Set $`[c_i] = w_i >? w_i - b_{\ell-i} 2^{\ell-i} y`$
5. Set $`[q] = \sum_{i=1}^{\ell} [c_i] 2^{\ell-i}`$.