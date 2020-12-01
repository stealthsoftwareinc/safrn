package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo;

import javax.validation.constraints.NotNull;
import javax.validation.constraints.Size;

import static com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.IConstants.*;

import lombok.AllArgsConstructor;
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
public class PasswordForm {
  @Size(min =  minMinPasswordLength, max =  maxMaxPasswordLength)
  private String password;
  @Size(min =  minMinPasswordLength, max =  maxMaxPasswordLength)
  private String passwordAgain;
}
