package com.stealthsoftwareinc.commercial.safrn.dashboard.config;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.data.domain.AuditorAware;
import org.springframework.data.jpa.repository.config.EnableJpaAuditing;

import com.stealthsoftwareinc.commercial.safrn.dashboard.misc.Utils;

@Configuration
@EnableJpaAuditing(auditorAwareRef = "auditorConfig")
public class DBAuditConfig {
//https://www.petrikainulainen.net/programming/spring-framework/spring-data-jpa-tutorial-auditing-part-two/
    @Bean
    public AuditorAware<String> auditorConfig() {
        return new AuditorAware<String>() {
            @Override
            public  java.util.Optional<String> getCurrentAuditor() {
              return  java.util.Optional.of(Utils.getUserLoginId());
            }
        };
    }
}