# Secure Analytics For Reticent Non-consolidated databases (SAFRN)

SAFRN is a Secure MultiParty Computation engine for statistical
analysis. Multiple parties, each holding fragments of a dataset, may
use SAFRN to join their fragments and compute statistical analysis.
At the moment SAFRN is targeting the following analysis methods:

- Linear Regression:
  - user-configurable intercept term included or excluded.
  - per-coefficient standard error.
  - significance based on T and F tests.
- Low-order Moments (count, mean, variance).

In this repository you will find:

- SAFRN 1 Core (``core/``): The MPC statistical engine.
  - This is under active development and support.
- Documention (``doc/``): A wiki detailing the computations supported
  by SAFRN and their implementation.
- SAFRN 0 Prototype (``safrn0/``): An earlier iteration of the MPC
  statistical engine.
  - This is no longer under active development. When the SAFRN 1 Core
    exceeds feature parity with SAFRN 0, we will remove this.
- Helper Scripts (``src/``): Directory containing various shell
  scripts used to simplify development and deployment of SAFRN.
