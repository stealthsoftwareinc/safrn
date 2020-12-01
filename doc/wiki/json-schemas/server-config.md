# Server Configuration

## Notes
(As a dataowner, e.g.) When running safrn-core.exe, you can either:
  1) Run with command-line option ``org_id = XXX``
  2) Run with no command-line option, or to be explicit, command line ``server_config=XXX``
     (defaults to ``server_config=server.cfg`` in local/working dir).

If you use (1), all the values below will be assigned default values and/or read (using ``org_id`` as "key") from other .cfg files. Othersie, the server.cfg file should be .json format as specified below.

## Structure

This document details the server configuration file, which SAFRN1 will read on start up.
The config file is in JSON. It has the following attributes

 - ``organizationId`` The [DBUID]](/json-schemas/dbuid) assigned to the organization which this server represents.
 - ``organizationName`` (Optional) the name of the organization, for log/display uses.
 - ``port`` the TCP port number which the server listens on.
 - ``address`` (optional, defaults to ``0.0.0.0``), and supports only the numbers and dots format specified by ``inet_aton``.
 - ``certificateInfo`` (Optional). If provided, need to specify info for self certificates and for authority:
    - ``authorityCertificate`` (Optional). If provided, exactly one of the following fields must be specified:
      - ``certificateAuthorityFile`` (Optional)
        A path to a file of TLS CA certificates in PEM format.
        If the path is not absolute, it is taken relative to the current
        working directory.

        This variable corresponds to the `CAfile` variable of OpenSSL.
      - `certificateAuthorityDirectory` (Optional)
        A path to a directory of TLS CA certificates in PEM format.
        If the path is not absolute, it is taken relative to the current
        working directory.

        Each file should contain exactly one certificate and be named as
        the certificate's subject name hash with a `.0` suffix.
        For example, a file might be named `4ac4756f.0`.
        The hashes can be computed using the
        `openssl x509 -hash -noout -in <file>` command.
        If multiple certificates have the same hash, the files should be
        named with increasing suffixes like `.0`, `.1`, `.2`, and so on.

        This variable corresponds to the `CApath` variable of OpenSSL.
   - ``selfCertficate`` (Optional). If provided, *both* of the following fields must be specified:
     - `certificateFile`
     A path to a TLS certificate file in PEM format that identifies
     this peer.
     If the path is not absolute, it is taken relative to the current
     working directory.

     The file can optionally contain intermediate CA certificates, in
     which case the file should be ordered such that each certificate
     is signed by the immediately following certificate.
     - `certificateKeyFile`
       A path to the private key file for `certificateFile` in PEM
       format.
       If the path is not absolute, it is taken relative to the current
       working directory.
 - ``approvedSessions`` is a list of objects containing file names of the [session](/json-schemas/session-config), [peer](/json-schemas/peer-config), and [database](/json-schemas/database-config) files.
   This may be replaced with a directory name which will be scanned for these files, but at the moment I'm doing this out of lazyness.
   - ``study`` the study configuration file name.
   - ``peer`` the peer configuration file which is paired to this study.
   - ``database`` the database configuration file for this study.



## Example

?
