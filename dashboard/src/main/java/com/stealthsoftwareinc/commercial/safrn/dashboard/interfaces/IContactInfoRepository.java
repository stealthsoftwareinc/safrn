package com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces;

import java.util.List;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.ContactInfo;

@Repository
public interface IContactInfoRepository extends JpaRepository<ContactInfo, Integer> {
  ContactInfo findByLoginId(String loginId);	
  //List<ContactInfo> findByLoginId(String loginId);
  //List<ContactInfo> findByOrgId(String orgId);
  ContactInfo findByOrgId(String orgId);
  List<ContactInfo> findAll();
}
