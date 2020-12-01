package com.stealthsoftwareinc.commercial.safrn.dashboard.pojo;

import java.util.HashSet;
import java.util.Set;
import javax.persistence.CascadeType;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.Lob;
import javax.persistence.ManyToMany;
import javax.persistence.Table;
import javax.validation.constraints.NotNull;

import org.hibernate.annotations.Type;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.AbstractDBAudit;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.Session;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import lombok.ToString;

@Entity  
@Getter
@Setter
@NotNull
@ToString
@NoArgsConstructor
@AllArgsConstructor
@Table(name = "users")
public class User extends AbstractDBAudit {
  @Id
  @GeneratedValue(strategy = GenerationType.IDENTITY)
  private Long id;
  @Lob
  @Type(type="org.hibernate.type.BinaryType")
  private byte[] profilePicture;
  private String orgId;
  private String loginId;
  private String password;
  private boolean enabled;
  @ManyToMany(cascade = CascadeType.MERGE)
  @JoinTable(name = "user_role", joinColumns = @JoinColumn(name = "user_id"), inverseJoinColumns = @JoinColumn(name = "role_id"))
  private Set<Role> roles;
 
  @ManyToMany(cascade = CascadeType.MERGE)
  @JoinTable(name = "user_sessions", joinColumns = @JoinColumn(name = "user_id"), inverseJoinColumns = @JoinColumn(name = "session_id"))
  private Set<Session> sessions;
  
  public User(String orgId,String loginId, String password, boolean enabled, Set<Role> roles) {
    this.orgId = orgId;
    this.loginId = loginId;
    this.password = password;
    this.enabled = enabled;
    this.roles = roles;
    sessions = new HashSet<Session>();
  }
  
  public void addSession(Session session) {
    sessions.add(session);
  }
}
