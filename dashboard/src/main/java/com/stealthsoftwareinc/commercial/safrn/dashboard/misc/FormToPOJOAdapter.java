package com.stealthsoftwareinc.commercial.safrn.dashboard.misc;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;

import javax.validation.Valid;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.EInvitationStatus;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.ESessionStatus;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.ConnectionInfoForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.ContactInfoForm; 
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.CreateSessionForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.OrgInfo;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.admin.CreateOrgForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.ConnectionInfo;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.ContactInfo;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.Peer;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.Session;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.Vertical;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.VerticalColumn;

public class FormToPOJOAdapter {
  public static OrgInfo convert(CreateOrgForm createOrgForm) {
    return OrgInfo.builder()
        .orgEmail(createOrgForm.getOrgEmail())
        .orgName(createOrgForm.getOrgName())
        .active(true)
        .build();
  }
  
  public static ContactInfo convert(ContactInfoForm contactInfoForm, String loginId, String orgId) {
    return ContactInfo.builder()
        .about(contactInfoForm.getAbout())
        .phone(contactInfoForm.getPhone())
        .address(contactInfoForm.getAddress())
        .loginId(loginId)
        .orgId(orgId)
        .build();
  }
  
  public static ConnectionInfo convert(ConnectionInfoForm connectionInfoForm, String loginId, String orgId) {
    return ConnectionInfo.builder()
        .connectionName(connectionInfoForm.getConnectionName())
        .ipAddress(connectionInfoForm.getIpAddress())
        .port(Integer.parseInt(connectionInfoForm.getPort()))
        .loginId(loginId)
        .orgId(orgId)
        .build();
  }

  public static Session convert(CreateSessionForm createSessionForm, String myOrgName) { //todo: savedraft: verticalid
    Session aSession = Session.builder()
         .sessionName(createSessionForm.getSessionName())
         .sessionStatus(ESessionStatus.CREATED)
         .initiatorOrganizationName(myOrgName)
         .verticals(convert(createSessionForm.getVerticals()))
         .aboutSession(createSessionForm.getAboutSession())
         .isDraft(false)
         .build();
    aSession = setForeignKeys(aSession);
    HashSet<Peer> peers = Utils.convertListToSet(createSessionForm.getPeers(),EInvitationStatus.PENDING);
    aSession.setPeers(peers);
    return aSession;
  }
  
  private static Session setForeignKeys(Session aSession) {
    List<Vertical> verticals = aSession.getVerticals();
    for(Vertical vertical:verticals) {
      vertical.setSession(aSession);
      List<VerticalColumn> columns = vertical.getColumns();
      for(VerticalColumn column:columns) {
        column.setVertical(vertical);
      }
    }
    return aSession;
  }

  public static Session convert(@Valid CreateSessionForm createSessionForm, Session draftSession, String myOrgName) {
    draftSession.setDraft(false);
    draftSession.setSessionName(createSessionForm.getSessionName());
    draftSession.setAboutSession(createSessionForm.getAboutSession());
    draftSession.setInitiatorOrganizationName(myOrgName); 
    draftSession.setSessionStatus(ESessionStatus.CREATED);
    draftSession.setPeers(Utils.convertListToSet(createSessionForm.getPeers(),EInvitationStatus.PENDING));
    draftSession.setVerticals(createSessionForm.getVerticals());  
    draftSession = setForeignKeys(draftSession);
    return draftSession;
  }
  
  private static List<Vertical> convert(List<Vertical> uiVerticals){//we need this so that ID will be created
    List<Vertical> verticals = new ArrayList<>();
    int index =0;
    for(Vertical uiVertical:uiVerticals) { 
      Vertical dbVertical = Vertical.builder()
          .columns(uiVertical.getColumns())
          .verticalName(uiVertical.getVerticalName())
          .build();
      
//      if(dbVertical.getId() == null)
//        dbVertical.setId(0l); //generates detached object exception
      verticals.add(dbVertical);
    }
    return verticals;
  }
}
