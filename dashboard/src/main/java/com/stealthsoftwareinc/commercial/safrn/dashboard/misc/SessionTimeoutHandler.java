//package com.stealthsoftwareinc.commercial.safrn.dashboard.misc;
//
//import java.io.IOException;
//
//import javax.servlet.ServletException;
//import javax.servlet.http.HttpServletRequest;
//import javax.servlet.http.HttpServletResponse;
//
//import org.springframework.security.core.Authentication;
//import org.springframework.security.web.authentication.SimpleUrlAuthenticationSuccessHandler;
//
//public class SessionTimeoutHandler extends SimpleUrlAuthenticationSuccessHandler {
//  public final Integer SESSION_TIMEOUT_IN_SECONDS = 60 * 2;
//  @Override
//  public void onAuthenticationSuccess(HttpServletRequest request, HttpServletResponse response,
//      Authentication authentication) throws ServletException, IOException {
//    request.getSession().setMaxInactiveInterval(SESSION_TIMEOUT_IN_SECONDS);
//  }
//}