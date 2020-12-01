package com.stealthsoftwareinc.commercial.safrn.dashboard.controller;

import java.util.Arrays;
import java.util.HashSet;
import java.util.List;

import javax.validation.Valid;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.validation.BindingResult;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.servlet.ModelAndView;
import org.springframework.web.servlet.config.annotation.WebMvcConfigurer;

import com.google.common.collect.BiMap;
import com.stealthsoftwareinc.commercial.safrn.dashboard.config.AppConfig;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.ERoles;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.IConstants;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.IRoleRepository;
import com.stealthsoftwareinc.commercial.safrn.dashboard.misc.DBUID;
import com.stealthsoftwareinc.commercial.safrn.dashboard.misc.FormToPOJOAdapter;
import com.stealthsoftwareinc.commercial.safrn.dashboard.misc.POJOToFormAdapter;
import com.stealthsoftwareinc.commercial.safrn.dashboard.misc.Utils;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.ConnectionInfoForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.ContactInfoForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.OrgInfo;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.Role;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.User;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.admin.CreateOrgForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.admin.EmailOrgForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.admin.InactivateOrgForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.ConnectionInfo;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.ContactInfo;
import com.stealthsoftwareinc.commercial.safrn.dashboard.security.Encoder;
import com.stealthsoftwareinc.commercial.safrn.dashboard.services.ConnectionInfoService;
import com.stealthsoftwareinc.commercial.safrn.dashboard.services.ContactInfoService;
import com.stealthsoftwareinc.commercial.safrn.dashboard.services.DashboardUserService;
import com.stealthsoftwareinc.commercial.safrn.dashboard.services.OrgInfoService;
import com.stealthsoftwareinc.commercial.safrn.dashboard.services.UtilitiesService;

import lombok.extern.slf4j.Slf4j;

@Slf4j
@Controller
@RequestMapping("/admin")
public class AdminController implements WebMvcConfigurer {

  @Autowired
  private DashboardUserService userService;
  @Autowired
  private UtilitiesService utilitiesService;
  @Autowired
  private OrgInfoService orgInfoService;
  @Autowired
  Encoder encoder;
  @Autowired
  private IRoleRepository roleRepository;
  @Autowired
  private ContactInfoService contactInfoService;
  @Autowired
  private ConnectionInfoService connectionInfoService;
  //@Autowired
 // private AppConfig appConfig;
  private DBUID orgDBUID ; //todo: create a service to hold prev values
  
  @Autowired
  public AdminController(AppConfig appConfig) {
    log.info("constructor..AdminController..");
    orgDBUID = new DBUID(appConfig.getOrgDbuidLength());
  }
  @GetMapping("/browserepository")
  public ModelAndView browseRepository() {
    log.info("browseRepository get ");
   /* List<OrgInfo> orgs = orgInfoService.findAll();
    String value = Utils.getFormattedOutput(orgs);
    ModelAndView modelAndView = new ModelAndView();
    modelAndView.addObject("successMessage", value);
    modelAndView.setViewName("admin/landing_form_post_admin_login");
    return modelAndView;*/
    
    List<OrgInfo> orgs = orgInfoService.findAll();
    ModelAndView modelAndView = new ModelAndView();
    modelAndView.addObject("allOrgs", orgs);
    modelAndView.setViewName("admin/browserepository_form");
    return modelAndView;
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
    modelAndView.setViewName("admin/organization_detail.html");
    return modelAndView;
  }
  

  @GetMapping("/inactivateorg")
  public String inactivateOrg(InactivateOrgForm inactivateOrg) {
    log.info("inactivateorg get ");
    return "admin/inactivateorg_form";
  }

