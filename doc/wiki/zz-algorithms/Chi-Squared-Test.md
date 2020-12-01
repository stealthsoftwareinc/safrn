**NOT TO BE IMPLEMENTED IN SAFRN1**


# Chi-Squared Test
Chi-Squared test is used to test the relationship between frequencies of category variables

# Inputs and outputs
Observed(in: *integer value*) represents observed frequency
Expected(in: *integer value*) represents expected frequency
$`\chi`$(out: *real value*) represents Chi-Squared value

# Non-Secure Implementation
Chi-Squared test is calculated as follows[1]
```math
     \chi = \frac{(Observed-Expected)^2}{Expected}
```

Above value is used to look up in the table in determining if hypothesis can be rejected.


# References
https://en.wikipedia.org/wiki/Chi-squared_test
