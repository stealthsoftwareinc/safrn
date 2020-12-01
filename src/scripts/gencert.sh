#! /bin/sh -
#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

case $# in
  1 | 2) ;;
  *)
    cat <<EOF
Usage: gencert.sh [<signing_cert>] <output_cert>

Generates a TLS certificate file <output_cert> along with a key file
<output_cert>.key. If <signing_cert> is not given, then <output_cert>
will be self-signed. Otherwise, it will be signed with the certificate
file <signing_cert> and the key file <signing_cert>.key.

You'll be prompted to fill in various fields, but it should be fine to
just hit enter to use the default values for all fields except for the
Locality Name field when generating a self-signed certificate. If you
fail to provide a nonempty value for this field, then the certificate
might not work properly as a signing certificate.
EOF
    exit 1
  ;;
esac

case $# in
  1)
    openssl req -newkey rsa:4096 -sha256 \
      -nodes -days 99999 -x509 -out "$1" -keyout "$1.key" || exit $?
  ;;
  2)
    openssl req -newkey rsa:4096 -sha256 \
      -nodes -out "$2.csr" -keyout "$2.key" || exit $?
    openssl x509 -req -in "$2.csr" -CA "$1" -CAkey "$1.key" \
      -CAcreateserial -sha256 -days 99999 -out "$2" || exit $?
  ;;
esac
