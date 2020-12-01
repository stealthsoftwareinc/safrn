package com.stealthsoftwareinc.commercial.safrn.dashboard.controller;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.time.LocalDateTime;
import java.util.Arrays; 
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.Set;
import java.util.TreeSet;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.stream.Collectors;

import javax.validation.Valid;
import javax.validation.constraints.NotNull;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.core.io.ByteArrayResource;
import org.springframework.http.HttpHeaders;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.stereotype.Controller;
import org.springframework.validation.BindingResult;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.multipart.MultipartFile;
import org.springframework.web.servlet.ModelAndView;
import org.springframework.web.servlet.config.annotation.WebMvcConfigurer;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.google.common.collect.BiMap;
import com.google.common.primitives.Longs;
import com.stealthsoftwareinc.commercial.safrn.dashboard.config.AppConfig;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.EConfigDownload;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.EInvitationStatus;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.ESessionAuditType;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.ESessionSection;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.ESessionStatus;
import com.stealthsoftwareinc.commercial.safrn.dashboard.misc.DBUID;
import com.stealthsoftwareinc.commercial.safrn.dashboard.misc.FormToJSONAdapter;
import com.stealthsoftwareinc.commercial.safrn.dashboard.misc.FormToPOJOAdapter;
import com.stealthsoftwareinc.commercial.safrn.dashboard.misc.POJOToFormAdapter;
import com.stealthsoftwareinc.commercial.safrn.dashboard.misc.StreamResource;
import com.stealthsoftwareinc.commercial.safrn.dashboard.misc.Utils;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.AdminLoginForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.ConnectionInfoForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.ContactInfoForm; 
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.CreateSessionForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.OrgInfo;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.PasswordForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.SessionAudit;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.SessionStatusForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.User;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.ConnectionInfo;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.ContactInfo;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.Peer;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.Session;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.SessionForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.Vertical;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.VerticalId;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.json.JsonSessionPeers;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.json.JsonVerticalConfig;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.json.JsonPeerConnectionConfig;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.json.JsonPeerConnections;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.json.JsonSessionConfig;
import com.stealthsoftwareinc.commercial.safrn.dashboard.security.Encoder;
import com.stealthsoftwareinc.commercial.safrn.dashboard.services.ConnectionInfoService;
import com.stealthsoftwareinc.commercial.safrn.dashboard.services.ContactInfoService;
import com.stealthsoftwareinc.commercial.safrn.dashboard.services.DashboardUserService;
import com.stealthsoftwareinc.commercial.safrn.dashboard.services.OrgInfoService;
import com.stealthsoftwareinc.commercial.safrn.dashboard.services.SessionAuditService;
import com.stealthsoftwareinc.commercial.safrn.dashboard.services.SessionInfoService;
import com.stealthsoftwareinc.commercial.safrn.dashboard.services.VerticalIdService;

import lombok.extern.slf4j.Slf4j;
 
@Slf4j
@Controller
@RequestMapping("/organization")
public class OrganizationController implements WebMvcConfigurer {

  @Autowired
  Encoder encoder;
  @Autowired
  private DashboardUserService userService;
  @Autowired
  private ContactInfoService contactInfoService;
  @Autowired
  private ConnectionInfoService connectionInfoService;
  @Autowired
  private OrgInfoService orgInfoService;
  @Autowired
  private SessionInfoService sessionInfoService; 
  @Autowired
  private SessionAuditService sessionAuditService;
  @Autowired
  private VerticalIdService verticalIdService; 
  private DBUID verticalDBUID ; //todo: create a service to hold prev values
  private DBUID sessionDBUID ; 
  @Autowired//autowired   constructor
  public OrganizationController(AppConfig appConfig) {
    log.info("constructor..OrganizationController..");
    verticalDBUID = new DBUID(appConfig.getVerticalDbuidLength());
    sessionDBUID = new DBUID(appConfig.getVerticalDbuidLength());
  }
  
  @GetMapping("/orglogout")
  public String adminLogout(AdminLoginForm adminLoginForm) {
    log.info("orglogout logout..");
    SessionAudit sessionAudit = SessionAudit.builder()
        .userId(Utils.getUserLoginId())
        .auditType(ESessionAuditType.LOGOUT)
        .timeStamp(LocalDateTime.now())
        .build();
    sessionAuditService.save(sessionAudit);
    return "organization/login_form";
  }

  @GetMapping("/profile")
  public ModelAndView getProfile() {
    log.info("updatepassword get");
    ModelAndView modelAndView = new ModelAndView();
    Authentication auth = SecurityContextHolder.getContext().getAuthentication();
    org.springframework.security.core.userdetails.User springUser = (org.springframework.security.core.userdetails.User)auth.getPrincipal();
    User dashboardUser = userService.findByLoginId(springUser.getUsername());
    ContactInfo contactInfo = contactInfoService.findByLoginId(dashboardUser.getLoginId());
    ConnectionInfo connectionInfo = connectionInfoService.findByLoginId(dashboardUser.getLoginId());
    ContactInfoForm persistedContactInfoForm; 
    ConnectionInfoForm persistedConnectionInfoForm; 
    if(contactInfo == null)
      persistedContactInfoForm = new ContactInfoForm();
    else
      persistedContactInfoForm = POJOToFormAdapter.convert(contactInfo);
    if(connectionInfo == null)
      persistedConnectionInfoForm = new ConnectionInfoForm();
    else
      persistedConnectionInfoForm = POJOToFormAdapter.convert(connectionInfo);
    modelAndView.addObject("passwordForm", new PasswordForm("*******","*******"));
    modelAndView.addObject("contactInfoForm",persistedContactInfoForm);
    modelAndView.addObject("connectionInfoForm",persistedConnectionInfoForm);
    modelAndView.setViewName("organization/orgProfile"); 
    return modelAndView;
  } 
  
