package com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces;

import java.time.LocalDateTime;
import javax.persistence.Column;
import javax.persistence.MappedSuperclass;
import javax.validation.constraints.NotNull;

import org.hibernate.annotations.CreationTimestamp;
import org.hibernate.annotations.UpdateTimestamp;
import org.springframework.data.annotation.CreatedBy;
import org.springframework.data.annotation.LastModifiedBy;

import lombok.Getter;
import lombok.Setter;
import lombok.ToString;
@Getter
@Setter
@NotNull
@ToString
@MappedSuperclass
public  class AbstractDBAudit { //stored in UTC zone. Modify the zone in application.properties for different zone
  @CreationTimestamp
  @Column(name = "created_on", nullable = false, updatable=false)
  private LocalDateTime  createDate;

  @UpdateTimestamp
  @Column(name = "updated_on")
  private LocalDateTime  updateDate;
  
  @CreatedBy 
  @Column(name = "created_by")
  private String  createdBy;
  
  @LastModifiedBy
  @Column(name = "last_modified_by")
  private String  lastModifiedBy;
}
