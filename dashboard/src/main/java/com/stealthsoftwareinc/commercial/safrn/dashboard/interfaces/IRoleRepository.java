package com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.Role;

@Repository
public interface IRoleRepository extends JpaRepository<Role, Integer> {
  Role findByRole(String role);
}