  @GetMapping("/viewsession")
  public ModelAndView viewSession() throws Exception {
    log.info("viewSession get");
    ModelAndView modelAndView = new ModelAndView();
    List<OrgInfo> orgs = orgInfoService.findAll();
    User user = userService.findByLoginId(Utils.getUserLoginId());
    BiMap<String,Long> orgMapping = Utils.getOrgBiMap(orgs);
    String myOrgName = orgMapping.inverse().get(Long.parseLong(user.getOrgId()));
    
    List<ESessionStatus> sessionStatuses = Arrays.asList(ESessionStatus.ACCEPTED);
    List<Session> acceptedSessions = Utils.loadMySessions(user,sessionInfoService.findAllSessions(),orgs,sessionStatuses);  
    List <SessionForm> acceptedSessionForms = POJOToFormAdapter.convert(acceptedSessions,myOrgName,false);
    //acceptedSessionForms.sort(Comparator.comparing(SessionForm::getId));
    
    sessionStatuses = Arrays.asList(ESessionStatus.DECLINED);
    List<Session> declinedSessions = Utils.loadMySessions(user,sessionInfoService.findAllSessions(),orgs,sessionStatuses);  
    List <SessionForm> declinedSessionsForms = POJOToFormAdapter.convert(declinedSessions,myOrgName,false);
    
    sessionStatuses = Arrays.asList(ESessionStatus.CREATED);
    List<Session> pendingSessions = Utils.loadMySessions(user,sessionInfoService.findAllSessions(),orgs,sessionStatuses);  
    List<Session> pendingMyResposeSessions = Utils.loadMyPendingSessions(user,pendingSessions,orgs,sessionStatuses); 
    List<Session> pendingPeerResponseSessions = Utils.loadPeerPendingSessions(user,pendingSessions,orgs,sessionStatuses); 
    pendingPeerResponseSessions.removeAll(pendingMyResposeSessions);
    List <SessionForm> pendingSessionForms = POJOToFormAdapter.convert(pendingSessions,myOrgName,false);
    List <SessionForm> pendingMyResposeSessionForms = POJOToFormAdapter.convert(pendingMyResposeSessions,myOrgName,false);
    List <SessionForm> pendingPeerResponseSessionForms = POJOToFormAdapter.convert(pendingPeerResponseSessions,myOrgName,false);
    
     
    
    modelAndView.addObject("acceptedSessions",acceptedSessionForms);
    modelAndView.addObject("declinedSessions",declinedSessionsForms);
    modelAndView.addObject("pendingSessions",pendingSessionForms);
    modelAndView.addObject("pendingMyResposeSessions",pendingMyResposeSessionForms);
    modelAndView.addObject("pendingPeerResponseSessions",pendingPeerResponseSessionForms);
    modelAndView.setViewName("organization/viewsession_form");
    return modelAndView; 
  } 
  
  @GetMapping("/searchsession")
  public ModelAndView searchSession(@NotNull @RequestParam(required = true) String sessionName/*, BindingResult bindingResult*/) throws Exception{
    log.info("searchsession get");
    ModelAndView modelAndView = new ModelAndView();
    List<OrgInfo> orgs = orgInfoService.findAll();
    User user = userService.findByLoginId(Utils.getUserLoginId());
    BiMap<String,Long> orgMapping = Utils.getOrgBiMap(orgs);
    String myOrgName = orgMapping.inverse().get(Long.parseLong(user.getOrgId()));
    Session aSession = sessionInfoService.findSessionBySessionName(sessionName);
    SessionForm sessionForm = POJOToFormAdapter.convert(aSession,myOrgName,false);
    
    if(aSession == null)
      modelAndView.addObject("successMessage", "no session found with name:"+sessionName);
    else {
      ConnectionInfo connectionInfo = connectionInfoService.findByLoginId(user.getLoginId());
      modelAndView.addObject("connectionInfo", connectionInfo);
      modelAndView.addObject("sessionDetails", sessionForm);
      modelAndView.addObject("myOrgName", myOrgName);
    }

    modelAndView.setViewName("organization/searchsession_form");
    return modelAndView;
  }
  
