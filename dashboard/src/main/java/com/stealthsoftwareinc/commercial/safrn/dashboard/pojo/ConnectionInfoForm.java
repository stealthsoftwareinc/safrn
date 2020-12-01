package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo;

import javax.validation.constraints.NotNull;
import javax.validation.constraints.Size;

import static com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.IConstants.*;

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
public class ConnectionInfoForm {
  @Size(min =  minMinEmailLength,max =  maxMaxEmailLength)
  private String connectionName;
  @Size(min =  minMinPasswordLength)
  private String ipAddress;
  @Size(min = 4,max = 5)
  private String port;
}
