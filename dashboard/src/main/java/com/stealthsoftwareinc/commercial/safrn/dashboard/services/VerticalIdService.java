package com.stealthsoftwareinc.commercial.safrn.dashboard.services;

import java.util.List;
import java.util.Optional;

import javax.transaction.Transactional;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.IVerticalIdRepository;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.VerticalId;

@Service
@Transactional
public class VerticalIdService {
  @Autowired
  private IVerticalIdRepository verticalIdRepository;
  
  public Optional<VerticalId> findById(int i) {
    return verticalIdRepository.findById(i);
  }
  
  public void save(VerticalId verticalId) {
    verticalIdRepository.save(verticalId);
  }
  
  public List<VerticalId> findAll(){
    return verticalIdRepository.findAll();
  }
}