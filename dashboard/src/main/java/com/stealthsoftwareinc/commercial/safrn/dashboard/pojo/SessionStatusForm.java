package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo;

import javax.validation.constraints.NotNull;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.EInvitationStatus;
import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
@NotNull
public class SessionStatusForm {
  private String loginId;
  private String sessionName;
  private EInvitationStatus status;
  private ConnectionInfoForm connectionInfoForm;
}
