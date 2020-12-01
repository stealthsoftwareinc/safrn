package com.stealthsoftwareinc.commercial.safrn.dashboard.controller;

import static org.springframework.security.test.web.servlet.request.SecurityMockMvcRequestBuilders.formLogin;
import static org.springframework.security.test.web.servlet.response.SecurityMockMvcResultMatchers.authenticated;
import static org.springframework.security.test.web.servlet.response.SecurityMockMvcResultMatchers.unauthenticated;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.get;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.status;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.view;

import com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces.IUserRepository;

import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.web.servlet.AutoConfigureMockMvc;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.test.web.servlet.MockMvc;
import org.springframework.transaction.annotation.Transactional;

@SpringBootTest
@AutoConfigureMockMvc
public class LoginControllerTest {
  @Autowired
  private MockMvc mockMvc;

  @Autowired
  private IUserRepository user;

  @Test
  public void testHomePage() throws Exception {
    this.mockMvc
      .perform(get("/"))
      .andExpect(status().isOk())
      .andExpect(view().name("home"));
  }

  // No user registration
  // @Test
  // public void testRegisterGetPage() throws Exception {
  //   this.mockMvc
  //     .perform(get("/register"))
  //     .andExpect(status().isOk())
  //     .andExpect(view().name("registration_form"));
  // }

  // @Test
  // @Transactional(rollbackFor = Exception.class)
  // public void testRegisterPostPage() throws Exception {
  //   user.deleteByLoginId("test@register.com");
  //   this.mockMvc
  //     .perform(
  //       post("/register")
  //       .param("email", "test@register.com")
  //       .param("password", "test1234")
  //       .param("passwordAgain", "test1234")
  //     )
  //     .andExpect(status().isOk())
  //     .andExpect(view().name("home"));
  // }

  // @Test
  // @Transactional(rollbackFor = Exception.class)
  // public void testRegisterPostUnmatchPasswordPage() throws Exception {
  //   user.deleteByLoginId("test@register.com");
  //   this.mockMvc
  //     .perform(
  //       post("/register")
  //       .param("email", "test@register.com")
  //       .param("password", "test1234")
  //       .param("passwordAgain", "test123")
  //     )
  //     .andExpect(status().isOk())
  //     .andExpect(view().name("registration_form"));
  // }

  @Test
  public void testLoginGetPage() throws Exception {
    this.mockMvc
      .perform(get("/organization/orglogin"))
      .andExpect(status().isOk())
      .andExpect(view().name("organization/login_form"));
  }

  @Test
  @Transactional(rollbackFor = Exception.class)
  public void testLogin() throws Exception {
    this.mockMvc
      .perform(
        formLogin("/login")
        .user("user_name","stealth")
        .password("password","password")
      )
      .andExpect(authenticated().withRoles("ADMIN"));
  }

  @Test
  @Transactional(rollbackFor = Exception.class)
  public void testLoginInvalidPassword() throws Exception {
    this.mockMvc
      .perform(
        formLogin("/login")
        .user("user_name","stealth")
        .password("password","invaliadpassword")
      )
      .andExpect(unauthenticated());
  }
}