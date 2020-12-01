package com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces;

import java.util.List;
import java.util.Optional;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository; 
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.VerticalId;

@Repository
public interface IVerticalIdRepository extends JpaRepository<VerticalId, Integer> {
  List<VerticalId> findAll();
  Optional<VerticalId> findById(Integer i);
}
