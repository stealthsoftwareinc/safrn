package com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces;

import java.util.List;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.Session;

@Repository
public interface ISessionRepository extends JpaRepository<Session, Integer> {
  Session findById(Long id);
  List<Session> findBySessionName(String sessionName);
  List<Session> findByIsDraft(boolean isDraft);
  Session findBySessionNameAndIsDraft(String sessionName,boolean isDraft);
  List<Session> findByInitiatorOrganizationNameOrderById(String sessionName);
  List<Session> findByInitiatorOrganizationNameAndIsDraftOrderById(String orgName,boolean isDraft);
  Session findBySessionNameAndInitiatorOrganizationNameAndIsDraft(String sessionName,String orgName,boolean isDraft);
  void deleteBySessionNameAndInitiatorOrganizationNameAndIsDraft(String sessionName,String orgName,boolean isDraft); 
  List<Session> findAll();
  
//  @Query(value = "SELECT max(verticalId) FROM Session")
//  public Long maxVerticalId();
  
  //void save(Session mySession);
}
