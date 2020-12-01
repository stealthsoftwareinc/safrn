package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.json;

import javax.validation.constraints.NotNull;

import lombok.AllArgsConstructor;
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
public class JsonPeerConnectionConfig {
  private String organizationId;
  private String domainOrIp;
  private int port; 
}
