package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.json;

import javax.validation.constraints.NotNull;

import com.fasterxml.jackson.annotation.JsonInclude;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import lombok.ToString;

 
@Getter
@Setter
@NotNull
@ToString
@NoArgsConstructor
@AllArgsConstructor
@Builder 
@JsonInclude(JsonInclude.Include.NON_NULL)
public class JsonVerticalColumn { 
  private String name;
  private String type;
  //private boolean unique; 
  private String signed;
  private String number_of_bits;
  private String whole_bits;
  private String fractional_bits;
  private String categorical;
}
