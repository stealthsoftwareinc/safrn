package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization;

import java.util.List;
import java.util.Set;

import javax.validation.constraints.NotNull;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.EInvitationStatus;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.ESessionStatus;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import lombok.ToString;

@Getter
@Setter
@NotNull
@ToString
@AllArgsConstructor
@NoArgsConstructor
@Builder
public class SessionForm {
  private Long id;
  private String sessionName;
  private List<Vertical> verticals;
  private Set<Peer> peers;
  private String initiatorOrganizationName;
  private ESessionStatus sessionStatus;
  private EInvitationStatus invitationStatus;
  private String aboutSession;
  private boolean isDraft;
}
 