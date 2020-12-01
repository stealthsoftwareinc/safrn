package com.stealthsoftwareinc.commercial.safrn.dashboard.misc;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.lang3.RandomStringUtils;
import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.crypto.codec.Hex;

import com.google.common.collect.BiMap;
import com.google.common.collect.HashBiMap;
import com.google.common.primitives.Longs;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.EInvitationStatus;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.ESessionStatus;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.OrgInfo;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.User;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.ParticipantStatus;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.Peer;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.Session;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.Vertical;

import lombok.extern.slf4j.Slf4j;

@Slf4j
public class Utils {
  public static String getUserLoginId() {
    Object userDetails = SecurityContextHolder.getContext().getAuthentication().getPrincipal();
    String userLoginId = "TBD";
    if (userDetails instanceof UserDetails)
      userLoginId = ((UserDetails) userDetails).getUsername();
    else
      userLoginId = userDetails.toString();

    return userLoginId;
  }

  public static String getUserLoginPassword(int length) {
    return RandomStringUtils.randomAlphanumeric(length).toUpperCase();
  }

  public static boolean hasAuthority(String role) {
    boolean flag = false;
    Collection<? extends GrantedAuthority> authorities = SecurityContextHolder.getContext().getAuthentication()
        .getAuthorities();
    for (GrantedAuthority authority : authorities) {
      if (role.equalsIgnoreCase(authority.toString())) {
        flag = true;
        break;
      }
    }
    return flag;
  }
  
  public static String getFormattedOutput(List<OrgInfo> orgs) {
    String value;
    if(orgs.size() == 0)
      value = "No organizations found ...";
    else {
      value = "<table><tr><th>Organization Id</th><th>Organization Name</th><th>Email Address</th></tr>";
      for (OrgInfo org : orgs) {
        value += "<tr><td>"+org.getOrgId()+"</td><td>"+org.getOrgName() + "</td><td>" +org.getOrgEmail()+"</td></tr>";
      }
      value += "</table>";
    }
    return value;
  }
  
  public static BiMap<String,Long> getOrgBiMap(List<OrgInfo> orgs){ //
    BiMap<String,Long> biMap = HashBiMap.create(); //<name,id>
    for(OrgInfo org:orgs) {
      biMap.put(org.getOrgName(),org.getOrgId());
    }
    return biMap;
  }


  public static Map<String,List<ParticipantStatus>> mySessionParticipants(User user) {
    Map<String,List<ParticipantStatus>> participantStatus = new HashMap<>();
    Set<Session> mySessions = user.getSessions();
    for(Session session:mySessions) {
      for(Peer peer:session.getPeers()) {
        List<ParticipantStatus> sessionStatus = new ArrayList<>();
        sessionStatus.add(ParticipantStatus.builder()
            .orgName(peer.getOrganizationName())
            .status(peer.getStatus())
            .build());
        participantStatus.put(session.getSessionName(), sessionStatus);
      } 
    }
    return participantStatus;
  }
  
  public static List<Session> loadMySessions(User user, List<Session> sessions,List<OrgInfo> orgs,List<ESessionStatus> sessionStatuses) {
    List<Session> mySessions = new ArrayList<>();
    BiMap<String,Long> orgMap = getOrgBiMap(orgs);
    String myOrgName = orgMap.inverse().get(Long.parseLong(user.getOrgId()));
    for(Session aSession:sessions) {
      for(Peer peer:aSession.getPeers()) {
        if(peer.getOrganizationName().equalsIgnoreCase(myOrgName)) {
          if(sessionStatuses == null ||
              sessionStatuses.contains(aSession.getSessionStatus()))
            mySessions.add(aSession);
          break;
        }
      }
    }
    return mySessions;
  }

  public static List<Session> loadMyPendingSessions(User user, List<Session> sessions, List<OrgInfo> orgs, List<ESessionStatus> sessionStatuses) {
    List<Session> mySessions = new ArrayList<>();
    BiMap<String,Long> orgMap = getOrgBiMap(orgs);
    String myOrgName = orgMap.inverse().get(Long.parseLong(user.getOrgId()));
    for(Session aSession:sessions) {
      for(Peer peer:aSession.getPeers()) {
        if(peer.getOrganizationName().equalsIgnoreCase(myOrgName)  && peer.getStatus()==EInvitationStatus.PENDING && sessionStatuses.contains(aSession.getSessionStatus())){
          mySessions.add(aSession);
          break;
        }
      }
    }
    return mySessions;
  }

  public static List<Session> loadPeerPendingSessions(User user, List<Session> sessions, List<OrgInfo> orgs, List<ESessionStatus> sessionStatuses) {
    List<Session> mySessions = new ArrayList<>();
    BiMap<String,Long> orgMap = getOrgBiMap(orgs);
    String myOrgName = orgMap.inverse().get(Long.parseLong(user.getOrgId()));
    for(Session aSession:sessions) {
      for(Peer peer:aSession.getPeers()) {
        if(!peer.getOrganizationName().equalsIgnoreCase(myOrgName)  && peer.getStatus()==EInvitationStatus.PENDING && sessionStatuses.contains(aSession.getSessionStatus())){
          mySessions.add(aSession);
          break;
        }
      }
    }
    return mySessions;
  }
 
  public static HashSet<Peer> convertListToSet(List<Peer> peers,EInvitationStatus invitationStatus) {
    HashSet<Peer> peerSet = new HashSet<Peer>(peers);//todo: make sure the sizes match
    for(Peer peer:peerSet)
      peer.setStatus(invitationStatus);
    log.info("size of input list = "+peers.size()+" and size of set = "+peerSet.size());
    return peerSet;
  }

  @SuppressWarnings("deprecation")
  public static Long setVerticalIdsDeprecated(Session aSession, Long currentMaxId) {
    List<Vertical> verticals = aSession.getVerticals();
    for(Vertical vertical:verticals) {
     // vertical.setId(new Long((++currentMaxId).toString()));
    }
    return currentMaxId;
  }
  
  public static String convertToHex(Long dbuid) {
    byte[] idInBytes = Longs.toByteArray(dbuid);
    char[] idInString = Hex.encode(idInBytes);
    return String.copyValueOf(idInString);
  }
  
  public static String convertToHex(byte[] dbuid) {
    final StringBuilder builder = new StringBuilder();
    for(byte b : dbuid) {
        builder.append(String.format("%02x", b & 0xff));
    }
    return "0x"+builder.toString().toUpperCase();
}

}
