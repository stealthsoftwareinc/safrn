package com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces;

import java.util.List;

import org.springframework.data.jpa.repository.JpaRepository;

import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.User;

// This will be AUTO IMPLEMENTED by Spring into a Bean called userRepository
// CRUD refers Create, Read, Update, Delete

public interface IUserRepository extends JpaRepository<User, Integer> {
  User findByLoginId(String loginId);
  User findByOrgId(String orgId);
  //long deleteByLoginId(String loginId); //todo
  List<User> findAll();
  //void deleteAll();
}
