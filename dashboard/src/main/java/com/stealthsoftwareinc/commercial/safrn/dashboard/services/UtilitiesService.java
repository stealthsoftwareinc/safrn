package com.stealthsoftwareinc.commercial.safrn.dashboard.services;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.mail.SimpleMailMessage;
import org.springframework.mail.javamail.JavaMailSender;
import org.springframework.stereotype.Service;

import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.OrgInfo;

import lombok.extern.slf4j.Slf4j;
 
@Slf4j
@Service
public class UtilitiesService {
  @Autowired
  private JavaMailSender javaMailSender;
  
  @Autowired
  private com.stealthsoftwareinc.commercial.safrn.dashboard.config.AppConfig appConfig;
  
  public  boolean isValidCredentials() {
    String smtpusername = appConfig.getSmtpUsername();
    String smtppassword = appConfig.getSmtpPassword();
    String smtpfrom = appConfig.getSmtpFrom();
    if(smtpusername == null || smtpusername.trim() == null || smtpusername.trim().length()==0 ||
        smtppassword == null || smtppassword.trim() == null || smtppassword.trim().length()==0 ||
        smtpfrom == null || smtpfrom.trim() == null || smtpfrom.trim().length()==0)
      return false;
    
    return true;  
  }
  
  public void sendEmail(String to, String subject, String message) {
    if(!isValidCredentials()) {
      log.info("\n====Invalid SMTP credentials..Ignoring Emailing..====\n"+message);
      return;
    }
    log.info("==== Sending email to "+to+" with message ====\n"+message);
    SimpleMailMessage email = new SimpleMailMessage();
    email.setSubject(subject);
    email.setText(message);
    email.setTo(to);
    email.setFrom(appConfig.getSmtpFrom());
    javaMailSender.send(email); //todo: this needs to happen in a separate thread (pool)
  }
  
  public void sendEmail(List<OrgInfo> orgs,String message) {
    for(OrgInfo orgInfo:orgs) {
      if(orgInfo.isActive())
        this.sendEmail(orgInfo.getOrgEmail(), "Message From SAFRN-Dashboard", message);
    }
  }
}
