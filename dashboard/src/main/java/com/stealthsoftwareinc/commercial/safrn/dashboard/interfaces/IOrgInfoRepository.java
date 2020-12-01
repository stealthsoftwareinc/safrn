package com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces;

import java.util.List;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.OrgInfo;

@Repository
public interface IOrgInfoRepository extends JpaRepository<OrgInfo, Integer> {
  List<OrgInfo> findAll();
  OrgInfo  findByOrgName(String orgName);
  OrgInfo  findByOrgEmail(String orgEmail);
  OrgInfo findByOrgId(Long orgId);
  void deleteByOrgName(String orgName); //todo
  //void deleteAll();
}
