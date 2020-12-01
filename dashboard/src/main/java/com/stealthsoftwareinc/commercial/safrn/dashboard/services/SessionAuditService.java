package com.stealthsoftwareinc.commercial.safrn.dashboard.services;

import java.util.List;

import javax.transaction.Transactional;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.ISessionAuditRepository;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.SessionAudit;

@Service
@Transactional
public class SessionAuditService {
  @Autowired
  private ISessionAuditRepository iSessionAuditRepository;
  
  public SessionAudit save(SessionAudit sessionAudit) {
    return iSessionAuditRepository.save(sessionAudit);
  }
  public List<SessionAudit> findByUserIdOrderByIdDesc(String userId) {
    return iSessionAuditRepository.findByUserIdOrderByIdDesc(userId);
  }
}
