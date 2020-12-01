package com.stealthsoftwareinc.commercial.safrn.dashboard.security;

import javax.annotation.PostConstruct;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.security.crypto.argon2.Argon2PasswordEncoder;
import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.security.crypto.password.Pbkdf2PasswordEncoder;
import org.springframework.security.crypto.scrypt.SCryptPasswordEncoder;
import org.springframework.stereotype.Service;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.ECryptoAlgos;

import lombok.Getter;
import lombok.extern.slf4j.Slf4j;

@Service
@Slf4j
public class Encoder  {
  @Value("${app.cryptoAlgo}") 
  private String cryptoAlgorithm;
  @Value("${app.bcryptStrength}") 
  private int cryptoStrength;
  @Getter
  private PasswordEncoder passwordEncoder;
  
  @PostConstruct
  public void init() throws Exception{
    if(cryptoAlgorithm.equalsIgnoreCase(ECryptoAlgos.BCRYPT.toString()))
      passwordEncoder = new BCryptPasswordEncoder(cryptoStrength);
    else if(cryptoAlgorithm.equalsIgnoreCase(ECryptoAlgos.SCRYPT.toString()))
      passwordEncoder = new SCryptPasswordEncoder();//untested
    else if(cryptoAlgorithm.equalsIgnoreCase(ECryptoAlgos.ARGON2.toString()))
      passwordEncoder = new Argon2PasswordEncoder();//untested
    else if(cryptoAlgorithm.equalsIgnoreCase(ECryptoAlgos.PBKDF2.toString()))
      passwordEncoder = new Pbkdf2PasswordEncoder();//untested
    else {
      log.error("Unknow crypto algo specified in application properties.."+cryptoAlgorithm);
      throw new RuntimeException("invalid app.cryptoAlgo specified in application.properties file.."+cryptoAlgorithm);
    }
  }

  public String encode(String rawPassword) {
    return passwordEncoder.encode(rawPassword);
  }
  
  public boolean matches(String rawPassword,String encodedPassword) {
    return passwordEncoder.matches(rawPassword, encodedPassword);
  }
}
