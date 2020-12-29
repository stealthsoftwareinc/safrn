# Study Configuration

## Introduction
The study configuration file details an MPC study and is signed by each participant before the study is allowed to begin.
The study prominently consists of the lexicon (defines one or more vertical partitions and the column names and datatypes of each such partition), a set of verticals used for computation, the organizations partaking in the calculations, and the restrictions on allowable queries.
The study does not include connection information for peers, as that is liable to change between the initiation of a study, and the execution of said study.
Instead, a separate [peer configuration](/doc/wiki/json-schemas/peer-config.md) file is provided by the _Dashboard_.

Each participant (or peer) can have a combination of the following 4 roles, and each role may be played by multiple parties:

 - _Analyst_ who makes queries.
 - _Dataownwer_ who performs MPC using their own data combined with other _Dataowner's_ data.
 - _Dealer_ who generates correlated randomness for use during MPC.
 - _Recipient_ receives the result.

## Questions

* How is fixed point going to be represented?
  * In the study-config, real numbers will be represented as "whole_bits" and "fraction_bits" format (loosely based on "Q Numerical Format").  If another form is needed, reals will be converted before crypto operations.

## Structure

 - ``studyId`` a [DBUID](/doc/wiki/json-schemas/dbuid.md) unique to this study. Assigned by the dashboard.
 - ``<<array>> lexicon``
   - ``<<integer>> verticalIndex``
   - ``<<array<columnLexicon_t>>> columns`` -- Each item in the array is a single flat object with the following attributes:
     - ``<<integer>> columnIndex``
     - ``<<string>> name`` -- is a string form column name.
     - ``<<enumeration>> type`` -- is an enumerated-value string which may be:
       - ``integer`` -- see [Theory Design Doc](/doc/wiki/design/theory.md) more info
       - ``real`` -- see [Theory Design Doc](/doc/wiki/design/theory.md) more info
       - ``categorical`` -- implicitly identifies a column as a ``<<bool>>``.  Also labels this column are part of a collection of other ``<<bool>>`` columns that map to enumeration values.
       - ``bool`` -- Stand-alone boolean value that can have an integer value of 0 or 1.
     - *(optional)* ``<<integer>> scale`` -- only used if "real" type is specified <br>
     *This follows the SQL standard definition for "scale", please see this [page](https://docs.microsoft.com/en-us/sql/t-sql/data-types/precision-scale-and-length-transact-sql?view=sql-server-ver15) with examples.*

     - *(optional)* ``<<integer>> precision`` -- only used if "real" type is specified <br>
     *This follows the SQL standard definition for "precision", please see this [page](https://docs.microsoft.com/en-us/sql/t-sql/data-types/precision-scale-and-length-transact-sql?view=sql-server-ver15) with examples.*

     - *(optional)* ``<<bool>> signed`` -- only used if "integer" type is specified
	     - "true" -- number is signed
	     - "false" -- number is unsigned
     - *(optional)* ``<<integer>> bits`` -- only used if "integer" type is specified
     - *(optional)* ``<<string>> categorical`` -- only used if "categorical" type is specified

 - ``peers`` is an array of all the peers participating.
   Each peer is an object holding identifying information, and some permissions.
   - ``organizationId`` the organization ID [Dashboard UID](/doc/wiki/json-schemas/dbuid.md).
   - ``OrganizationName`` (optional) the organization's name, for logging and display purposes.
   - ``analyst`` (optional) This object indicates by presence if the peer is allowed to make new queries.
     Its attributes detail restrictions to the peer's analyst permissions.
     - ``allowedQueryCount``: limit on number of queries the analyst may make.
     - TODO: add more restrictions, but don't go nuts on scope creep.
   - ``dataowner`` (optional) This object indicates by presence if the peer can contribute data to the processing of a query.
     - ``vertical`` The vertical which the peer is allowed to present.
      Represented as an index into the ``lexicon`` array.
   - ``dealer`` (optional) This object indicates by presence if the peer can deal randomness to other peers.
     Attributes indicate restrictions on the ability to deal randomness.
     - TODO: restrictions on who the dealer can deal to.
   - ``recipient`` (optional) This object indicates by presence if the peer will recieve the results of a quey.
     Attributes may indicate restrictions on which query results this peer receives.
     - TODO: what restrictions on the recipient can we make
 - TODO: Query restrictions

The following attributes are unnecessary for MPC calculations, however we include them for easy reading by users.

 - ``studyName`` A brief human form name for the study.
 - ``createdDate`` is the date and time that the study initiator created the study proposal. Use ISO8601 notation for date and time. Assigned by the dashboard.

## Example
See ``study.json`` file included in the repository.