  @GetMapping("/addrow")
  public ModelAndView addRow(@RequestParam(required = true) ESessionSection sessionSection,CreateSessionForm createSessionForm,@RequestParam(required = false) String verticalIndex) {
    log.info("addRow get");
    ModelAndView modelAndView = new ModelAndView();
    CreateSessionForm aForm;
    if(verticalIndex == null)
      aForm = createSessionForm.addRow(sessionSection);
    else
      aForm = createSessionForm.addRow(sessionSection,Integer.parseInt(verticalIndex));
    modelAndView.addObject("createSessionForm",aForm);
    modelAndView.addObject("orgInfos",orgInfoService.findAll());
    modelAndView.setViewName("organization/createsession_form");
    return modelAndView; 
  }  
  
  @GetMapping("/deleterow")
  public ModelAndView deleteRow(@RequestParam(required = true) ESessionSection sessionSection,CreateSessionForm createSessionForm,int index,@RequestParam(required = false) String verticalIndex) {
    log.info("deleterow get");
    ModelAndView modelAndView = new ModelAndView();
    CreateSessionForm aForm;
    if(verticalIndex == null)
      aForm = createSessionForm.deleteRow(sessionSection,index);
    else
      aForm = createSessionForm.deleteRow(sessionSection,index,Integer.parseInt(verticalIndex));
    
    modelAndView.addObject("createSessionForm",aForm);
    modelAndView.addObject("orgInfos",orgInfoService.findAll());
    modelAndView.setViewName("organization/createsession_form");
    return modelAndView; 
  }
  
  @PostMapping("/savesession")
  @ResponseBody 
  public Map<String,String> saveDraftSessionPosted(@Valid CreateSessionForm createSessionForm, BindingResult bindingResult) throws Exception {
    //test cases:i)if I have taken as regular session(ignore other orgs which will be tested in createsession), throw error; ii)if it is my org + sessionname exists as draft, then overwrite existing draft)
    log.info("savesession post " + createSessionForm.toString());
    Map<String,String> response = new HashMap<>();
    ModelAndView modelAndView = new ModelAndView(); 
    modelAndView.addObject("createSessionForm",createSessionForm);
    modelAndView.addObject("orgInfos",orgInfoService.findAll());
    modelAndView.setViewName("organization/createsession_form");
    
    User user = userService.findByLoginId(Utils.getUserLoginId());
    BiMap<String,Long> orgMapping = Utils.getOrgBiMap(orgInfoService.findAll());
    String myOrgName = orgMapping.inverse().get(Long.parseLong(user.getOrgId()));
    Session draftSession = sessionInfoService.findDraftBySessionNameAndInitiatorOrganizationName(createSessionForm.getSessionName(), myOrgName);
    Session aSession = sessionInfoService.findSessionBySessionNameAndInitiatorOrganizationName(createSessionForm.getSessionName(), myOrgName);
    if(draftSession!=null) {//copy input form into draft(merge)
      log.info("copying sessionform into existing draftsession. \nDraft= "+draftSession.toString()+"\nsessionform= "+createSessionForm.toString());
      this.delDraftSession(draftSession.getSessionName());
      draftSession = new Session();
      draftSession = FormToPOJOAdapter.convert(createSessionForm,draftSession,myOrgName);
      this.setDBUIDs(draftSession);
      draftSession.setDraft(true);
      sessionInfoService.save(draftSession); 
      response.put("success","true");
      response.put("message","empty");
      return response;//modelAndView; 
    }
    else if(draftSession==null && aSession!=null) {//draft name can't be same as existing session name
      log.error("draft name can't be same as existing session name. \nsessionform= "+createSessionForm.toString());
      bindingResult.rejectValue("sessionName", "Duplicate.createsessionform.SessionName", "...SessionName already exists...");
      response.put("success","false");
      response.put("message","SessionName already exists..chose a diffferent session name");
      return response;
    }
    //it is OK to have drafts with same sessionname but from different org (so we ignore to check this condition). So just create a new entry
    aSession = FormToPOJOAdapter.convert(createSessionForm, myOrgName);
    setDBUIDs(aSession);
    aSession.setDraft(true);      
    sessionInfoService.save(aSession); 
    response.put("success","true");
    response.put("message","empty");
    return response;
  }
  
  
  @PostMapping("/deldraftsession")
  public ModelAndView delDraftSession(@RequestParam(required = true) String sessionName) {
    User user = userService.findByLoginId(Utils.getUserLoginId());
    BiMap<String,Long> orgMapping = Utils.getOrgBiMap(orgInfoService.findAll());
    String myOrgName = orgMapping.inverse().get(Long.parseLong(user.getOrgId()));
    Session draftSession = sessionInfoService.findDraftBySessionNameAndInitiatorOrganizationName(sessionName, myOrgName);
    if(draftSession != null) {
      sessionInfoService.deleteDraftBySessionNameAndInitiatorOrganizationName(sessionName, myOrgName);
      log.info("Deleted draft:"+sessionName);
    }
    return this.getDraftSession(null);
  }
  
  @GetMapping("/countdraftsessions")
  @ResponseBody
  public Integer countDraftSessions() {
    User user = userService.findByLoginId(Utils.getUserLoginId());
    BiMap<String,Long> orgMapping = Utils.getOrgBiMap(orgInfoService.findAll());
    String myOrgName = orgMapping.inverse().get(Long.parseLong(user.getOrgId()));
    List<Session> drafts = sessionInfoService.findDraftsByInitiatorOrganizationName(myOrgName);
    return drafts.size();
  }
  
