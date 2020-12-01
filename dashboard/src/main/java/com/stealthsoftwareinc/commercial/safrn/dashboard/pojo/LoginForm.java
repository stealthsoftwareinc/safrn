package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo;

import javax.validation.constraints.Email;
import javax.validation.constraints.NotNull;
import javax.validation.constraints.Size;

import static com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.IConstants.*;

import lombok.Getter;
import lombok.Setter;
import lombok.ToString;

@Getter
@Setter
@NotNull
@ToString
public class LoginForm {
  @Size(min =  minMinLoginIdLength, max =  maxMaxLoginIdLength)
  @Email
  private String email;
  @Size(min =  minMinPasswordLength, max =  maxMaxPasswordLength)
  private String password;
}
