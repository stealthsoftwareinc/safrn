package com.stealthsoftwareinc.commercial.safrn.dashboard.services;

import java.util.List;

import javax.transaction.Transactional;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.IConnectionInfoRepository;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.ConnectionInfo;

@Service
@Transactional
public class ConnectionInfoService {
  @Autowired
  private IConnectionInfoRepository connectionInfoRepository;
  
  public void save(ConnectionInfo connectionInfo) {
    connectionInfoRepository.save(connectionInfo);
  }

  public ConnectionInfo  findByLoginId(String loginId) {
    ConnectionInfo connectionInfos = connectionInfoRepository.findByLoginId(loginId);
    return connectionInfos;
  }
  
  public  ConnectionInfo  findByOrgId(String orgId) {
    ConnectionInfo  connectionInfos = connectionInfoRepository.findByOrgId(orgId);
    return connectionInfos;
  }
  
  public List<ConnectionInfo> findAll(){
    return connectionInfoRepository.findAll();
  }

}