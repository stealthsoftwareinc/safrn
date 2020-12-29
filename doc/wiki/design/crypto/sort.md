# Secure sorting

[[_TOC_]]

## Introduction

This document is an extension of the [theory design document](/doc/wiki/design/theory.md), with full details on the implementation of the four secure sorting protocol variants.

There are two paradigms for a secure sort. For shared inputs, we use a SISO sort that consists of a secure shuffle followed by quicksort. For a PISO sort, we can either share the private inputs and use the same SISO sort, or use a PISO secure merge that is more elaborate but asymptotically more efficient.  

## Secure block sort

When the payload is much larger than the keys (e.g. when the payload represents a large block of entries), it can be more efficient to sort via a block sort.

Each party holds shares of a vector $`(k_i,p_i)`$ of keys and payloads. With correlated randomness, add random tags $`r_i \in \{0,1\}^{\lambda}`$ to each pair, so that all parties now hold two lists:  
  
$`X=([k_i],[r_i])`$ and $`Y=([p_i],[r_i])`$.

The secure block protocol now works as follows:

- Run `secure shuffle` on $`Y`$.
- Run `secure shuffle` on $`X`$.
- Run `oblivious quicksort` on $`X`$.
- Open all $`r_i`$'s and send shares of payloads from $`Y`$ to keys from $`X`$.

## Secure Shuffle    
    
### Via a Waksman network  
  
A waksman network has `2 \log n` rounds of `n/2` swap operations, and can generate an arbitrary permutation on `n` elements, although choosing the swap operations uniformly at random does *not* give a uniformly random permutation.  
  
Instead, the randomness dealer generates a uniformly random permutation, derives the swap bits from the permutation (using a recursive algorithm based on the recursive construction of the Waksman network), and then shares the swap bits as XOR shares and over all primes used as moduli for the encrypted list.
    
### Via rerandomizable encryption (Deprecated)  

At a high level, a secure shuffle works by encrypting all data under some rerandomizable encryption scheme, shuffling locally, and passing to the next player.  
  
There are two variants here based on whether the data is shared first (multi-key) or the private key is shared (threshold key). The current implementation plan is to use the multi-key shuffle for 2PC Shuffle, and the threshold key shuffle for MPC shuffle.

We also need some choice of rerandomizable encryption scheme. The current plan is to use lattice-based encryption, specifically the BFVrns scheme in PALISADE.
  
#### Multi-key shuffle  
  
For $`m`$ parties, each with a list of length $`n`$.

- All parties generate their secret-key-public-key pair $`(sk_i,pk_i)`$, and publishes $`pk_i`$.
- Each party generates $`m`$ shares of each of their elements, and encrypts the $`i`$th share with $`pk_i`$. 
- Each party sends their encrypted shares to $`P_1`$, who know holds a $`mn \times m`$ table of encrypted shares, with the $`i`$th column encrypted under $`pk_i`$
- For $`i=1\dots n`$:
  - For each row of the table, $`P_i`$ generates a sharing of $`0`$, encrypts the $`j`$th share with $`pk_j`$, and adds to the table via additive HE
  - $`P_i`$ shuffles the list and sends to $`P_{i+1}`$
- $`P_1`$ sends column $`i`$ to player $`i`$
- All parties decrypt their column with their secret key and now hold a sharing sharing of the plaintext

#### Threshold key shuffle

For $`m`$ parties, each with a list of length $`n`$.

- All parties generate private shares of a secret key, and then compute and reveal the public key. 
- All parties encrypt their data and send to $`P_1`$. 
- For $`i=1\dots n`$:
  - $`P_i` shuffles, re-encrypts with the public key, and sends to `P_{i+1}`$.
- All parties run a multiparty decryption algorithm to convert the encrypted value to shares of the plaintext.

#### ElGamal threshold encryption variation

**Note: warm-up / deprecated**

This is a detailed write-up of how a threshold key system works under El-Gamal. I'm including it partly so we have something to fall back on if LWE doesn't work, and partly as a specific example to help explain how the threshold scheme will work in LWE. 