  @PostMapping("/inactivateorg")
  public ModelAndView inactivateOrgPosted(@Valid InactivateOrgForm inactivateOrgForm, BindingResult bindingResult) {
    log.info("inactivateOrg Posted "+inactivateOrgForm.toString());
    ModelAndView modelAndView = new ModelAndView();
    OrgInfo  orgs = orgInfoService.findByOrgName(inactivateOrgForm.getOrgName());
    if (bindingResult.hasErrors()) {
      modelAndView.setViewName("admin/inactivateorg_form");
      return modelAndView;
    }
    
    if (orgs != null) {
      orgs.setActive(false);
        orgInfoService.save(orgs);
    }
    modelAndView.addObject("successMessage", "Inactivated   org "+orgs.getOrgName());
    modelAndView.setViewName("admin/landing_form_post_admin_login");
    return modelAndView;
  }

  @GetMapping("/emailorgs")
  public String emailOrgs(EmailOrgForm emailOrgForm) {
    log.info("emailorgs get ");
    return "admin/emailorg_form";
  }
  
  @PostMapping("/emailorgs")
  public ModelAndView emailOrgsPosted(@Valid EmailOrgForm emailOrgForm, BindingResult bindingResult) {
    log.info("emailorgs posted "+emailOrgForm.toString());
    ModelAndView modelAndView = new ModelAndView();
    if (bindingResult.hasErrors()) {
      modelAndView.setViewName("admin/emailorg_form");
      return modelAndView;
    }
    List<OrgInfo> orgs = orgInfoService.findAll();
    utilitiesService.sendEmail(orgs, emailOrgForm.getMessage());
    modelAndView.addObject("successMessage", "Emails processed"); 
    modelAndView.setViewName("admin/landing_form_post_admin_login");
    log.info("Emails processed successfully");
    return modelAndView;
  }
  
  @GetMapping("/createorg")
  public String createOrg(CreateOrgForm createOrgForm) {
    log.info("createorg get ");
    return "admin/createorg_form";
  }

  @PostMapping("/createorg")
  public ModelAndView createOrgPosted(@Valid CreateOrgForm createOrgForm, BindingResult bindingResult) throws Exception{
    log.info("createorg Posted "+createOrgForm.toString());
    ModelAndView modelAndView = new ModelAndView();
    User user = userService.findByLoginId(createOrgForm.getLoginId());
    if (user != null)
      bindingResult.rejectValue("loginId", "Duplicate.orgregistration.loginid", "...login already exists...");
    OrgInfo org = orgInfoService.findByOrgName(createOrgForm.getOrgName());
    if (org != null)
      bindingResult.rejectValue("orgName", "Duplicate.orgregistration.orgName", "...orgName already exists...");
    org = orgInfoService.findByOrgEmail(createOrgForm.getOrgEmail());
    if (org != null)
      bindingResult.rejectValue("orgEmail", "Duplicate.orgregistration.orgEmail", "...orgEmail already exists...");
    if(createOrgForm.getLoginId().trim().equalsIgnoreCase(createOrgForm.getOrgName().trim()))
      bindingResult.rejectValue("loginId", "Duplicate.orgregistration.duplicate", "...login and orgname must differ...");
    
    if (bindingResult.hasErrors()) {
      modelAndView.setViewName("admin/createorg_form");
    } else {
      OrgInfo orgInfo = FormToPOJOAdapter.convert(createOrgForm);
      orgInfo.setDbuid(orgDBUID.plusOne(orgDBUID.getPosition()).clone());
      orgInfoService.save(orgInfo);
      String loginId = createOrgForm.getLoginId();
      String password = Utils.getUserLoginPassword(IConstants.passwordLength); // todo: use range
      user = new User(orgInfo.getOrgId().toString(), loginId, encoder.encode(password),false,
          new HashSet<Role>(Arrays.asList(roleRepository.findByRole(ERoles.USER.toString()))));
      utilitiesService.sendEmail(orgInfo.getOrgEmail(), "SAFRN Invitation: MPC Session",
          "login:" + loginId + "\npassword:" + password + "\nURL:http://localhost:8090/");
      userService.save(user, ERoles.USER.toString());
      modelAndView.addObject("successMessage", "Organization has been added successfully");
      modelAndView.setViewName("admin/landing_form_post_admin_login");
      log.info("Organization has been added successfully.." + loginId +" with DBUID = "+Utils.convertToHex(orgInfo.getDbuid()));
    }
    return modelAndView;
  }
}
