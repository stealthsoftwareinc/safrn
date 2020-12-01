package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo;

import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Table;
import javax.validation.constraints.NotNull;

import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import lombok.ToString;

@Entity // This tells Hibernate to make a table out of this class
@Getter
@Setter
@NotNull
@ToString
@NoArgsConstructor
@Table(name = "roles")
public class Role {
  @Id
  @GeneratedValue(strategy = GenerationType.AUTO)
  private Long id;
  private String role;
  
  public Role(String role) {
    this.role = role;
  }
}