Whether we need to carefully work out and write up the LWE solution in the same level of detail will depend on how much we can directly use PALISADE libraries, in particular whether we need to write our own `decrypt-to-shares` protocol.

Choose some group $`G`$ for which DDH holds (e.g. a Schnorr group). Then:

Public key generation (SIPO):
 
- Parties choose $`g`$, a generator of $`G`$ in the clear
- Parties choose shares $`[x]`$ of the shared secret key $`x`$
- Parties compute $`g^{[x]}`$ locally (i.e. a multiplicative share of $`g^x`$)
- Each party $`P_i`$ secret shares (additively) their share $`g^{[x]_i}`$ with all parties
- Parties compute (additive) shares of $`g^x`$ through m multiplications
- Parties open $`h=g^x`$
  
Local encryption:
    
- Each party, for each element $`s`$ in their array, chooses a random $`y`$, and computes $`(g^y,sh^y)`$    
    
Re-randomization:

  - For each $`i`$, choose random $`y`$, and set $`A[i] = (g^y c_1,h^y c_2)`$

Decrypt-to-shares of $`(c_1,c_2)`$: (PISO)

- Each party computes $`c_1^{(q-1)-[x]}`$ locally
- Parties compute shares of $`c_1^{-x}`$ through m multiplications
- Parties compute shares $`[s]`$ of $`s=c_1^{-x}c_2`$ through scalar multiplication


## Quicksort

Once the array is shuffled, we implement quicksort, keeping the values private but leaking the results of all comparisons.

We do not wish to leak when keys are equal, so we first tag all elements with the shares of a psuedo-random permutation (the PRP can be computed by the randomness dealer with a Fisher-Yates shuffle and shared and distributed in advance).

