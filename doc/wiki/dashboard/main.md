# Dashboard development status  
  
  The SAFRN dashboard is not production ready, and is not in active development. The documents in this section should be read as a snapshot of a work in progress.

# SAFRN Dashboard

The SAFRN Dashboard is a standalone website that can be ran as a service or self-hosted and serves the purpose of optionally assisting users in the setup, configuration, and orchestration of the SAFRN secure computation.  The dashboard helps users get a better understanding of which role they are playing and how they connect with other parties.  In terms of end results, the dashboard will produce downloadable SAFRN configuration files.

# Hosting the Service

The SAFRN Dashboard is a self-contained web-server that can be deployed as a Docker container.  The dashboard can be hosted as a service to the participants of MPC.  Participants may also choose to self-host the service.  We go into more detail in the Dashboard Hosting page (TODO).

# Login and Registration

This dashboard service is meant to help various parties come together.  We require them to register so that they can discover and identify each other, determine the roles, and so forth.  We do **not** require any or all MPC participants to use the dashboard, each individual party may choose to use or not use the dashboard to their own preference. It is meant as a simplifying step to avoid having to manually configure SAFRN.

# Features

- Registration/login
- Profile?
- Discovery of other users?
- Config Helper/wizard (same flow as desktop setup wizard?)
- Graphical party setup
- Schema setup help?
- Query setup help?
- Database setup help?

## Example workflows

### Dashboard Installation

The dashboard server is installed on some centralized host that all (potential) parties are aware of.  Only the service provider needs to install the dashboard, all other participants only access the dashboard via the web interface.  There may be more than one dashboard service provider since this is an open-source piece of software that anyone can self-host.  Parties within the same MPC must use the **same** dashboard service.  One can imagine LDAP as a reasonable analogue to the dashboard service.

### Registration

A user -- typically a representative of an organization who will play the role of a party in some MPC computation -- can register to a SAFRN dashboard.  This involves entering their username, password, email, and organization/title.

### Login

A user can login to a SAFRN dashboard with their previously registered credentials.

### Profile Update

A user can update their profile on the SAFRN dashboard.  This can include their current IP address/Domain name, open port(s), public keys, and other organization details (logo, tagline, website, etc.)

### Creating an MPC Invite

TODO: Define a workflow where a registered user can create an MPC invite.

### Responding to an MPC Invite

Upon getting the configuration file, it must be combined with other portions of the config file before being used in an MPC computation.  The MPC computation is a separate process different than the dashboard (though integration between the MPC application and the dashboard service may be future feature).


# Usage Scenarios

We go over different usage scenarios between users who do and do not use the dashboard in the Scenarios page (TODO).
