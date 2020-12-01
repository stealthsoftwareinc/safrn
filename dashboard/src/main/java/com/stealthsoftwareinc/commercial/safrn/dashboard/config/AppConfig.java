package com.stealthsoftwareinc.commercial.safrn.dashboard.config;

import javax.validation.constraints.NotNull;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.context.annotation.PropertySource;

import lombok.Getter;
import lombok.Setter;
import lombok.ToString;

//@Configuration //strange it is not loading config file, hence change in DashboardApplication.java
@Getter
@Setter
@NotNull
@ToString
@ConfigurationProperties(prefix = "app")
@PropertySource(value = "classpath:application.properties")
public class AppConfig {
  private String adminLoginId;
  private String adminPassword;
  private int emailMinLength;
  private int emailMaxLength;
  private int passwordMinLength;
  private int passwordMaxLength;
  private int bcryptStrength;
  private String cryptoAlgo;
  private int orgDbuidLength;
  private int verticalDbuidLength;
  @Value("${spring.mail.username}")
  private String smtpUsername;
  @Value("${spring.mail.password}")
  private String smtpPassword;
  @Value("${spring.mail.from}")
  private String smtpFrom;
}
