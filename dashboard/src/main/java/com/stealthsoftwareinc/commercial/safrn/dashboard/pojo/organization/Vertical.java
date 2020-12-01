package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization;

import java.util.ArrayList;
import java.util.List;

import javax.persistence.*;
import javax.validation.constraints.NotNull;

import org.hibernate.annotations.LazyCollection;
import org.hibernate.annotations.LazyCollectionOption;
import org.hibernate.annotations.Type;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;
import lombok.Setter;

 
@Entity
@Getter
@Setter
@NotNull
@AllArgsConstructor
@Builder
public class Vertical { // this is same as "vertical" from wiki 
  @Id
  @GeneratedValue(strategy = GenerationType.SEQUENCE)
  private Long id;
  @Lob
  @Type(type="org.hibernate.type.BinaryType")
  private byte[] dbuid;
  private String verticalName;

  @ManyToOne(cascade = CascadeType.ALL)
  @JoinColumn(name = "session_id")
  private Session session;

  @LazyCollection(LazyCollectionOption.FALSE)
  @OneToMany(cascade = CascadeType.ALL, mappedBy = "vertical")
  List<VerticalColumn> columns;

  public Vertical() {
    columns = new ArrayList<>();
  }
  
  public Vertical(int size) {
    columns = new ArrayList<>(size);
    for (int i = 0; i < size; i++) {
      columns.add(new VerticalColumn());
    }

  }

}
