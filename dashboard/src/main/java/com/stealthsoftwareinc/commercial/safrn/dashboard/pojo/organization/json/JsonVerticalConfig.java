package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.json;

import java.util.ArrayList;
import java.util.List;

import javax.validation.constraints.NotNull;

import com.fasterxml.jackson.annotation.JsonInclude;

import lombok.Getter;
import lombok.Setter;
import lombok.ToString;

@Getter
@Setter
@NotNull
@ToString
@JsonInclude(JsonInclude.Include.NON_NULL)
public class JsonVerticalConfig {
  private String verticalId;
  private List<JsonVerticalColumn> columns = new ArrayList<>(); 
  
  public JsonVerticalConfig(String verticalId) {
     this.verticalId = verticalId;
  }

  public void add(JsonVerticalColumn column) {
    columns.add(column);
  }
}
