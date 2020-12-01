package com.stealthsoftwareinc.commercial.safrn.dashboard.services;

import java.util.List;

import javax.transaction.Transactional;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.IOrgInfoRepository;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.OrgInfo;

@Service
@Transactional
public class OrgInfoService {
  @Autowired
  private IOrgInfoRepository orgInfoRepository;
  
  public void save(OrgInfo orgInfo) {
    orgInfoRepository.save(orgInfo);
  }

  public List<OrgInfo> findAll() {
    return orgInfoRepository.findAll();
  }

  public  OrgInfo  findByOrgName(String orgName) {
    OrgInfo  orgInfos = orgInfoRepository.findByOrgName(orgName);
    return orgInfos;
  }
  
  public  OrgInfo  findByOrgEmail(String orgEmail) {
    OrgInfo orgInfos = orgInfoRepository.findByOrgEmail(orgEmail);
    return orgInfos;
  }

  public OrgInfo findByOrgId(Long orgId) {
    OrgInfo  orgInfos = orgInfoRepository.findByOrgId(orgId);
    return orgInfos;
  }
  
  public void deleteByOrgName(String orgName) {//todo
    orgInfoRepository.deleteByOrgName(orgName);
  }
  
  public boolean orgExists(String orgName) {
    OrgInfo  org = this.findByOrgName(orgName);
    if (org == null) {
      return false;
    }
    return true;
  }
  
//  public void deleteAll() {
//    orgInfoRepository.deleteAll();
//  }
}