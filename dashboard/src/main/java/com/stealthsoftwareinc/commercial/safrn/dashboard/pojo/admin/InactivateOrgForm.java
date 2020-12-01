package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.admin;

import javax.validation.constraints.NotNull;
import javax.validation.constraints.Size;

import static com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.IConstants.*;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.Setter;
import lombok.ToString;

@Getter
@Setter
@NotNull
@ToString
@AllArgsConstructor
public class InactivateOrgForm {
  @Size(min =  minMinPasswordLength, max =  maxMaxPasswordLength)
  private String orgName;
}
