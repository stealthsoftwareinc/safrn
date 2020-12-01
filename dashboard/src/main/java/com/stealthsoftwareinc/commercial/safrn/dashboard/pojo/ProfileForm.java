package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo;


import javax.validation.constraints.NotNull;

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
public class ProfileForm {
  private String password;
  private String connectionName;
  private String ipAddress;
  private String port;
  private String about;
  private String phone;
  private String address;
}
