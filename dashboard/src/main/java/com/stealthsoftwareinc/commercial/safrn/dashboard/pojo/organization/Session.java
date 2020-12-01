package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization;

import java.util.List;
import java.util.Objects;
import java.util.Set;

import javax.persistence.CascadeType;
import javax.persistence.CollectionTable;
import javax.persistence.ElementCollection;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.Lob;
import javax.persistence.OneToMany;
import javax.validation.constraints.NotNull;

import org.hibernate.annotations.Type;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.AbstractDBAudit;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.ESessionStatus; 

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;
import lombok.Setter;
import lombok.ToString;
import lombok.extern.slf4j.Slf4j;

@Entity
@Getter
@Setter
@NotNull
@ToString
@AllArgsConstructor
@Builder
@Slf4j
public class Session extends AbstractDBAudit {
  @Id
  @GeneratedValue(strategy = GenerationType.SEQUENCE)
  private Long id;
  @Lob
  @Type(type="org.hibernate.type.BinaryType")
  private byte[] dbuid;
  private String sessionName;
  private String aboutSession;
  private boolean isDraft;
  
  //@ElementCollection(targetClass = Vertical.class, fetch = FetchType.EAGER)
 // @CollectionTable(joinColumns = @JoinColumn(name = "session_id"))
  @OneToMany(fetch = FetchType.EAGER, cascade = CascadeType.ALL, mappedBy = "session")
  private List<Vertical> verticals;

  @ElementCollection(targetClass = Peer.class, fetch = FetchType.EAGER)
  @CollectionTable(joinColumns = @JoinColumn(name = "session_id"))
  private Set<Peer> peers;

  private ESessionStatus sessionStatus;
  private String initiatorOrganizationName;

  public Session() {
    //log.info("Testing.." + this.id);
  }

  @Override
  public int hashCode() {
      return Objects.hash(sessionName,aboutSession,initiatorOrganizationName);
  }
  
  @Override
  public boolean equals(Object object) {
    if (object == this) return true;
    if (!(object instanceof Session)) {
        return false;
    }
    Session otherActor = (Session) object;
    return sessionName.equalsIgnoreCase(otherActor.getSessionName()) &&
        aboutSession.equalsIgnoreCase(otherActor.getAboutSession()) &&
        initiatorOrganizationName.equalsIgnoreCase(otherActor.getInitiatorOrganizationName()) ; 
    
//    return Objects.equals(sessionName,otherActor.getSessionName()) &&
//        Objects.equals(aboutSession,otherActor.getAboutSession()) &&
//        Objects.equals(initiatorOrganizationName,otherActor.getInitiatorOrganizationName()) ;       
   }
}
