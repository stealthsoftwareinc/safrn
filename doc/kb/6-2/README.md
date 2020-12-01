# How the dashboard test server works

We have an AWS account for SAFRN.
The IAM login URL is
<https://242081808536.signin.aws.amazon.com/console>.
You can request an IAM account by contacting Quinn.

There are two EC2 instances in the Oregon (us-west-2) region:
`dashboard-test-instance`, which runs
<https://safrn-dashboard-test.stealthsoftwareinc.com>, and
`dashboard-cnc-instance`, which occasionally renews the HTTPS
certificate and uploads it to `dashboard-test-instance`.

Only Quinn has SSH access to `dashboard-cnc-instance` because it
contains sensitive credentials.

Anyone else can be given SSH access to `dashboard-test-instance`.
You can request SSH access by contacting Quinn.
The SSH username is `ubuntu`.

Inside the home directory of the server, you'll find three files:

```
fullchain.pem  privkey.pem  refresh
```

`fullchain.pem` and `privkey.pem` are the HTTPS certificate files that
occasionally get updated by `dashboard-cnc-instance`.

`refresh` is a Bash script that currently has simple behavior: destroy
the current dashboard container, make sure the dashboard image is up to
date by pulling from
`registry.stealthsoftwareinc.com/stealth/safrn/master/safrn-dashboard:latest`,
and start a new dashboard container.

The server is open to the world on port 22 for SSH, but it otherwise has
a whitelist in `dashboard-test-security-group`.
