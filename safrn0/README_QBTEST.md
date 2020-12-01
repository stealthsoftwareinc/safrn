#NOTE

This readme describes the functionality of `/src/com/stealthsoftwareinc/commercial/safrn/QBTest.java` in this repo. See Shaan's repo `https://gitlab.stealthsoftwareinc.com/shaanmathur/qbtest-cli` for the current CLI version.

# QueryBuilder inputs

The QueryBuilder constructor takes in the following inputs:

  * String analysis
  * boolean loanlinkbool
  * HashMap<String, ArrayList<String>> domainNames
  * ArrayList<String> dvList
  * ArrayList<String> ivList
  * int localParty
  * int remoteParty
  * String tableName

They can be classified into several categories:

  * Determined by FrontServerHttpHandler, based only on REST query
    - analysis
    - loanlinkbool
  * Determined by FrontServerHttpHandler, by REST query and a look-up from a FrontServerGlobals object
    - dvList
    - ivList
    - remoteParty
  * Determined by BackServerRawHandler, by look-up from a BackServerGlobals object
    - domainNames
    - localParty
    - tableName

# SAFRN conditions

In SAFRN, the inputs must satisfy the following conditions:

  * analysis: Can be "freq" or "mean"
  * loanlinkbool: Can be `true` only if used with an income/loan database with a "School" column
  * domainNames: Only needed if `loanlinkbool == true`. Must contain a key `"School"`.
  * dvList, ivList: A list of String objects matching column names of the working table
  * localParty: If `0`, return null. If `1`, return `null` unless `ivList.size()==0`. If `2`, return null unless `ivList.size()!=0`.
  * remoteParty: We requre `0 <= remoteParty - 2 < domainNames["School"].size()`
  * tableName: The name of the working table

# QBTest.java conditions

In QBTest.java, we modify the conditions in the following ways:

  * domainNames: Always set to `{"School": {"Test school"}}`
  * dvList, ivList: Does not check against global variables. Checks for redundancy and non-empty intersection between lists.
  * localParty: Set to `ivList.size()+1`, so that QueryBuilder() never returns null.
  * remoteParty: Always set to `2`.
  * tableName: Always set to `"SQL_TABLE"`.

# QBTest.java use

All queries in QBTest.java must begin with the seven character string `"\query?"`, followed by a list of pairs of the form `Attrib=Val`, seperated by `&`. When `Val` is a list, its entries are separated by `+`.

QBTest requires the .java package files for `sst` to be placed into the folder `/src/com/stealthsoftwareinc`

QBTest can be run in manual mode, where queries are entered by hand, and the output given in the console, or on files. By default, the input file is `RESTQueries.txt` and the output is `SQLQueries.txt`. These files should be placed in the folder `/src`.

QBTest should be compiled and run from `/src` with:

`javac com/stealthsoftwareinc/commercial/safrn/QBTest.java`

and

`java com.stealthsoftwareinc.commercial.safrn.QBTest`
