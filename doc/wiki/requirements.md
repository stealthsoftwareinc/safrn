# SAFRN 1 Requirements

This document describes the requirements for SAFRN Version 1 (SAFRN1).
At the moment, SAFRN Version 0 (SAFRN0) is a cryptographic research system for performing statistical operations on decentralized data.
Specifically, SAFRN0 has a data schema in which many groups hold horizontally partitioned (aka, union compatible) datasets of student degrees, genders, GPAs, and other distinguishing features.
Simultaneously, another (single) party holds the average incomes of these individuals, after their graduation.
SAFRN0 enables an analyst to count the number of students matching a distinguishing feature set, or take the average of their incomes.
This is done by performing Private Set Intersection (PSI) against a shared unique identifier (say Social Security Number) to join these datasets, while keeping them secret.
Then SAFRN0 uses MPC to calculate an operation in a way that reveals nothing about each subset, other than the result of the desired operation.

SAFRN0 was born from a proposed ["Student Right to Know Before You Go Act"](https://www.wyden.senate.gov/news/press-releases/wyden-rubio-warner-introduce-student-right-to-know-before-you-go-act-to-empower-students-as-consumers-and-showcase-new-privacy-protecting-technology) under which colleges, tax collection bureaus, and lending institutions would be required to perform MPC to determine statistics such as average income after graduation, or average loan principal after graduation.
Enabling such reporting remains a primary goal for SAFRN1.
In this case, a dominating number of parties would collectively perform MPC.
Additionally, there is potential value in using SAFRN1 for data science research on private, longitudinal datasets, as well as for holding scientists accountable for papers relying on private datasets to draw their conclusions.
In this case, number of parties may not be as dominant as number of records.

The purpose of SAFRN1 is to extend this capability to arbitrary datasets and a broader selection of operations.
Specifically, we would like SAFRN1 to execute queries considering combinations of n-many horizontally partitioned across one or two vertical partitions.
Moreover, SAFRN1 should consider a broader set of capabilities, in order to make it more useful to researchers and data scientists.

Throughout this document, we will make reference to an "Analyst" who would like to make statistical queries on these datasets.
The Analyst is assumed not to own or have access to any of the data upon which SAFRN operates.
However, the result of all queries should be revealed only to the Analyst.

We will also refer to "Data owners", organizations who own, or have custody for, datasets which are used in participation of MPC.
The collective, canonical schema or format of these datasets will be referred to as a Lexicon.

In this document we use the words "shall" "must", "should", "may", their negations, and other words as described in [RFC-2119](https://tools.ietf.org/html/rfc2119).
Briefly, "shall" and "must" indicate an absolute requirement, "should" indicates a strong, yet flexible, requirement, while "may" indicates an optional requirement.

Specific requirements may be referenced using the following scheme.
Each section of requirements has a letter parenthesized at the end of its title.
Requirements are given a hierarchy of enumerated points, with sub-points having greater indentation.
To identify a requirement, join the enumerations each sub-requirement's enumeration around a dot ("."), and then prefix it with the section's letter.
For example "F1.2.3".

## Functional Requirements (F)
 1. SAFRN1 lexicons must support the following representations of data.
    1. SAFRN1 lexicons must support processing of integers.
       1. Integers should should allow minimum and maximum constraints on an otherwise continuous input domain.
       2. Integers may be represented as values in a finite field sufficiently large to perform a given calculation.
       3. Integers may be represented as fixed length binary values, with or without sign.
       4. Choice of integer representation may be varied by the implementer as necessary.
    2. SAFRN1 lexicons must support real numbers calculations.
       - _MPC calculations using real numbers are often time consuming. [HLOW16](https://eprint.iacr.org/2016/319) briefly outlines a simple scheme for conducting inexpensive fixed point calculations, however the denominator is known in  cleartext_.
    3. SAFRN1 lexicons must support conversion of enumerated-string values into categorical a categorical set of integer values on the range of $`[0, 1]`$.
       1. All enumerated-string values must be known a priori by all parties and in clear text.
 2. SAFRN1 shall perform the following statistical computations.
    1. It must calculate frequency (counting records in a dataset).
    2. It must calculate the mean, variance, and higher order moments, up to fourth order.
    4. It must calculate Linear Regression with multiple regressors.
    6. It should calculate the T-Statistic and F-Test for hypothesis testing.
    7. It may calculate maximum, minimum, and median.
    8. It may perform Logistic Regression.
    9. It may perform K-Means Clustering.
 3. SAFRN1 must support lexicons divided amongst multiple parties.
    1. SAFRN1 must operate on the union of N-many disjoint, horizontally partitioned datasets.
       1. SAFRN1 may enforce uniqueness of records by adding additional key values to the query, in cases where it does not meaningfully alter the calculation and successful completion of a calculation necessitates this action.
    2. SAFRN1 must represent n-many vertically partitioned datasets in its public data schema.
    3. SAFRN1 must operate on the intersection (join) of two vertically partitioned datasets.
          - Finding the intersection on n-many datasets is out of scope.
       1. SAFRN1 may have the capability to use more than one column simultaneously as the joining key.
    3. SAFRN1 must operate on a combination of N and M many horizontally partitioned datasets across the intersection of two vertically-partitioned datasets.
       - _For clarity, this means that given datasets $`A`$, $`B`$, $`C`$, and $`D`$, SAFRN1 jointly computes some function on $`(A \cup B) \cap (C \cup D)`$_.

## Security Requirements (S)
 1. SAFRN1 shall assume that all participants act in an honest-but-curious manner.
1. SAFRN1 shall be secure against collusion by `n-1` dataowner parties.
1. SAFRN1 shall verify the identity of peers before commencing a calculation.
 2. The schemas of all datasets shall be known by all parties in the clear.
 3. SAFRN1 shall not reveal participants' private datasets to other participants or observers.
    1. SAFRN1 should not leak any meaningful information to any party which could not be inferred from the result.
    2. SAFRN1 should provide the result only to the Analyst.
 4. SAFRN1 should refuse to reveal the result of a query which is artificially or otherwise constrained to a point where it would reveal information directly from a dataset.
    - _This may may bring up questions of de-anonymization and other active research areas that are out of scope. At this point, I expect we should only consider "trivial" measures of de-anonymization prevention. Perhaps a threshold of records, below which SAFRN1 will refuse to reveal results. However, proper discussion is required. These requirements are in consideration for SAFRN 1.1_
    1. SAFRN1 should refuse to reveal the result of a query in which the constituent records do not meet a certain threshold.
    2. SAFRN1 may refuse to reveal the result of a query in which the result does not meet constraints on the ratio of mean:variance.
    3. SAFRN1 shall not reveal its reason for refusing to reveal the result of a particular query.
       _This should also include revelation by means of timing_.
 5. SAFRN1 should use access control to require that an analyst receive permission from all data-owning parties before querying a dataset.
 6. SAFRN1 servers shall protect their calculations from eavesdroppers.

## Performance Expectations (P)

 > Given that we will be constrained by what is actually possible in terms of performance, I don't think we can put in traditional performance requirements in terms of how long some operation should take, how much memory it should use, etc.
 > Instead, what we should be looking at is the constraints of the systems we will be working with.
 > We may encounter limited memory in comparison to our datasets, network limitations, and dominant factors in O-analysis.
 > For this reason, I will note expectations about SAFRN1's operating environment, instead of concrete requirements.

 1. We expect SAFRN1 to be constrained in memory when compared to the size and quantity of datasets on which it computes.
 2. We do not expect SAFRN1's performance to be dominated by number of participants.
 3. We do expect SAFRN1's performance to be dominated by number of records.
 4. We do not expect SAFRN1's networking environment to be significantly constrained in latency or bandwidth.

## Configuration Requirements (C)
Real world testing of SAFRN0 revealed that a major barrier to usage was the necessary configuration, and the effort it took to complete correctly.
This section details tools to minimize this configuration burden.
It also introduces a new role in SAFRN administration, the Data Steward.
The Data Steward is responsible for balancing an organization's often conflicting goals of cooperating with other organizations to analyze partitioned datasets while ensuring the privacy of its own data.

 1. SAFRN1 may have a configuration dashboard as a public resource.
    1. The dashboard shall coordinate MPC Sessions between multiple organizations.
    2. The dashboard shall enable data stewards to manage a lexicon (1.1).
    3. The dashboard shall deliver a session's network configurations to data stewards (1.0).
    4. The dashboard should deliver a session's network configurations directly to SAFRN servers (1.1).
       1. SAFRN servers must directly receive approval from a data steward before acting on a configuration delivered from the dashboard.
    5. The dashboard should deliver a session's lexicon directly to SAFRN servers (1.1).
 2. Individual SAFRN1 servers should assist data stewards in locally diagnosing configuration issues.
    1. SAFRN1 must assist the data steward in configuring its connection with the dashboard.
    2. SAFRN1 should indicate network connectivity issues and suggest solutions.
    3. SAFRN1 should indicate database connectivity issues and suggest solutions.
    4. SAFRN1 may have a tool for assisting data stewards in converting from a database schema to a lexicon (1.1).

## Documentation Requirements (D)
 1. SAFRN1 shall have a manual for system administrators.
    1. The system administration manual shall have network, connectivity, and hardware requirements. 
    2. This manual shall have database connection and configuration instructions.
    3. This manual shall have setup instructions regarding authentication and access control infrastructure.
    4. This manual shall have other relevant setup instructions.
    5. This manual shall have relevant maintenance instructions.
 2. SAFRN1 shall have a data provider's manual.
    1. The data provider's manual shall explain how SAFRN uses data, and to whom it is revealed.
    2. This manual shall explain access control features which are available to the data owners (dependent on S5).
    3. This manual shall have instructions for restricting access to datasets (dependent on S5).
       1. This manual may make recommendations regarding best practices for access restrictions.
    4. This manual shall have instructions for adding new datasets and updating datasets with new data.
 3. SAFRN1 shall have an analyst's manual.
    1. The analyst's manual shall detail each algorithm which SAFRN1 can compute.
       1. This manual shall indicate the data representation used for each algorithm.
       2. This manual shall indicate any restrictions or limitations of each algorithm's implementation.
       3. This manual shall indicate performance expectations and limitations for each algorithm's implementation.
    2. This manual shall have instructions on how to use SAFRN1.
