package com.stealthsoftwareinc.commercial.safrn.dashboard.controller;

import static org.assertj.core.api.Assertions.assertThat;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.get;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.post;
import static org.springframework.test.web.servlet.result.MockMvcResultHandlers.print;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.content;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.status;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.view;

import java.util.Arrays;
import java.util.HashSet;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.web.servlet.AutoConfigureMockMvc;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.http.MediaType;
import org.springframework.test.context.testng.AbstractTestNGSpringContextTests;
import org.springframework.test.web.servlet.MockMvc;
import org.springframework.test.web.servlet.setup.MockMvcBuilders;
import org.springframework.web.context.WebApplicationContext;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.stealthsoftwareinc.commercial.safrn.dashboard.DashboardApplication;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.ERoles;
import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.IRoleRepository;
import com.stealthsoftwareinc.commercial.safrn.dashboard.misc.DBUID;
import com.stealthsoftwareinc.commercial.safrn.dashboard.misc.FormToPOJOAdapter;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.OrgInfo;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.Role;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.User;
import com.stealthsoftwareinc.commercial.safrn.dashboard.pojo.admin.CreateOrgForm;
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
@AutoConfigureMockMvc
@SpringBootTest(classes = DashboardApplication.class)
public class AdminControllerTest  extends AbstractTestNGSpringContextTests {
  @Autowired
  Encoder encoder;
  @Autowired
  private WebApplicationContext webApplicationContext;
  @Autowired
  private OrgInfoService orgInfoService;
  @Autowired
  private DashboardUserService userService;
  private MockMvc mockMvc;
  private CreateOrgForm createOrgForm;
  @Autowired
  private IRoleRepository roleRepository;
  @Autowired
  private UtilitiesService utilitiesService;
  @Autowired
  private ContactInfoService contactInfoService;
  @Autowired
  private ConnectionInfoService connectionInfoService;
  private DBUID orgDBUID ;
  @BeforeClass
  public void setup() {
    mockMvc = MockMvcBuilders.webAppContextSetup(webApplicationContext).build();
    //webAppContextSetup(webApplicationContext).apply(springSecurity()).build();
    createOrgForm = new  CreateOrgForm("kashili","kashili","kashili@stealthsoftwareinc.com");
    orgInfoService.deleteByOrgName(createOrgForm.getOrgName());
    orgDBUID = new DBUID(16);
  }

  //success test cases
 
 // @Test
 // @Order(1)
  public void testCreateOrg() throws Exception {
    log.info("###test 1:testCreateOrg");
    ObjectMapper objectMapper = new ObjectMapper();
    String json = objectMapper.writeValueAsString(createOrgForm);
    mockMvc
      .perform(post("/admin/createorg").contentType(MediaType.APPLICATION_JSON).content(json).characterEncoding("utf-8")
      .accept(MediaType.APPLICATION_JSON))
      .andDo(print())
      .andExpect(status().isOk());
    
     

  }
    
 // @Test
 // @Order(2)
  public void testBrowseRepository() throws Exception {
    log.info("###test 2:testBrowseRepository");
    mockMvc
      .perform(get("/admin/browserepository")) 
      .andExpect(content().contentType("text/html;charset=UTF-8"))
      .andExpect(status().isOk());
    OrgInfo orgs = orgInfoService.findByOrgName(createOrgForm.getOrgName());
    assertThat(orgs);
  }
 
 // @Test
 // @Order(3)
  public void testInactivateOrg() throws Exception {
    log.info("###test 3:testInactivateOrg");
    OrgInfo orgs =  orgInfoService.findByOrgName(createOrgForm.getOrgName());
    assertThat(orgs);
    assertThat(orgs.getOrgName()).containsSequence(createOrgForm.getOrgName());
    assertThat(orgs.isActive()).isTrue();
    InactivateOrgForm inactivateOrgForm = new  InactivateOrgForm(createOrgForm.getOrgName());
    ObjectMapper objectMapper = new ObjectMapper();
    String json = objectMapper.writeValueAsString(inactivateOrgForm);
    mockMvc
      .perform(post("/admin/inactivateorg").contentType(MediaType.APPLICATION_JSON).content(json))//.characterEncoding("utf-8"))
      .andExpect(status().isOk())
      .andExpect(view().name("admin/landing_form_post_admin_login"));
    orgs =  orgInfoService.findByOrgName(createOrgForm.getOrgName());
    assertThat(orgs); //todo
  }
  
  @Test
   public void testCreate5Orgs() throws Exception{ //manually delete DB  and run this test case for creating users
    createOrgForm = new  CreateOrgForm("kashili1","kashili","kashili@stealthsoftwareinc.com");//orgname,loginid,email
    createOrg(createOrgForm,"1",orgDBUID.plusOne());   
    createOrgForm = new  CreateOrgForm("36kashili1","36kashili","36kashili@gmail.com");
    createOrg(createOrgForm,"2",orgDBUID.plusOne());
    createOrgForm = new  CreateOrgForm("safrndemo1","safrndemo","krishna@gmail.com");
    createOrg(createOrgForm,"3",orgDBUID.plusOne());
    //createOrgForm = new  CreateOrgForm("desithree1","desithree","desithree@gmail.com");
    //createOrg(createOrgForm,"4");
    createOrgForm = new  CreateOrgForm("deal1231","deal123","deal123@gmail.com");
    createOrg(createOrgForm,"4",orgDBUID.plusOne());
   }
  
  private void createOrg(CreateOrgForm createOrgForm,String orgid, byte[] dbuid) {
    OrgInfo orgInfo = FormToPOJOAdapter.convert(createOrgForm);
    orgInfo.setDbuid(dbuid);
    orgInfoService.save(orgInfo);
    String loginId = createOrgForm.getLoginId();
    String password = "password";
    User user = new User(orgInfo.getOrgId().toString(), loginId, encoder.encode(password),false,
         new HashSet<Role>(Arrays.asList(roleRepository.findByRole(ERoles.USER.toString()))));
    
    user.setEnabled(true);
    userService.save(user, ERoles.USER.toString());
    ContactInfo contactInfo = ContactInfo.builder()
        .loginId(createOrgForm.getLoginId())
        .orgId( orgid)
        .about("ABOUT "+createOrgForm.getOrgName())
        .address("ADDRESS "+createOrgForm.getOrgName())
        .phone("4084212120")
        .build();
    contactInfoService.save(contactInfo);
    ConnectionInfo connectionInfo = ConnectionInfo.builder()
        .loginId(createOrgForm.getLoginId())
        .orgId(orgid)
        .ipAddress(createOrgForm.getOrgName()+".someip.com")
        .port(8090+Integer.parseInt(orgid))
        .connectionName("CONN1")
        .build();
    connectionInfoService.save(connectionInfo);
    utilitiesService.sendEmail(orgInfo.getOrgEmail(), "SAFRN Invitation: MPC Session",
         "login:" + loginId + "\npassword:" + password + "\nURL:http://localhost:8090/");
    
    
  }
}
