package com.stealthsoftwareinc.commercial.safrn.dashboard.services;

import java.util.List;

import javax.transaction.Transactional;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.ISessionRepository;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.Session;

import lombok.extern.slf4j.Slf4j;

@Slf4j
@Service
@Transactional
public class SessionInfoService {
  @Autowired
  private ISessionRepository iSessionRepository;
  public Session findById(Long id) {
    return iSessionRepository.findById(id);
  }
  
  public List<Session> findSessionsByInitiatorOrganizationName(String orgName) {
    return iSessionRepository.findByInitiatorOrganizationNameAndIsDraftOrderById(orgName,false);
  }
  
  public List<Session> findDraftsByInitiatorOrganizationName(String orgName) {
    return iSessionRepository.findByInitiatorOrganizationNameAndIsDraftOrderById(orgName,true);
  }
  
  public Session findDraftBySessionNameAndInitiatorOrganizationName(String sessionName,String orgName) {
    return iSessionRepository.findBySessionNameAndInitiatorOrganizationNameAndIsDraft(sessionName,orgName,true);
  }
  
  public Session findSessionBySessionNameAndInitiatorOrganizationName(String sessionName,String orgName) {
    return iSessionRepository.findBySessionNameAndInitiatorOrganizationNameAndIsDraft(sessionName,orgName,false);
  }
  
  public Session findSessionBySessionName(String sessionName) {
    Session session = iSessionRepository.findBySessionNameAndIsDraft(sessionName,false);
    return session;
  }
  
  public Session findDraftBySessionName(String sessionName) {
    Session session = iSessionRepository.findBySessionNameAndIsDraft(sessionName,true);
    return session;
  }

  public List<Session> findSessionOrDraftBySessionName(String sessionName) {
    List<Session> session = iSessionRepository.findBySessionName(sessionName);
    return session;
  }
  
  public void deleteDraftBySessionNameAndInitiatorOrganizationName(String sessionName,String orgName) {
    iSessionRepository.deleteBySessionNameAndInitiatorOrganizationNameAndIsDraft(sessionName,orgName,true);
  }
  
  public List<Session> findAll() {
    List<Session> sessions = iSessionRepository.findAll();//Sort.Direction.ASC, "id");
    return sessions;
  }

  public List<Session> findAllSessions() {//andNoDrafts
    List<Session> sessions = iSessionRepository.findByIsDraft(false);
    return sessions;
  }
  
  public List<Session> findAllDraft() {//andNoSessions
    List<Session> sessions = iSessionRepository.findByIsDraft(true);
    return sessions;
  }
  
  public void save(Session mySession) {
    log.info("saving session:"+mySession.toString());
    iSessionRepository.save(mySession);
  }
}
