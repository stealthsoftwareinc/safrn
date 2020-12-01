package com.stealthsoftwareinc.commercial.safrn.dashboard.services;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.authority.SimpleGrantedAuthority;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.core.userdetails.UserDetailsService;
import org.springframework.security.core.userdetails.UsernameNotFoundException;
import org.springframework.stereotype.Service;

import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.Role;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.User;

import lombok.extern.slf4j.Slf4j;

import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

@Slf4j
@Service
public class DashboardUserDetailsService implements UserDetailsService {
  // https://docs.spring.io/spring-security/site/docs/3.1.x/reference/technical-overview.html#tech-userdetailsservice
  // this is DAO class that gets user from DB and converts into Spring user for
  // authenticationmanager to create token on success and add it to
  // securitycontext
  // (related dashboard class:SecurityConfigurer.java)

  @Autowired
  private DashboardUserService userService;

  public DashboardUserDetailsService() {
    log.info("DashboardUserDetailsService ..dashboard...");
  }

  @Override
  @Transactional
  public UserDetails loadUserByUsername(String loginId){
    User user = userService.findByLoginId(loginId);  
    log.info("DashboardUserDetailsService ..loadUserByUsername..." + (user == null?null:user.toString()));
    if(user == null)
      throw new UsernameNotFoundException("user not found with id: "+loginId+" ..register to use the system");
    Set<GrantedAuthority> roles = new HashSet<>();
    for (Role role : user.getRoles()) {
      roles.add(new SimpleGrantedAuthority(role.getRole()));
    }
    List<GrantedAuthority> authorities = new ArrayList<>(roles);
    return new org.springframework.security.core.userdetails.User(user.getLoginId(), user.getPassword(), true, true, true,
        true, authorities);
  }
}