  @GetMapping("/getdraftsession")
  public ModelAndView getDraftSession(@RequestParam(required = false) String sessionName) {
    log.info("getdraftsession ");
    ModelAndView modelAndView = new ModelAndView();
    if (sessionName == null) {//return all drafts
      User user = userService.findByLoginId(Utils.getUserLoginId());
      BiMap<String, Long> orgMapping = Utils.getOrgBiMap(orgInfoService.findAll());
      String myOrgName = orgMapping.inverse().get(Long.parseLong(user.getOrgId()));
      List<Session> sessions = sessionInfoService.findDraftsByInitiatorOrganizationName(myOrgName);
      List<CreateSessionForm> sessionForms = POJOToFormAdapter.convert(sessions);
      modelAndView.addObject("draftSessionForm", sessionForms);
      modelAndView.setViewName("organization/draftsession_form");
    } else {
      User user = userService.findByLoginId(Utils.getUserLoginId());
      BiMap<String,Long> orgMapping = Utils.getOrgBiMap(orgInfoService.findAll());
      String myOrgName = orgMapping.inverse().get(Long.parseLong(user.getOrgId()));
      Session aSession = sessionInfoService.findDraftBySessionNameAndInitiatorOrganizationName(sessionName,myOrgName);
      List<Session> sessionList = Arrays.asList(aSession);
      List<CreateSessionForm> createSessionList = POJOToFormAdapter.convert(sessionList);
      //CreateSessionForm aForm = new CreateSessionForm(2);
      modelAndView.addObject("createSessionForm", createSessionList.get(0));
      modelAndView.addObject("orgInfos", orgInfoService.findAll());
      modelAndView.setViewName("organization/createsession_form");
    }
    return modelAndView;
  }

  @GetMapping("/createsession")
  public ModelAndView createSession(@RequestParam(required = false) String sessionName,
      CreateSessionForm createSessionForm) throws Exception {
    log.info("createsession get");
    ModelAndView modelAndView = new ModelAndView();
    if (sessionName == null) {
      CreateSessionForm aForm = new CreateSessionForm(1);
      modelAndView.addObject("createSessionForm", aForm);
      modelAndView.addObject("orgInfos", orgInfoService.findAll());
    } else {
      ModelAndView someModelAndView = this.searchSession(sessionName); 
      SessionForm sessionForm = (SessionForm) someModelAndView.getModel().get("sessionDetails"); 
      modelAndView.addObject("createSessionForm", sessionForm);
      modelAndView.addObject("orgInfos", orgInfoService.findAll());
    }
    modelAndView.setViewName("organization/createsession_form");
    return modelAndView;
  }
  
  @PostMapping("/createsession")
  public ModelAndView createSessionPosted(@Valid CreateSessionForm createSessionForm, BindingResult bindingResult) throws Exception{//todo: bug:sessions occasionally not going to DB
    log.info("createsession post "+createSessionForm.toString());
    ModelAndView modelAndView = new ModelAndView();  
    Session aSession = sessionInfoService.findSessionBySessionName(createSessionForm.getSessionName());
    if(aSession !=null)
      bindingResult.rejectValue("sessionName", "Duplicate.createsessionform.SessionName", "...SessionName already exists...");
    List<Peer> formPeers  = createSessionForm.getPeers();
    BiMap<String,Long> orgMapping = Utils.getOrgBiMap(orgInfoService.findAll());
    for(Peer peer:formPeers) {
      if(orgMapping.get(peer.getOrganizationName()) == null)
        bindingResult.rejectValue("sessionName","some random stuff","Organization does not exist.."+peer.getOrganizationName());//todo change the first param after html is updated
    }
    if (bindingResult.hasErrors()) {
      modelAndView.setViewName("organization/createsession_form"); 
      modelAndView.addObject("orgInfos",orgInfoService.findAll());
    } 
    else { 
      User user = userService.findByLoginId(Utils.getUserLoginId());
      String myOrgName = orgMapping.inverse().get(Long.parseLong(user.getOrgId()));
      Session draftSession = sessionInfoService.findDraftBySessionName(createSessionForm.getSessionName());
      log.info("createsession post ...else:draftSession =");//+(draftSession == null?"null":draftSession.toString()));
      if(draftSession == null)
        aSession = FormToPOJOAdapter.convert(createSessionForm,myOrgName);//no draft exists; so just get the pojo
      else {
        this.delDraftSession(draftSession.getSessionName());
        draftSession = null;
        Session tempSession = new Session();
        aSession = FormToPOJOAdapter.convert(createSessionForm,tempSession,myOrgName);//merge session and draft (control comes here in 2 ways: either draft->session; or session->draft)
      }

//      Optional<VerticalId> currentMaxId = verticalIdService.findById(1);//  this table has only one entry with primary key as '1' to hold most recent max value of verticalId
//      VerticalId aVerticalId;
//      
//      if(currentMaxId.isPresent()) {
//        aVerticalId = currentMaxId.get();
//      }
//      else { 
//        aVerticalId = new VerticalId();
//        ByteBuffer byteBuffer = ByteBuffer.allocate(8);
//        aVerticalId.setRecentId(byteBuffer.putLong(1).array());
//      }
//      byte[] maxIdBytes = aVerticalId.getRecentId();
//      Long maxId = Longs.fromByteArray(maxIdBytes);
//      maxId = Utils.setVerticalIds(aSession,maxId);
//      aVerticalId.setRecentId(Longs.toByteArray(maxId));
//      verticalIdService.save(aVerticalId);
      
      setDBUIDs(aSession);
      sessionInfoService.save(aSession); 
      CopyOnWriteArrayList<Peer> peers = new CopyOnWriteArrayList<>(aSession.getPeers());
      for(Peer peer:peers) {
        Long orgId = orgMapping.get(peer.getOrganizationName());//maps session.orgname to orgid (from UI we get org name, but internally we use orgid)
        user = userService.findByOrgId(orgId.toString());
        user.getSessions().add(aSession);
        userService.save(user);
      }
      //modelAndView.setViewName("defaultrandompage");
      return this.viewSession();
    }
    return modelAndView; 
  } 
  
