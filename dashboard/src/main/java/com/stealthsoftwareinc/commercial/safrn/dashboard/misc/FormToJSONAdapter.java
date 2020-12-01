package com.stealthsoftwareinc.commercial.safrn.dashboard.misc;

import java.util.ArrayList;
import java.util.List;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.EColumnType;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.EPeerType;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.EPeerValue;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.Vertical;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.VerticalColumn;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.json.JsonSessionPeers;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.json.JsonVerticalColumn;

public class FormToJSONAdapter { 

  private static List<JsonVerticalColumn> buildCategoryColumns(VerticalColumn verticalColumn) {
    List<JsonVerticalColumn> jsonColumns = new ArrayList<>();
    String[] categories = verticalColumn.getClb().split(",");
    for(String aCategory:categories) {
      JsonVerticalColumn aJsonVerticalColumn = JsonVerticalColumn.builder()
          .name(aCategory)
          .type(EColumnType.CATEGORICAL.toString().toLowerCase())
          .categorical(verticalColumn.getCname())
          .build();
      jsonColumns.add(aJsonVerticalColumn);
    }
    return jsonColumns;
  }
  
  private static JsonVerticalColumn buildIntegerColumn(VerticalColumn verticalColumn) {
    JsonVerticalColumn jsonColumn = JsonVerticalColumn.builder()
        .name(verticalColumn.getCname())
        .type(EColumnType.INTEGER.toString().toLowerCase())
        .signed(verticalColumn.getClb().toLowerCase())
        .number_of_bits(verticalColumn.getCub())
        .build();
    return jsonColumn;
  }
  
  private static JsonVerticalColumn buildRealColumn(VerticalColumn verticalColumn) {
    JsonVerticalColumn jsonColumn = JsonVerticalColumn.builder()
        .name(verticalColumn.getCname())
        .type(EColumnType.REAL.toString().toLowerCase())
        .whole_bits(verticalColumn.getClb())
        .fractional_bits(verticalColumn.getCub()) 
        .build();
    return jsonColumn;
  }
  
  public static  List<JsonVerticalColumn> convert(Vertical vertical) {
    List<JsonVerticalColumn> jsonColumns = new ArrayList<>();
    List<VerticalColumn> verticalColumns = vertical.getColumns();
    for(VerticalColumn verticalColumn:verticalColumns) {
      if(verticalColumn.getCtype().equalsIgnoreCase(EColumnType.CATEGORICAL.toString().toLowerCase())) {
        jsonColumns.addAll(buildCategoryColumns(verticalColumn));
      }
      else if (verticalColumn.getCtype().equalsIgnoreCase(EColumnType.INTEGER.toString().toLowerCase())) {
        jsonColumns.add(buildIntegerColumn(verticalColumn));
      }
      else if (verticalColumn.getCtype().equalsIgnoreCase(EColumnType.REAL.toString().toLowerCase())) {
        jsonColumns.add(buildRealColumn(verticalColumn));
      }
    }
    return jsonColumns;
  }
  
  public static JsonSessionPeers convert(String orgId, EPeerType ePeerType, EPeerValue ePeerValue, int value) {
    JsonSessionPeers config = new JsonSessionPeers();
    config.setOrganizationId(orgId);
    if(ePeerType == EPeerType.ANALYST) {
      config.setAnalyst(config.createAnalys(value));
    }
    else if(ePeerType == EPeerType.DATAOWNER) {
      config.setDataowner(config.createDataowner(value));
    }
    else if(ePeerType == EPeerType.DEALER) {
      config.setDealer(config.createDealer(value));
    }
    else if(ePeerType == EPeerType.RECIPIENT) {
      config.setRecipient(config.createRecipient(value));
    }
    return config;
  }
  
}
