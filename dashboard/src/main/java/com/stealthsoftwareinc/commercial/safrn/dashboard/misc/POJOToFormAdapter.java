package com.stealthsoftwareinc.commercial.safrn.dashboard.misc;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import java.util.Set;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.EInvitationStatus;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.ConnectionInfoForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.ContactInfoForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.CreateSessionForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.ProfileForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.ConnectionInfo;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.ContactInfo;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.Peer;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.Session;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.SessionForm;

public class POJOToFormAdapter {
  public static ContactInfoForm convert(ContactInfo  contactInfo) {
    return ContactInfoForm.builder()
        .about(contactInfo.getAbout())
        .phone(contactInfo.getPhone())
        .address(contactInfo.getAddress())
        .build();
  }
  
  public static ConnectionInfoForm convert(ConnectionInfo connectionInfo) {
    if(connectionInfo == null)
      return new ConnectionInfoForm();
    else
      return ConnectionInfoForm.builder()
        .connectionName(connectionInfo.getConnectionName())
        .ipAddress(connectionInfo.getIpAddress())
        .port(connectionInfo.getPort().toString())
        .build();
  }

  public static ProfileForm convert(ContactInfo contactInfo, ConnectionInfo connectionInfo) {
    return ProfileForm.builder()
        .connectionName(connectionInfo.getConnectionName())
        .ipAddress(connectionInfo.getIpAddress())
        .port(connectionInfo.getPort().toString())
        .about(contactInfo.getAbout())
        .phone(contactInfo.getPhone())
        .address(contactInfo.getAddress())
        .build();
  }
  
  public static List<CreateSessionForm> convert(List<Session> sessions){
    List<CreateSessionForm> sessionForms = new ArrayList<>();
    for(Session session:sessions) {
      CreateSessionForm sessionForm = CreateSessionForm.builder()
          .sessionName(session.getSessionName())
          .aboutSession(session.getAboutSession())
          .verticals(session.getVerticals())
          .peers(new ArrayList<>(session.getPeers())) 
          .build();
      sessionForms.add(sessionForm);
    }
    return sessionForms;
  }
  
  public static  SessionForm convert( Session session,String myOrgName,boolean isDraft) throws Exception {
    EInvitationStatus invitationStatus = getMyInvitationStatus(session,myOrgName);
    SessionForm sessionForm = SessionForm.builder()
        .id(session.getId())
        .verticals(session.getVerticals())
        .peers(session.getPeers())
        .sessionStatus(session.getSessionStatus())
        .initiatorOrganizationName(session.getInitiatorOrganizationName())
        .invitationStatus(invitationStatus)
        .sessionName(session.getSessionName())
        .aboutSession(session.getAboutSession())
        .isDraft(session.isDraft())
        .build();
    return sessionForm;
  }
  
  public static List<SessionForm> convert(List<Session> sessions,String myOrgName,boolean isDraft) throws Exception {
    List<SessionForm> sessionForms = new ArrayList<>();
    for(Session session:sessions) {
     // EInvitationStatus invitationStatus = getMyInvitationStatus(session,myOrgName);
      SessionForm sessionForm = SessionForm.builder()
          .sessionName(session.getSessionName())
          .id(session.getId())
          .sessionStatus(session.getSessionStatus())
          .initiatorOrganizationName(session.getInitiatorOrganizationName())
          .aboutSession(session.getAboutSession())
          .isDraft(session.isDraft())
          //.invitationStatus(invitationStatus)
          .build();
      if(!isDraft) {
        EInvitationStatus invitationStatus = getMyInvitationStatus(session,myOrgName);
        sessionForm.setInvitationStatus(invitationStatus);
      }
      sessionForms.add(sessionForm);
    }
    sessionForms.sort(Comparator.comparing(SessionForm::getId));
    return sessionForms;
  }
  
  private static EInvitationStatus getMyInvitationStatus(Session session, String myOrgName) throws Exception {
    Set<Peer> peers = session.getPeers();
    for(Peer peer:peers) {
      if(peer.getOrganizationName().equalsIgnoreCase(myOrgName))
        return peer.getStatus();
    } 
    throw new Exception("Unknown organization name "+myOrgName);
  }
}
