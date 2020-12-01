package com.stealthsoftwareinc.commercial.safrn.dashboard.misc;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;
import org.springframework.validation.Errors;
import org.springframework.validation.ValidationUtils;
import org.springframework.validation.Validator;

import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.RegistrationForm;
import lombok.extern.slf4j.Slf4j;

//ref https://docs.spring.io/spring/docs/4.1.x/spring-framework-reference/html/validation.html

@Slf4j
@Component
public class UserInputValidator implements Validator {
  @Autowired
  private com.stealthsoftwareinc.commercial.safrn.dashboard.config.AppConfig appConfig;

  @Override
  public boolean supports(Class<?> aClass) {
    return RegistrationForm.class.equals(aClass);
  }

  @Override
  public void validate(Object o, Errors errors) {
    RegistrationForm registration = (RegistrationForm) o;

    final String email  = registration.getEmail();
    final int emailLength = email.length();
    final int passwordLength = registration.getPassword().length();
    
    final int minEmailLength = appConfig.getEmailMinLength();
    final int maxEmailLength = appConfig.getEmailMaxLength();

    final int minPasswordLength = appConfig.getPasswordMinLength();
    final int maxPasswordLength = appConfig.getPasswordMaxLength();

    ValidationUtils.rejectIfEmptyOrWhitespace(errors, "email", "NotEmpty");
    if (emailLength < minEmailLength || emailLength > maxEmailLength) {
      log.error("email = "+email+" .. its size = "+emailLength+" ..and did not meet the size requirement");
      errors.rejectValue("email","Size.registrationform.email", "...Use between "+minEmailLength+" and "+maxEmailLength+" characters...");
    }

    ValidationUtils.rejectIfEmptyOrWhitespace(errors, "password", "NotEmpty");
    if (passwordLength < minPasswordLength || passwordLength > maxPasswordLength) {
      log.error("passwordLength = "+passwordLength+" ..and did not meet the size requirement");
      errors.rejectValue("password", "Size.registrationform.password","...Use between "+minPasswordLength+" and "+maxPasswordLength+" characters...");
    }

    if (!registration.getPassword().equals(registration.getPasswordAgain())) {
      log.error("passwords donot match..");
      errors.rejectValue("passwordAgain", "Diff.registrationform.passwordAgain","...Passwords does not match...");
    }
  }
}
