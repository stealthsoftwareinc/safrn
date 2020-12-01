package com.stealthsoftwareinc.commercial.safrn.dashboard.controller;

import javax.servlet.RequestDispatcher;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpSession;

import org.springframework.boot.web.servlet.error.ErrorController;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.servlet.ModelAndView;

import lombok.Setter;
import lombok.extern.slf4j.Slf4j;

@Slf4j
@Setter
@Controller
public class DashboardErrorController implements ErrorController {
  private static String customMessage;// todo

  @GetMapping(value = "/error")
  public ModelAndView handleError(HttpServletRequest request) {
    log.error("handleError ");
    ModelAndView modelAndView = new ModelAndView("defaultrandompage");
    Object status = request.getAttribute(RequestDispatcher.ERROR_STATUS_CODE);
    if (status != null) {
      Integer statusCode = Integer.valueOf(status.toString());
      String message = "Error code:" + statusCode;
      log.error(message);
      if (request != null && request.getQueryString() != null && request.getQueryString().contains("error=true")
          && statusCode == 404) {
        modelAndView = new ModelAndView("organization/login_form");
        modelAndView.addObject("param.error", "invalid credentials");
      } else if (request != null && statusCode == 500) {
        HttpSession session = request.getSession();
        if (session == null || session.getAttribute("User") == null) {
          log.error("session expired");
          modelAndView = new ModelAndView("organization/login_form");
          modelAndView.addObject("param.error", "Timed out. Login Again");
        }
      } else {
        modelAndView = new ModelAndView("error");
        modelAndView.addObject("errorMessage", message);
      }
    }
    customMessage = null;
    return modelAndView;
  }

  @Override
  public String getErrorPath() {
    return "/error";
  }
}
