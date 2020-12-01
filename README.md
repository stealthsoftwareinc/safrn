# Secure Analytics For Reticent Non-consolidated databases (SAFRN)

SAFRN is a Secure MultiParty Computation engine for statistical analysis.
Multiple parties, each holding fragments of a dataset, may use SAFRN to join their fragments and compute statistical analysis.
At the moment SAFRN is targeting the following analysis methods.

 - Linear Regression (Ridge?)
   - per-coefficient standard error
   - significance based on T and F tests
 - Low-order Moments (count, mean, variance)

In this repository you will find:

 - SAFRN 1 Core (``core/``): The MPC statistical engine.
   - This is under active development and support.
 - Configuration Dashboard (``dashboard/``): A collaborative tool for conceiving and configuring studies.
   - This is a prototype which is not under active development.
   - We have strong interest in resuming development at a later date.
 - Analyst Console (``analyst-ui/react/``): A Web UI for building SAFRN Queries and displaying results.
   - This is a prototype which is not under active development.
   - We have interest in resuming development at a later date, however this codebase may or may not be reused.
 - SAFRN 0 Prototype (``safrn0/``): An earlier iteration of the MPC statistical engine.
   - This is no longer under active development.
   - When the SAFRN 1 Core exceeds feature parity with SAFRN 0, we will remove this.
