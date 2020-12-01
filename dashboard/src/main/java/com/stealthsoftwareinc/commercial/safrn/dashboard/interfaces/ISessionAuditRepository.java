package com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces;

import java.util.List;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.SessionAudit;

@Repository
public interface ISessionAuditRepository extends JpaRepository<SessionAudit, Integer> {
  SessionAudit save(SessionAudit sessionAudit);
  List<SessionAudit> findByUserIdOrderByIdDesc(String userId);
}
