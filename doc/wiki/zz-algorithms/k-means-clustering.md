**NOT TO BE IMPLEMENTED IN SAFRN1**

# k-means clustering
k-means clustering partitions a given data set into k clusters in which each observation belongs to the cluster with the nearest mean.

# Inputs and outputs

* **X**(in: *vector of reals*) data set of real numbers
* **k**(in: *integer number*) number of clusters
* **Y**(out: *k number of vectors*) output is k number of vectors where each vector has the elements of a cluster


# Non-Secure Implementation
Initialization step: Randomly chose k-observations as the centroids.

Assignment step: Assign each observation to the cluster whose mean has the least squared distance (also called nearest mean).

Update step: Calculate the mean of the 'nearest means'

Algorithm has converged when the assignment no longer changes the mean (TOO: needs more clarification on how to reassign)

# References
https://en.wikipedia.org/wiki/K-means_clustering
