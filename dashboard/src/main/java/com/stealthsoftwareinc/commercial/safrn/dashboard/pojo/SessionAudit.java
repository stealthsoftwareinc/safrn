package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo;

import java.time.LocalDateTime;

import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Table;
import javax.validation.constraints.NotNull;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.ESessionAuditType;

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
@Table(name = "sessionauditinfo")
@Builder
public class SessionAudit {
  @Id
  @GeneratedValue(strategy = GenerationType.IDENTITY)
  private Long id;
  private String userId;
  private ESessionAuditType auditType;
  private LocalDateTime timeStamp;
}
