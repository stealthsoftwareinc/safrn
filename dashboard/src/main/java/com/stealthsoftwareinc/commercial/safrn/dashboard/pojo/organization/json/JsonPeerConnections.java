package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.json;

import java.util.ArrayList;
import java.util.List;

import javax.validation.constraints.NotNull;

import lombok.AllArgsConstructor;
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
public class JsonPeerConnections {
  private List<JsonPeerConnectionConfig> peers = new ArrayList<>();
  
  public void add(JsonPeerConnectionConfig pConfig) {
    peers.add(pConfig);
  }
}
