package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.json;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

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
public class JsonSessionConfig {
  private String sessionId;
  private List<JsonVerticalConfig> lexicon = new ArrayList<>();
  private Set<JsonSessionPeers> peers;
  
  public  JsonSessionConfig(String id) {
    sessionId = id;
  }
  public void add(JsonVerticalConfig config) {
    lexicon.add(config);
  }
}
