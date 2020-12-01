# Peer Config Files
This peer configuration file accompanies a [study config](/json-schemas/study-config), holding peer connection information, which is liable to change.

This JSON file has just one attribute, the ``peers`` array which holds objects with the following attributes.

 - ``organizationId`` A [DBUID](/json-schemas/dbuid) indicating which organization this party represents.
   Since there is only one server from each organization participating in a study, this will be used to identify servers.
 - ``domainOrIp`` is the domain name, host name, or IP address of the server. Examples would be ``safrn-participant.stealthsoftwareinc.com`` or ``10.17.12.2``.
 - ``port`` is the TCP port number which on which the server accepts connections.
 - `certificateFingerprintAlgorithm`
   The hash algorithm that will be used to verify that the TLS
   certificate presented by a peer indeed belongs to that peer.
   The supported algorithms can be listed using the
   `openssl list -digest-algorithms | grep -v =` command.
 - `certificateFingerprint`
   The fingerprint of the peer's TLS certificate as a string of
   hexadecimal digits.
   The fingerprint can be computed using the
   `openssl x509 -fingerprint -<algorithm> -noout -in <file>`
   command, where `<algorithm>` corresponds to
   `certificateFingerprintAlgorithm`.
   The accepted names for `<algorithm>` can be listed using the
   `openssl list -digest-commands` command.
   These names may be slightly different than those accepted by
   `certificateFingerprintAlgorithm`.

## Examples


Here is an example peer config file, using the "Right to Know Before You Go" example.
```
{
  "peers": [{
      "organizationId": "fffffffffffffffffffffffffffffff0",
      "domainOrIp": "safrn-analyst.icpsr.org",
      "port": 8000,
      "certificateFingerprintAlgorithm": "SHA256",
      "certificateFingerprint": "b832eeb9473c78ebdbfa9b42b7ae1bb754702b2850d9cc2bb0ca6df6c8e0d188"
    }, {
      "organizationId": "fffffffffffffffffffffffffffffff1",
      "domainOrIp": "safrn-data.irs.gov",
      "port": 8000,
      "certificateFingerprintAlgorithm": "SHA256",
      "certificateFingerprint": "709a3239f48d7bd4f0113897dcfdccc3be63efd36f498169530971519ec643b4"
    }, {
      "organizationId": "fffffffffffffffffffffffffffffff2",
      "domainOrIp": "safrn-data.umich.edu",
      "port": 8000,
      "certificateFingerprintAlgorithm": "SHA256",
      "certificateFingerprint": "3f283e923852188b6e16311c70a1b2258a613ffa4bdb8e779e9e58899f556630"
    }, {
      "organizationId": "fffffffffffffffffffffffffffffff3",
      "domainOrIp": "safrn-data.ucla.edu",
      "port": 8000,
      "certificateFingerprintAlgorithm": "SHA256",
      "certificateFingerprint": "331e343c8438f30420cb3eec675fed77615082dff30cbab61b73eb480e6d2c7d"
    }]
}
```
