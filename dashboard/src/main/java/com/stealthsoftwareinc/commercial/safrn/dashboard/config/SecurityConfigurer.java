package com.stealthsoftwareinc.commercial.safrn.dashboard.config;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Configuration;
import org.springframework.security.config.annotation.authentication.builders.AuthenticationManagerBuilder;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.builders.WebSecurity;
import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
import org.springframework.security.config.annotation.web.configuration.WebSecurityConfigurerAdapter;
import com.stealthsoftwareinc.commercial.safrn.dashboard.security.Encoder;
import com.stealthsoftwareinc.commercial.safrn.dashboard.services.DashboardUserDetailsService;

@Configuration
@EnableWebSecurity
public class SecurityConfigurer extends WebSecurityConfigurerAdapter {
  @Autowired
  private DashboardUserDetailsService userDetailsService;
  @Autowired
  private Encoder encoder;

  @Override
  protected void configure(AuthenticationManagerBuilder authenticationManager) throws Exception {
    authenticationManager.inMemoryAuthentication().withUser("stealth").password("{noop}password").roles("ADMIN");
    authenticationManager.userDetailsService(userDetailsService).passwordEncoder(encoder.getPasswordEncoder());
  }
  
  @Override
  public void configure(WebSecurity webSecurity) throws Exception {
    webSecurity
      .ignoring()
      .antMatchers(
          "/assets/**",
          "/css/**",
          "/dis/**",
          "/fonts/**",
          "/images/**",
          "/js/**",
          "/resources/**",
          "/static/**",
          "/v2/api-docs",
          "/v2/api-docs/**",
          "/v2/api-docs/swagger-ui.html",
          "/swagger-resources",
          "/swagger-resources/**",
          "/configuration/ui",
          "/configuration/**",
          "/configuration/security",
          "/swagger-ui.html",
          "/webjars/springfox-swagger-ui/**",
          "/webjars/**"
          );
  }

  @Override
  protected void configure(HttpSecurity http) throws Exception {
    //BEWARE: If you are modifying this method, run test suite. 
    //Following logic routes form-based authentication and grants permissions/authorizations to the REST endpoints
    String login = "/login";
    http
      .authorizeRequests()
      .antMatchers("/").permitAll()
      .antMatchers(login).permitAll()
      .antMatchers("/forgot").permitAll()
      .antMatchers("/admin/**").hasAuthority("ROLE_ADMIN")
      .antMatchers("/organization/**").permitAll()
      .antMatchers("/actuator/**").permitAll()
      .anyRequest().authenticated()
      .and().csrf().disable()
      .formLogin().loginPage(login).loginPage("/")
      .failureUrl("/organization/orglogin?error=true")
      .defaultSuccessUrl("/default")
      .usernameParameter("user_name")// note:donot change these form-parameters
      .passwordParameter("password")
      .and().exceptionHandling();
  }
}