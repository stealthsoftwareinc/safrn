package com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces;

import java.util.List;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.ConnectionInfo;

@Repository
public interface IConnectionInfoRepository extends JpaRepository<ConnectionInfo, Integer> {
  ConnectionInfo findByLoginId(String loginId);
  //List<ConnectionInfo> findByLoginId(String loginId);
  //List<ConnectionInfo> findByOrgId(String orgId);
  ConnectionInfo  findByOrgId(String orgId);
  List<ConnectionInfo> findAll();
}
