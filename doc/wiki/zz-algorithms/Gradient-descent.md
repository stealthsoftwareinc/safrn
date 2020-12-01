**NOT TO BE IMPLEMENTED IN SAFRN1**

# Gradient Descent
Gradient Descent[1] is an iterative optimization technique to find local minimum, which is proportional to negative gradient[2] of the function. If a function  $`F(x)`$ is defined and differentiable in neighborhood of point a, then  $`F(x)`$ decreases fastest if one goes from point a  in the direction of the negative gradient(tangential vector).



# Inputs and outputs
- $`F(x)`$  (in: _function_) Function  $`F(x)`$ whose local minima needs to be calculated

# Non-Secure Implementation
In the three-dimensional coordinate system, gradient is calculated  for function $`F(x)`$  as[2]
```math
  \nabla f = \frac{\partial f}{\partial x}i + \frac{\partial f}{\partial y}j + \frac{\partial f}{\partial z}k
```
Gradient Descent is calculated as[1]
```math
    x_{n+1} =  x_{n}-\gamma_n  \nabla f(x)
```
 where $`\gamma`$ is step size

# References
https://en.wikipedia.org/wiki/Gradient_descent
https://en.wikipedia.org/wiki/Gradient
