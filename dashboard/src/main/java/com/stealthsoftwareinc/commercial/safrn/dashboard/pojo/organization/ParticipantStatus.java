package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization;

import javax.validation.constraints.NotNull;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.EInvitationStatus;
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
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class ParticipantStatus {
  //private String loginId;
  private String orgName;
  private EInvitationStatus status;
}
