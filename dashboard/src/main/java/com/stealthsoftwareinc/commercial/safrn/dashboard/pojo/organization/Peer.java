package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization;

import javax.persistence.Embeddable;
import javax.validation.constraints.NotNull;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.EInvitationStatus;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.EPeerType;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.EPeerValue;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;
import lombok.Setter;
import lombok.ToString;

@Embeddable
@Getter
@Setter
@NotNull
@ToString
@AllArgsConstructor
@Builder
public class Peer {
  @NotNull
  private String organizationName;  
  private EPeerType peerType;
  private EPeerValue peerValue;
  @NotNull
  private int ordinalValue;
  private EInvitationStatus status;
  //@NotNull
  private String domainOrIp;
  //@NotNull
  private int port; 
  
  public Peer() {
    
  }
}
