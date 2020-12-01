package com.stealthsoftwareinc.commercial.safrn.dashboard.services;

import java.util.Arrays;
import java.util.HashSet;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
 
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.IRoleRepository;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.IUserRepository;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.Role;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.User;

import lombok.extern.slf4j.Slf4j;

@Slf4j
@Service
public class DashboardUserService {
  @Autowired
  private IUserRepository userRepository;
  @Autowired
  private IRoleRepository roleRepository;

  public void save(User user, String role) {
    Role userRole = roleRepository.findByRole(role);  
    user.setRoles(new HashSet<Role>(Arrays.asList(userRole)));
    userRepository.save(user);
  }
  
  public void save(User user) {
    log.info("saving user:"+user.toString());
    userRepository.save(user);
  }

  public User findByLoginId(String loginId) {
    return userRepository.findByLoginId(loginId);
  }
  
  public User findByOrgId(String orgId) {
    return userRepository.findByOrgId(orgId);
  }

  public Iterable<User> findAll() {
    return userRepository.findAll();
  }
  
//  public  void deleteAll() {
//    userRepository.deleteAll();
//  }

}