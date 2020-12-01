package com.stealthsoftwareinc.commercial.safrn.dashboard.controller;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import javax.validation.Valid;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.stereotype.Controller;
import org.springframework.validation.BindingResult;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.servlet.ModelAndView;
import org.springframework.web.servlet.config.annotation.ViewControllerRegistry;
import org.springframework.web.servlet.config.annotation.WebMvcConfigurer;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.ERoles;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.ESessionAuditType;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.IConstants;
import com.stealthsoftwareinc.commercial.safrn.dashboard.misc.Utils;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.CreateSessionForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.ForgotForm;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.OrgInfo;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.SessionAudit;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.User;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.organization.ParticipantStatus;
import com.stealthsoftwareinc.commercial.safrn.dashboard.security.Encoder;
import com.stealthsoftwareinc.commercial.safrn.dashboard.services.DashboardUserService;
import com.stealthsoftwareinc.commercial.safrn.dashboard.services.OrgInfoService;
import com.stealthsoftwareinc.commercial.safrn.dashboard.services.SessionAuditService;
import com.stealthsoftwareinc.commercial.safrn.dashboard.services.UtilitiesService;

import lombok.extern.slf4j.Slf4j;

@Slf4j
@Controller
public class HomeController implements WebMvcConfigurer {
  @Autowired
  private DashboardUserService userService;
  @Autowired
  Encoder encoder;
  @Autowired
  private OrgInfoService orgInfoService;
  @Autowired
  private UtilitiesService utilitiesService;
  @Autowired
  private SessionAuditService sessionAuditService;
  
  @Override
  public void addViewControllers(ViewControllerRegistry registry) {
    registry.addViewController("/home").setViewName("home");
    registry.addViewController("/").setViewName("home");// forwarding a request for "/" to a view called "home"
  }

  @GetMapping("/")
  public String home() {
    log.info("home ");
    return "organization/login_form";
  }

  @GetMapping("/default")
  public ModelAndView defaultAfterLogin(CreateSessionForm createSessionForm, BindingResult bindingResult) {
    log.info("defaultAfterLogin get");
    List<SessionAudit> lastLogin = sessionAuditService.findByUserIdOrderByIdDesc(Utils.getUserLoginId());
    SessionAudit now = SessionAudit.builder()
        .userId(Utils.getUserLoginId())
        .auditType(ESessionAuditType.LOGIN)
        .timeStamp(LocalDateTime.now())
        .build();
    sessionAuditService.save(now);
    ModelAndView modelAndView = new ModelAndView();
    if (Utils.hasAuthority(ERoles.ADMIN.toString())
        || Utils.hasAuthority("ROLE_"+ERoles.ADMIN.toString())) {
      modelAndView.setViewName("admin/landing_form_post_admin_login.html");
      return modelAndView;
    }
    else if (Utils.hasAuthority(ERoles.USER.toString())) {
      Authentication auth = SecurityContextHolder.getContext().getAuthentication();
      org.springframework.security.core.userdetails.User springUser = (org.springframework.security.core.userdetails.User)auth.getPrincipal();
      User dashboardUser = userService.findByLoginId(springUser.getUsername());
      if(!dashboardUser.isEnabled()) {
        log.info("First time Org Login");
        //dashboardUser.setEnabled(true);
        //userService.save(dashboardUser); 
        modelAndView.addObject("successMessage" , "Setup your profile in next 2 screens");
        modelAndView.setViewName("organization/landing_form_org_1sttime_login.html");
        return modelAndView;
      }
      else {
        log.info("default org login");
        User user = userService.findByLoginId(Utils.getUserLoginId());
        Map<String, List<ParticipantStatus>> participantStatus = Utils.mySessionParticipants(user);
        Set<Entry<String, List<ParticipantStatus>>> entries = participantStatus.entrySet();
        String message = "";
        for(Entry<String, List<ParticipantStatus>> entry:entries) {
          message+=" "+entry.getKey()+" with participants "+entry.getValue().toString();
        }
        log.info("==="+message);
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss");
        if(lastLogin !=null && 
            lastLogin.size()>0 &&
            lastLogin.get(0) !=null) {
          String formattedLastLogin = lastLogin.get(0).getTimeStamp().format(formatter);
          String message1 = "You last logged in at "+formattedLastLogin+" UTC";
          log.info("==="+message1);
          modelAndView.addObject("successMessage",message1); 
        }
        modelAndView.setViewName("organization/landing_form_org_regular_login.html");
        return modelAndView;
      }
    }
    else {
      modelAndView.setViewName("error");
      return modelAndView; 
    }
  }

  @GetMapping("/forgot")
  public String forgot(ForgotForm forgotForm) {
    log.info("forgot get ");
    return "forgot_form";
  }

  @PostMapping("/forgot")
  public String forgotPosted(@Valid ForgotForm forgotForm, BindingResult bindingResult) {
    log.info("forgot Posted ");
    OrgInfo orgs = orgInfoService.findByOrgEmail(forgotForm.getEmail());
    if(orgs ==null){
      bindingResult.rejectValue("email", "Notfound.organization.email", "...Email does not exist; contact administrator....");
    }
    if (bindingResult.hasErrors()) {
      log.info("Error found.." + bindingResult.toString());
      return "forgot_form";
    }
    log.info("Processing forgot " + forgotForm.toString());
    String password = Utils.getUserLoginPassword(IConstants.passwordLength);
    OrgInfo orginfo = orgInfoService.findByOrgEmail(forgotForm.getEmail());
    if(orginfo !=null) {
      User user = userService.findByOrgId(orginfo.getOrgId().toString());
      user.setPassword(encoder.encode(password));
      userService.save(user);
      utilitiesService.sendEmail(forgotForm.getEmail(), "SAFRN Dashboard Password Reset",
          "login:" + forgotForm.getEmail() + "\npassword:" + password + "\nURL:http://localhost:8090/");
    }
    
    return "postforgot_form";
  }
}