  private void setDBUIDs(Session aSession) throws Exception {
    aSession.setDbuid(sessionDBUID.plusOne(sessionDBUID.getPosition()).clone());//todo:take out clone and replace bytearray in vertical/session/peer with hexstring 
    List<Vertical> verticals = aSession.getVerticals();
    for(Vertical vertical:verticals) {
      vertical.setDbuid(verticalDBUID.plusOne(verticalDBUID.getPosition()).clone());
    }
  }
  
  private void setVerticalIdsDeprecated(Session aSession) {
    Optional<VerticalId> currentMaxId = verticalIdService.findById(1);//  this table has only one entry with primary key as '1' to hold most recent max value of verticalId
    VerticalId aVerticalId;
    
    if(currentMaxId.isPresent()) {
      aVerticalId = currentMaxId.get();
    }
    else { 
      aVerticalId = new VerticalId();
      ByteBuffer byteBuffer = ByteBuffer.allocate(8);
      aVerticalId.setRecentId(byteBuffer.putLong(1).array());
    }
    byte[] maxIdBytes = aVerticalId.getRecentId();
    Long maxId = Longs.fromByteArray(maxIdBytes);
    maxId = Utils.setVerticalIdsDeprecated(aSession,maxId);
    aVerticalId.setRecentId(Longs.toByteArray(maxId));
    verticalIdService.save(aVerticalId);
  }
  
  @PostMapping("/updatemysessionstatus")
  public ModelAndView updateSessionStatusPosted(@Valid SessionStatusForm sessionStatusForm, BindingResult bindingResult) throws Exception { 
    //ModelAndView modelAndView = new ModelAndView();
    User user = userService.findByLoginId(Utils.getUserLoginId());
    log.info("updateSessionStatusPosted.."+sessionStatusForm.toString());
    BiMap<String,Long> orgMapping = Utils.getOrgBiMap(orgInfoService.findAll());
    String sessionOrg = orgMapping.inverse().get(Long.parseLong(user.getOrgId()));
    Set<Session> sessions = user.getSessions();
    for(Session aSession:sessions) {
      if(aSession.getSessionName().equalsIgnoreCase(sessionStatusForm.getSessionName())) {
        Set<Peer> peers = aSession.getPeers();
        for(Peer peer:peers) {
          if(peer.getOrganizationName().equalsIgnoreCase(sessionOrg)) {
            peer.setStatus(sessionStatusForm.getStatus());
            user.addSession(aSession);
            updateSessionStatus(aSession);
            //todo: update connection only if session is accepted..??
            ConnectionInfoForm updatedConnectionInfo  = new ConnectionInfoForm(sessionStatusForm.getConnectionInfoForm().getConnectionName(),
                sessionStatusForm.getConnectionInfoForm().getIpAddress(),
                sessionStatusForm.getConnectionInfoForm().getPort());
            this.addConnectionInfoPosted(updatedConnectionInfo, null);
           // break;            
          }
        }
      }
    } 
    userService.save(user);
    return this.viewSession();
    //modelAndView.setViewName("defaultrandompage");
   // return modelAndView;
  }
  
  public void updateSessionStatus(Session mySession) {
    Set<Peer> peers = mySession.getPeers();
    for (Peer peer : peers) {
      if (peer.getStatus() == EInvitationStatus.DECLINED) {//if one of them is declined, then decline the session
        mySession.setSessionStatus(ESessionStatus.DECLINED);
        sessionInfoService.save(mySession);
        return;
      }  
    }
    for (Peer peer : peers) {
      if (peer.getStatus() == EInvitationStatus.PENDING) {//if one of them is pending, then simply return
        return;
      }  
    }
    //it means: all the peers have accepted the session
    mySession.setSessionStatus(ESessionStatus.ACCEPTED);
    sessionInfoService.save(mySession);
  }

  @GetMapping("/password")
  public ModelAndView updatePassword(PasswordForm passwordForm) {
    log.info("updatepassword get");
    ModelAndView modelAndView = new ModelAndView();
    modelAndView.addObject("passwordForm", new PasswordForm("*******","*******"));
    modelAndView.setViewName("organization/passwordupdate_form");
    return modelAndView; 
  }
  
