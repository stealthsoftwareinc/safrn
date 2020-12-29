# The Map-Reduce Paradigm

[[_TOC_]]

## Introduction

This document describes the map-reduce paradigm we used in our original design. This arises both in [PSO](/doc/wiki/design/theory.md#Private-Set-Operations) and in some of the modular conversion and comparison [protocols](/doc/wiki/design/crypto/bitconv.md).  
  
The map-reduce paradigm has been deprecated for the simpler `Batch` fronctocol, which has a vector of children fronctocols, each of which follows the same sequence of actions (handleReceive, handleComplete, etc.) in the course of their execution. However, this document can be read as a description of abstract design principles motivating the engineering. Whenever document refers to a `Reduce`, this has been implemented as code within the calling fronctocol, while references to `Map` are implemented as a call to `Batch` from the calling fronctocol.

## Map and Reduce

The `Map` fronctocol each act on a sequence of shared data by batching together a collection of sub-fronctocols, `MapOp` and `ReduceOp` respectively. The `Map` or `Reduce` fronctocol generates one `MapOp` or `ReduceOp` object *per element* of the sequence (or sometimes, per $`k`$-tuple of adjacent elements). We do not *require* that `Map` and `Reduce` be coupled together. In some cases we might need `Map` only, or `Map+Reduce+Reduce+Map+Reduce`.

### Map

The `Map` fronctocol begins with some secret shared list $`L`$ of length $`n`$, and desires to perform some computation $`f`$ on all blocks of $`k`$ consecutive elements simultaneously (requiring $`O(kn)`$ communication and $`O(1)`$ rounds)

Though this will probably be batched for efficiency reasons, abstractly we should imagine $`n-k+1`$ separate instances of MPC, with the $`i`$th instance carrying elements $`L[i],L[i+1],\dots, L[i+k-1]`$.   
  
I.e. the input $`f`$ will be some base object from which we could generate $`n-k+1`$ fronctocols. The fronctocol $`f`$ is assigned the unchanged peer-set of `Map`.

The function $`f`$ will be input as a `MapOp` fronctocol. The inputs to `Map` are therefore $`L,n,k,f`$, and the output is some list of length $`n-k+1`$ of objects matching the return type of $`f`$. 


### Reduce

  The `Reduce` fronctocol is a protocol on a secret shared list which can be executed without communication, such as:
  
  - Sum the results from each `MapOp`
  - Replace an array $`(a_1,\dots,a_n)`$ with $`(a_1-a_2,\dots,a_{n-1}-a_n)`$.
  
Note that each party is operating on the entirety of (their share of) the list, so this differs from MapReduce in the insecure database setting where each party is given a subset of the list to operate on.

### Applications  
  
#### Batched multiplication

As a simple example, multiplication (or any MPC) can be batched via a single `Map` step as follows. Given two lists $`L_1 = (a_1,\dots,a_n)`$ and $`L_2 = (b_1,\dots,b_n)`$, we form the list $`L = ((a_1,b_1),\dots,(a_n,b_n))`$, and call the batched multiplication `Map` fronctocol with $`k=1`$, $`n`$ and $`L`$ as above, and $`f`$ the multiplication fronctocol.

#### Batched Unbounded Fan-in Or

See [here](/doc/wiki/design/crypto/compare.md#Unbounded-Fan-In-Or) for the full unbounded fan-in or protocol. The only communication required is to compute and reveal $`[A] \times [r^{-1}]`$, where $`[A]`$ is some sum of local shares, and $`[r^{-1}]`$ is read from pre-generated randomness.

Therefore the `Map` is identical to that in batched multiplication, except the fronctocol $`f`$ now is multiply+reveal.

We now have a `Reduce` step as well, consisting of the additional local scalar multiplications and additions required to compute the desired polynomial, followed by concatenation of shares.

#### Prefix-Or  
  
As an example of composing multiple `Map` and `Reduce` fronctocols together, consider the [prefix-or protocol](/doc/wiki/design/crypto/compare.md#Prefix-Or), which consists of two batched unbounded fan-in ors and two multiplication steps, interleaved with local computations, i.e. addition and scalar multiplication of shares.

This looks like:

1. Begin with shares of a list of length $`n=\ell^2`$, indexed as if it were arranged in a square array.
2. Generate $`[c_k] = \sum_{i=1}^{k} \sum_{j=1}^{\ell} a_{i,j}`$ using a `Reduce` step.
1. Run batched unbounded fan-in or to compute $`[y_k]`$ from $`[c_k]`$ (where $`y_k`$ is the OR of the corresponding $`a_{i,j}`$'s. In an additional `Reduce` step, set $`y_0=0`$ and set $`[z_i]=[y_i]-[y_{i-1}]`$.
2. Run batched multiplication to compute all products $`[z_i][a_{i,j}]`$ and then in a `Reduce` step set $`[w_j] = \sum_{i=1}^{\lambda} [z_i]\cdot[a_{i,j}]`$ and $`[d_i] = \sum_{k=1}^{i} w_k`$.
3. Run batched unbounded fan-in or to compute $`[v_i] = \textrm{OR}_{k=1}^{i} [w_k]`$ from $`[d_i]`$. Then in an additional `Reduce` step, generate a list with $`\ell`$ copies of each $`[v_i]`$.
4. Run batched multiplication to compute all products $`[z_i][v_j]`$. Then set $`[s_i] = [y_i] - [z_i]`$ and $`[b_{i,j}] = [z_i]\cdot[v_j] + [s_i]`$ in a `Reduce` step.

#### PSO compare step

The compare step follows the Map-Reduce paradigm. In the two vertical case, as discussed [here](/doc/wiki/design/theory.md#SQL-join-semantics), we only need to determine how to extract adjacent entries with matching keys from different verticals.

The result can consist of some function on the payload columns (for now, one column or a product of two columns), or it can consist of a masked row which is equal to $`\bot`$ except on included rows, or both.

The `Map` step operates on blocks of size $`k=2`$, with $`L`$ the sorted list and $`n`$ its length. In the Fronctocol $`f`$, we first compute shares of 
```math
t = (\textrm{key1}==\textrm{key2}\ \textrm{AND}\ \textrm{flag1} < \textrm{flag2}),
```
where $`\textrm{flag1}`$ and $`\textrm{flag2}`$ are bit flags representing which vertical each element belongs to. Computing $`t`$ requires an equality test on $`\mod p`$ shares, a comparison gate on a single bit, and an AND gate.

If $`t`$ is true, we compute and return the desired result. This may require a `Reduce` operation, depending on the format desired of the result. Otherwise, we return $`\bot`$.

When all columns in the query belong to one vertical, no communication is necessary, so we do not need a `Map` step. Summing over the desired rows is performed in the `Reduce` step.

