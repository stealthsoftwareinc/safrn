package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.json;

import javax.validation.constraints.NotNull;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.databind.annotation.JsonSerialize;

import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import lombok.ToString;

@Getter
@Setter
@NotNull
@ToString
@NoArgsConstructor
@JsonInclude(JsonInclude.Include.NON_NULL)
public class JsonSessionPeers {
  private String organizationId;
  private Analyst analyst;
  private Dataowner dataowner;
  private Dealer dealer;
  private Recipient recipient;
  
  public Analyst createAnalys(int count) {
    return new Analyst().setAllowedQueryCount(count);
  }
  
  public Dataowner createDataowner(int vertical) {
    return new Dataowner().setVertical(vertical);
  }
  
  public Dealer createDealer(int vertical) {
    return new Dealer();
  }
  
  public Recipient createRecipient(int vertical) {
    return new Recipient();
  }
}

@Getter
@ToString
class Analyst {
  private int allowedQueryCount;

  public  Analyst setAllowedQueryCount(int count) {
    this.allowedQueryCount = count;
    return this;
  }
}

@Getter
@Setter
@ToString
@JsonSerialize
class Dealer {
   
}

@Getter
@Setter
@ToString
@JsonSerialize
class Recipient {
   
}

@Getter
@Setter
@ToString
class Dataowner {
  private int vertical;

  public Dataowner setVertical(int vertical) {
    this.vertical = vertical;
    return this;
  }
}
