package com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces;

public enum SessionStatus {
  CREATED,//initiator has sent invitations
  ACCEPTED,//all the participants have accepted
  DIMISSED;//because of violation, this is no longer a valid session
}