  @PostMapping("/updatepassword")
  public ModelAndView updatePasswordPosted(@RequestParam(required = false) Boolean isProfile,@Valid PasswordForm passwordForm, BindingResult bindingResult) {
    log.info("updatePasswordPosted post");
    ModelAndView modelAndView = new ModelAndView();
    if(!passwordForm.getPassword().contentEquals(passwordForm.getPasswordAgain())) 
      bindingResult.rejectValue("password", "Mismatch.organization.password", "...password does not match....");
    
    if (bindingResult.hasErrors()) {
      if(isProfile != null && isProfile)
        modelAndView = getProfile();
      else
        modelAndView.setViewName("organization/passwordupdate_form"); 
    } 
    else {
      Authentication auth = SecurityContextHolder.getContext().getAuthentication();
      org.springframework.security.core.userdetails.User springUser = (org.springframework.security.core.userdetails.User)auth.getPrincipal();
      User dashboardUser = userService.findByLoginId(springUser.getUsername());
      dashboardUser.setPassword( encoder.encode(passwordForm.getPassword()));
      if(dashboardUser.isEnabled()) {
        modelAndView = getProfile();
      }
      else {
        modelAndView.addObject("contactInfoForm", new ContactInfoForm());
        modelAndView.setViewName("organization/contactinfo_form"); 
        dashboardUser.setEnabled(true);
      }
      userService.save(dashboardUser);  
    }
    return modelAndView;
  }
  
  @GetMapping("/contactinfo")
  public ModelAndView updateContactInfo(@Valid ContactInfoForm contactInfoForm) {
    log.info("updatecontactinfoPosted get");
    ModelAndView modelAndView = new ModelAndView();
    Authentication auth = SecurityContextHolder.getContext().getAuthentication();
    org.springframework.security.core.userdetails.User springUser = (org.springframework.security.core.userdetails.User)auth.getPrincipal();
    User dashboardUser = userService.findByLoginId(springUser.getUsername());
    ContactInfo contactInfo = contactInfoService.findByLoginId(dashboardUser.getLoginId());
    ContactInfoForm persistedContactInfoForm = POJOToFormAdapter.convert(contactInfo);
    log.info(persistedContactInfoForm.toString());
    modelAndView.addObject("contactInfoForm", persistedContactInfoForm);
    modelAndView.setViewName("organization/contactinfo_form"); 
    return modelAndView;
  }
  
  @PostMapping("/updatecontactinfo")
  public ModelAndView updateContactInfoPosted(@Valid ContactInfoForm contactInfoForm, BindingResult bindingResult) {
    log.info("updatecontactinfoPosted post");
    ModelAndView modelAndView = new ModelAndView();
    if (bindingResult.hasErrors()) {
      modelAndView.setViewName("organization/contactinfo_form");//todo
    } 
    else {
      Authentication auth = SecurityContextHolder.getContext().getAuthentication();
      org.springframework.security.core.userdetails.User springUser = (org.springframework.security.core.userdetails.User)auth.getPrincipal();
      User dashboardUser = userService.findByLoginId(springUser.getUsername());
      ContactInfo contactInfo  = FormToPOJOAdapter.convert(contactInfoForm,Utils.getUserLoginId(),dashboardUser.getOrgId());
      contactInfoService.save(contactInfo);
      //modelAndView.addObject("successMessage", "Operation Success");
      //modelAndView.setViewName("organization/landing_form_org_regular_login.html");
      modelAndView = getProfile();
    }
    return modelAndView;
  }
  @PostMapping("/updateconnectioninfo")
  public ModelAndView addConnectionInfoPosted(@Valid ConnectionInfoForm connectionInfoForm, BindingResult bindingResult) {
    log.info("updatecontactinfoPosted post");
    ModelAndView modelAndView = new ModelAndView();
    if (bindingResult != null && bindingResult.hasErrors()) {
      modelAndView.setViewName("organization/connectioninfo_form"); 
    } 
    else { 
      Authentication auth = SecurityContextHolder.getContext().getAuthentication();
      org.springframework.security.core.userdetails.User springUser = (org.springframework.security.core.userdetails.User)auth.getPrincipal();
      User dashboardUser = userService.findByLoginId(springUser.getUsername());
      ConnectionInfo connectionInfo  = FormToPOJOAdapter.convert(connectionInfoForm,dashboardUser.getLoginId(),dashboardUser.getOrgId());
      connectionInfoService.save(connectionInfo);
      modelAndView = getProfile();
    }
    return modelAndView;
  } 
  
  @GetMapping("/getconnectioninfo")
  @ResponseBody
  public ConnectionInfo connectionInfo(@RequestParam(required = false) String someOrgName) throws Exception{
    List<OrgInfo> orgs = orgInfoService.findAll();
    BiMap<String,Long> orgMapping = Utils.getOrgBiMap(orgs);
    Long someOrgId = orgMapping.get(someOrgName);
    if(someOrgId == null)
      return new ConnectionInfo();
    else 
      return connectionInfoService.findByOrgId(someOrgId.toString());
  }
  
