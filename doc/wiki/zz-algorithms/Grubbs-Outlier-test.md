# Grubbs Outlier test  
This test is used to detect and remove the outliers from a data set; it can be used to remove outliers on either sides of the data set or to remove more than one outlier; hence it is more flexible than other outlier tests[1].

# Inputs and outputs  
* **Y**(in: *vector of reals*) data set of real numbers
* **G**(out: *real number*) Grubbs score

# Non-Secure Implementation 
Arrange the input data in ascending order and apply the following formula to detect if the extreme element (either lowest or highest) is the outlier
```math
    G = \frac{absolute(mean- extremeElement)}{standardDeviation}
              
``` 

Above G score is compared with the critical value from the table, to determine if the extreme element is outlier or not.

To detect more outlier in the data set, remove the current outlier from the series and repeat the above formula with the new series. 

# References
https://en.wikipedia.org/wiki/Grubbs%27s_test_for_outliers
