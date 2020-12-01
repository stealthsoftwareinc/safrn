package com.stealthsoftwareinc.commercial.safrn.dashboard;

import org.springframework.boot.CommandLineRunner;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.context.properties.EnableConfigurationProperties;

 
import com.stealthsoftwareinc.commercial.safrn.dashboard.config.AppConfig;

@SpringBootApplication
@EnableConfigurationProperties(AppConfig.class)
public class DashboardApplication implements CommandLineRunner {
  public static void main(String[] args) {
    SpringApplication.run(DashboardApplication.class, args);
  }

  @Override
  public void run(String... args) throws Exception {
  }
}