  @GetMapping("/connectioninfo")
  private ModelAndView updateConnectionInfo(ConnectionInfoForm connectionInfoForm) {
    log.info("updatecontactinfoPosted get");
    ModelAndView modelAndView = new ModelAndView();
    Authentication auth = SecurityContextHolder.getContext().getAuthentication();
    org.springframework.security.core.userdetails.User springUser = (org.springframework.security.core.userdetails.User)auth.getPrincipal();
    User dashboardUser = userService.findByLoginId(springUser.getUsername());
    ConnectionInfo connectionInfo = connectionInfoService.findByLoginId(dashboardUser.getLoginId());
    ConnectionInfoForm persistedConnectionInfoForm = POJOToFormAdapter.convert(connectionInfo);
    log.info(persistedConnectionInfoForm.toString());
    modelAndView.addObject("connectionInfoForm", persistedConnectionInfoForm);
    modelAndView.setViewName("organization/contactinfo_form"); 
    return modelAndView;
  }
  
  @GetMapping("/allorgnames")
  @ResponseBody
  public List<String> getAllOrgNames() {
    log.info("getallorgnames get ");
    List<String> orgNames = orgInfoService.findAll().stream().map(org->org.getOrgName()).collect(Collectors.toList());
    return orgNames;
  }
  
  @GetMapping("/allsessionnames")
  @ResponseBody
  public Set<String> getAllSessionNames() {//only send my sessions (don't include the sessions that I am not part of )
    log.info("getAllSessionNames get ");
    //List<String> sessionNames = sessionInfoService.findAll().stream().map(session->session.getSessionName()).collect(Collectors.toList());
    List<OrgInfo> orgs = orgInfoService.findAll();
    User user = userService.findByLoginId(Utils.getUserLoginId());
    BiMap<String,Long> orgMapping = Utils.getOrgBiMap(orgs);
    String myOrgName = orgMapping.inverse().get(Long.parseLong(user.getOrgId()));   
    List<Session> someSessions = Utils.loadMySessions(user,sessionInfoService.findAllSessions(),orgs,null); 
    List<String> peerSessionNames = someSessions.stream().map(session->session.getSessionName()).collect(Collectors.toList());
    someSessions = sessionInfoService.findSessionsByInitiatorOrganizationName(myOrgName);
    Set<String> sessionNames = someSessions.stream().map(session->session.getSessionName()).collect(Collectors.toCollection(TreeSet::new));
    //sessionNames.addAll(peerSessionNames);
    return sessionNames;
  }
  
  @GetMapping("/sessiondetails")
  @ResponseBody
  public Session getSessionDetails( @RequestParam("sessionname") String sessionName) {
    Session session = sessionInfoService.findSessionBySessionName(sessionName);
    return session;
  }
  
  @GetMapping("/orgdetails")
  public ModelAndView getOrganizationDetails(@RequestParam("orgname") String orgName) {
    log.info("getOrganizationDetails get ");
    OrgInfo orgInfo = orgInfoService.findByOrgName(orgName);
    List<OrgInfo> orgs = orgInfoService.findAll();
    BiMap<String, Long> orgMapping = Utils.getOrgBiMap(orgs);
    Long orgId = orgMapping.get(orgName);
    ContactInfo contactInfo = contactInfoService.findByOrgId(orgId.toString());
    ConnectionInfo connectionInfo = connectionInfoService.findByOrgId(orgId.toString());
    ContactInfoForm persistedContactInfoForm;
    ConnectionInfoForm persistedConnectionInfoForm;
    if (contactInfo == null)
      persistedContactInfoForm = new ContactInfoForm();
    else
      persistedContactInfoForm = POJOToFormAdapter.convert(contactInfo);
    if (connectionInfo == null)
      persistedConnectionInfoForm = new ConnectionInfoForm();
    else
      persistedConnectionInfoForm = POJOToFormAdapter.convert(connectionInfo);

    ModelAndView modelAndView = new ModelAndView();
    modelAndView.addObject("contactInfoForm", persistedContactInfoForm);
    modelAndView.addObject("connectionInfoForm", persistedConnectionInfoForm);
    modelAndView.addObject("orgInfo", orgInfo);
    modelAndView.setViewName("organization/organization_detail.html");
    return modelAndView;
  }
  
  @GetMapping("/listings")
  public ModelAndView listings() {
    log.info("org listings get ");
    List<OrgInfo> orgs = orgInfoService.findAll();
    ModelAndView modelAndView = new ModelAndView();
    modelAndView.addObject("allOrgs", orgs);
    modelAndView.setViewName("organization/vieworgs_forms");
    return modelAndView;
  }

