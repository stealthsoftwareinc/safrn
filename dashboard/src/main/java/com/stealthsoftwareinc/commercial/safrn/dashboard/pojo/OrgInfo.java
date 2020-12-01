package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo;

import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Lob;
import javax.persistence.Table;
import javax.validation.constraints.NotNull;

import org.hibernate.annotations.Type;

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
@Table(name = "orgmetainfo")
@Builder
public class OrgInfo  extends AbstractDBAudit{
  @Id
  @GeneratedValue(strategy = GenerationType.IDENTITY)
  private Long orgId;
  @Lob
  @Type(type="org.hibernate.type.BinaryType")
  private byte[] dbuid;
  private String orgName;
  private String orgEmail;
  private boolean active; 
}