We use the standard implementation with the Hoare partition scheme, see e.g. [Wikipedia](https://en.wikipedia.org/wiki/Quicksort#Hoare_partition_scheme). This requires $`O(\log n)`$ rounds of communication, since the recursive depth of quicksort is $`O(\log n)`$. 

In the $`i`$th round, we have a list divided into $`2^{i-1}`$ subintervals. We compute and reveal the comparison of each list element to the pivot of the interval to which it belongs. Each party then locally uses these comparisons to swap their shares and determine the pivots and subintervals for the next level of recursion.

## Secure Merge

NOTE: Secure merge has not been implemented in SAFRN 1.0.

The secure merge protocol is a great deal more involved, and uses the secure shuffle and SISO secure sort as subprotocols, as well as the additional protocols `sparse compact` and stable compact`.
   
     
Here are the step by step details. Sub-algorithms below.

Each party holds privately a list of length $`n`$ consisting of a key and payload $`(k,p)`$.

1. Locally sort each list by key with `quicksort`.
2. Divide into blocks of size $`O(\log^2 n)`$. The payload of each block is the entire block, plus an array of $`m`$ flags indicating which party it comes from. The key is the key of the last element of the block.  
3. Each party shares their blocks with the other parties.
3. Run `secure sort` on these blocks. Call the output $`A`$.
4. Flag each block that is preceded by $`\Omega(m \log^4 n)`$ blocks from the other party. These are the "stray blocks" (of *potentially* stray elements). This is done with the [Map-Reduce paradigm](/doc/wiki/design/crypto/mapreduce.md). We need $`mn/\log^2 n`$ rounds of batched fan-in AND, one for each possible party source of each block.
5. Create a new sparse array of blocks which holds the flagged blocks in the same positions as before, and zero blocks everywhere else. Set the key to the zero blocks to be $`\infty`$.
6. Sort the sparse array with `MPC secure sort` on blocks. Extract the leading $`\frac{n}{\log^6 n}`$ blocks, and place the elements into an array $`B`$ with $`\frac{n}{\log^4 n}`$ elements.
7. Add to $`B`$ the $`\frac{n}{\log^2 n}`$ pivots of $`A`$.
8. Run `secure sort` on the *elements* of $`B`$ (not the blocks).
9. Tag each stray element of $`B`$ with the pivot it corresponds to, via Map-Reduce: Tag each element with a secret shared indicator function $`\iota_{i}`$ that equals $`1`$ when the $`i`$th element is a pivot, and $`0`$ otherwise. Then set the (shared) tag of the $`i`$th element of $`B`$ to:
```math
[t_i] = n/\log^2 n + 1 - \sum_{j=i}^{|B|} [\iota_j]
```
10. Run `sparse compact` so that $`B`$ contains only the strays.
11. Find the secret-shared count $`s_i`$ of the number of strays for the $`i`$th pivot, for $`i=1..n/\log^2 n`$. We do this via Map-Reduce:  
  - Using the above notation, we tag each element with a tag $`[c_i] = [t_i] \times [\iota_i]`$. Note that each pivot is now tagged with its position among the pivots, and each stray is tagged with zero.
  - Assign to each element a secret sharing $`[d_i]`$ of its (public) index in $`B`$.
  - Securely shuffle $`B`$.
  - Reveal the elements $`c_i`$ and set $`[s_i]=[d_i]-[d_{i-1}]`$, so that $`[s_i]`$ is the desired sharing of the number of strays to be assigned to the $`i`$th pivot.   
11. Let $`C`$ be a list of $`n/\log^2 n`$ blocks, where each block has length $`2(\log^2 n -1)`$ and two kinds of dummy elements. Set the $`i`$th block to hold $`s_i`$ elements tagged with $`-1`$ and the remaining elements tagged with $`0`$ and the $`i`$th pivot.
12. Use `sparse compact` to compact out and discard the `-1` elements.
13. Take $`C=C \cup B`$, and note that $`C`$ has exactly $`2(\log^2 n - 1)`$ elements per pivot.
14. Shuffle $`C`$ with `secure shuffle`.
15. Open the pivot tags of $`C`$ and add the elements to the corresponding blocks of of $`A`$.
16. Sort each block with `secure sort`.
17. Run `stable compact` to compact out and discard the `0` tagged elements.  



### Sparse compact

Each party holds shares of a vector $`V`$ of length $`n`$, of which at most $`t`$ elements are non-dummies, for $`t < n/\log^2 n`$.

1. Break $`V`$ into blocks of size $`\log^2 n`$.
2. Identify counts $`c_i`$ of non-dummy elements of the $`i`$th block via Map-Reduce.
  - Compute shares of an indicator function $`\iota_{i,j}`$ for whether an element is dummy, where $`\iota_{i,j}=1`$ precisely when the $`j`$th element of the $`i`$th block is non-dummy, with an equality test in the `MapOp` step.
  - Compute $`[c_i] = \sum_{j=1}^{\log^2 n} [\iota_{i,j}]`$ in the `ReduceOp` step.
3. Tag counts and blocks with the same random tag $`r_i`$.
4. Run `secure sort` on counts with key $`c_i`$ and payload $`r_i`$ (so that nonzero counts are at the front of the list)
5. Open the tags $`r_i`$ on the count list and on the blocks, and move blocks to the position indicated by the tags.
6. Run `secure sort` on the elements of the leading $`t`$ blocks.

### Stable compact

Each party holds shares of a vector $`V`$ of length $`n`$, of which $`t`$ elements are dummies, for $`t`$ publicly known.

1. Tag each element with its true location, or 0 for dummies via Map-Reduce (to do batched computation):
  - Compute shares of an indicator function $`\iota_i`$ where $`\iota_i=1`$ when the $`i`$th element of $`V`$ is not a dummy and zero otherwise.
  - Compute $`[c_i] = \sum_{j=1}^{i} [\iota_j]`$ locally (in the `ReduceOp` step).
  - Compute $`[d_i] = [c_i] \times [\iota_i]`$ with MPC, for each $`i`$ in parallel, via batched multiplication. This is the tag representing the true location.
2. Run `2PC secure shuffle` on elements with their tags.
3. Open tags and move elements to the correct location.  