  public String getSessionJson(Session mySession) throws JsonProcessingException {
    String dbuid = Utils.convertToHex(mySession.getDbuid());
    JsonSessionConfig jsonSessionConfig = new JsonSessionConfig(dbuid);
    List<Vertical> verticals = mySession.getVerticals();
    for (Vertical vertical : verticals) {
      dbuid = Utils.convertToHex(vertical.getDbuid());
      JsonVerticalConfig verticalConfig = new JsonVerticalConfig(dbuid);
      verticalConfig.setColumns(FormToJSONAdapter.convert(vertical));  
      jsonSessionConfig.add(verticalConfig);
    }
    Set<JsonSessionPeers> peers = new HashSet<>();
    for (Peer peer : mySession.getPeers()) {
      List<OrgInfo> orgs = orgInfoService.findAll();
      //BiMap<String,Long> orgMapping = Utils.getOrgBiMap(orgs);
      //Long anOrgId = orgMapping.get(peer.getOrganizationName());
      OrgInfo orgInfo = orgInfoService.findByOrgName(peer.getOrganizationName());
      peers.add(FormToJSONAdapter.convert(Utils.convertToHex(orgInfo.getDbuid()) , peer.getPeerType(), peer.getPeerValue(),
          peer.getOrdinalValue()));
    }
    jsonSessionConfig.setPeers(peers);
    String json = new ObjectMapper().writerWithDefaultPrettyPrinter().writeValueAsString(jsonSessionConfig);// new
                                                                                                            // Gson().toJson(jsonSessionConfig);
    return json;
  }

  public String getPeerJson(Session mySession) throws JsonProcessingException {
    JsonPeerConnections jsonPeerConnections = new JsonPeerConnections();
    BiMap<String, Long> orgMapping = Utils.getOrgBiMap(orgInfoService.findAll());
    for (Peer peer : mySession.getPeers()) {
      //Long orgId = orgMapping.get(peer.getOrganizationName());
      // String dbuid = Utils.convertToHex(orgId);
      OrgInfo orgInfo = orgInfoService.findByOrgName(peer.getOrganizationName());
      String dbuid = Utils.convertToHex(orgInfo.getDbuid());
      JsonPeerConnectionConfig aConfig = new JsonPeerConnectionConfig(dbuid,
          connectionInfoService.findByOrgId(orgMapping.get(peer.getOrganizationName()).toString()).getIpAddress(),
          connectionInfoService.findByOrgId(orgMapping.get(peer.getOrganizationName()).toString()).getPort());
      jsonPeerConnections.add(aConfig);
    } 
    
    String json =  new ObjectMapper().writerWithDefaultPrettyPrinter().writeValueAsString(jsonPeerConnections);//new Gson().toJson(jsonPeerConnections);
    return json;
  }
  
  @GetMapping("/configfile")
  public ResponseEntity<ByteArrayResource>  configFile(@RequestParam("sessionName")String sessionName, @RequestParam("configDownload") EConfigDownload configDownload) throws Exception{
    Session mySession = sessionInfoService.findSessionBySessionName(sessionName);
    String json = "";
    String fn = "";
    if (configDownload == EConfigDownload.SESSION_CONFIG) {
      json = getSessionJson(mySession);// getPeerJson(mySession);
      fn = EConfigDownload.SESSION_CONFIG.name() + ".json";
    } else if (configDownload == EConfigDownload.PEER_CONFIG) {
      json = getPeerJson(mySession);
      fn = EConfigDownload.PEER_CONFIG.name() + ".json";
    }

    log.info("json = " + json);
    byte[] configBytes = json.getBytes();
    ByteArrayResource configResource = new ByteArrayResource(configBytes);
    return ResponseEntity.ok().header(HttpHeaders.CONTENT_DISPOSITION, "attachment;filename=" + fn)
        .contentType(MediaType.APPLICATION_JSON).contentLength(configBytes.length).body(configResource);
  }

  @PostMapping(path={"/upload/profilePicture"},consumes={MediaType.MULTIPART_FORM_DATA_VALUE})
  public ModelAndView uploadprofilePicture(@RequestParam("file") MultipartFile file) throws Exception{ 
    User user = userService.findByLoginId(Utils.getUserLoginId());
    if(file.getSize()>1000 &&
        file.getName() != null &&
        file.getOriginalFilename().trim().length() > 4) {
      user.setProfilePicture(file.getBytes());
      userService.save(user);
    }
    return getProfile();
  }

  @GetMapping(path={"/download/profilePicture"},produces=MediaType.APPLICATION_OCTET_STREAM_VALUE)
  public ResponseEntity<StreamResource> getBlob(@RequestParam(required = false) String orgName)  {
    HttpHeaders headers = new HttpHeaders();
    headers.add("Cache-Control", "no-cache, no-store, must-revalidate");
    headers.add("Pragma", "no-cache");
    headers.add("Expires", "0");
    User user = userService.findByLoginId(Utils.getUserLoginId());
    if(orgName != null) {
      BiMap<String,Long> orgMapping = Utils.getOrgBiMap(orgInfoService.findAll());
      Long orgId = orgMapping.get(orgName);
      user = userService.findByOrgId(orgId.toString());
    }
    byte[] bytes = user.getProfilePicture();
    if(bytes == null)
      bytes = "".getBytes();
    InputStream is = new ByteArrayInputStream(bytes); 
    return ResponseEntity.ok().headers(headers).contentLength(bytes.length) 
        .contentType(MediaType.valueOf("image/jpeg")).body(new StreamResource(is, bytes.length));
  }
}