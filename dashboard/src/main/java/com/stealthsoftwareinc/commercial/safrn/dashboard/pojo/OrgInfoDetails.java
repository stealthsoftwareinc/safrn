package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo;

import java.util.List;

import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.Session;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import lombok.ToString;

@Getter
@Setter
@ToString
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class OrgInfoDetails {
  private OrgInfo orgInfo;
  private List<Session> initiatedSessions;
}
