package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization;

import javax.persistence.CascadeType;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.validation.constraints.NotNull;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import lombok.ToString;

@Entity
@Getter
@Setter
@NotNull
@ToString
@AllArgsConstructor
@NoArgsConstructor
@Builder
public class VerticalColumn {
  @Id
  @GeneratedValue(strategy = GenerationType.IDENTITY)
  private Long id;
  @NotNull
  private String cname;
  private String ctype;
  private boolean cunique; 
  private String clb;
  private String cub;
  @ManyToOne(cascade = CascadeType.ALL)
  @JoinColumn(name = "vertical_id")
  private  Vertical vertical;
}
