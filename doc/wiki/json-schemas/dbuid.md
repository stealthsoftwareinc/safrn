# Dashboard Unique ID
The SAFRN Dashboard can generate unique IDs and ensure it issues an ID only once.
However, it cannot generate a DBUID which will be unique in contexts in which multiple dashboards are concerned.
A DBUID is additionally not guaranteed uniqueness in two different contexts, such as comparing a Study ID to an Organization ID.

A DBUID is a 16 byte or 128 bit unsigned integer.
They are generated randomly, however should be checked against a datastore of existing DBUIDs for duplicates.
For transport in JSON they should be serialized as hexadecimal numbers with the most-significant-bit first, and padded to full length with leading zeros.
For transport in binary they should be serialized as a sequence of bytes, again with most-significant-bit first.
