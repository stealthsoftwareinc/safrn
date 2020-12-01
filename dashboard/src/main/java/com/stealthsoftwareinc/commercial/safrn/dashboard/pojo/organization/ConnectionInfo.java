package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization;

import javax.persistence.Entity;
import javax.persistence.Id;
import javax.persistence.Table;
import javax.validation.constraints.NotNull;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.AbstractDBAudit;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import lombok.ToString;

@Entity 
@Getter
@Setter
@NotNull
@ToString
@NoArgsConstructor
@AllArgsConstructor
@Table(name = "connectioninfo")
@Builder
public class ConnectionInfo  extends AbstractDBAudit{
  @Id
  private String loginId;
  private String orgId;
  private String connectionName;
  private String ipAddress;
  private Integer port;
}