package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo;

import java.util.ArrayList;
import java.util.List;
import javax.validation.constraints.NotNull;
import javax.validation.constraints.Size;
import static com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.IConstants.*;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.ESessionSection;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.Peer;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.Vertical;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.VerticalColumn;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;
import lombok.Setter;
import lombok.ToString;
import lombok.extern.slf4j.Slf4j;

@Slf4j
@Getter
@Setter
@NotNull
@ToString
@AllArgsConstructor
@Builder
public class CreateSessionForm {
  @Size(min =  minMinLoginIdLength, max = maxMaxLoginIdLength)
  private String sessionName;
  @Size(min =  minMinEmailLength, max = maxMaxEmailLength)
  private String aboutSession;
  
  private List<Vertical> verticals;
  private List<Peer> peers;

  public CreateSessionForm(int size) {
    verticals = new ArrayList<>(size);
    peers = new ArrayList<>(size);
    for(int i=0;i<size;i++) {
        verticals.add(new Vertical(size));
        peers.add(new Peer());
    }
  }
  
  public CreateSessionForm() {
    verticals = new ArrayList<>();
    peers  = new ArrayList<>();
  }
  
  public CreateSessionForm addRow(ESessionSection section) {
    if(section == ESessionSection.VERTICALS)
      verticals.add(new Vertical(1));
    else  if(section == ESessionSection.PEERS)
      peers.add(new Peer());
  
    return this;
  }
  
  public CreateSessionForm addRow(ESessionSection section, int verticalIndex) {
    // TODO Auto-generated method stub
    if(section == ESessionSection.COLUMNS)
     verticals.get(verticalIndex).getColumns().add(new VerticalColumn()); 
    else
      log.error("Unkown session section for add row.."+section.toString()+" for index = "+verticalIndex);
    
    return this;
  }
  
  public CreateSessionForm deleteRow(ESessionSection section,int index) {
    if(section == ESessionSection.VERTICALS)
      verticals.remove(index);
    else  if(section == ESessionSection.PEERS)
      peers.remove(index);
    
    return this;
  }

  public CreateSessionForm deleteRow(ESessionSection section, int colIndex, int verticalIndex) {
    if(section == ESessionSection.COLUMNS)
      verticals.get(verticalIndex).getColumns().remove(colIndex);
     else
       log.error("Unkown session section for del row.."+section.toString()+" for verticalIndex = "+verticalIndex+" for colIndex = "+colIndex);
     
     return this;
  }

 
}
