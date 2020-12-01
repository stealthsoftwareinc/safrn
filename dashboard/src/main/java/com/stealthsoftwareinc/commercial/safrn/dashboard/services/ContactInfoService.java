package com.stealthsoftwareinc.commercial.safrn.dashboard.services;

import java.util.List;

import javax.transaction.Transactional;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.IContactInfoRepository;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.ContactInfo;

@Service
@Transactional
public class ContactInfoService {
  @Autowired
  private IContactInfoRepository contactInfoRepository;
  
  public void save(ContactInfo contactInfo) {
    contactInfoRepository.save(contactInfo);
  }

  public ContactInfo  findByLoginId(String loginId) {
    ContactInfo  contactInfos = contactInfoRepository.findByLoginId(loginId);
    return contactInfos;
  }
  
  public ContactInfo findByOrgId(String orgId) {
    ContactInfo  contactInfos = contactInfoRepository.findByOrgId(orgId);
    return contactInfos;
  }
  
  public List<ContactInfo> findAll(){
    return contactInfoRepository.findAll();
  }

}