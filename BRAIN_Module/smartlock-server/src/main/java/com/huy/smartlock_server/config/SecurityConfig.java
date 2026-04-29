package com.huy.smartlock_server.config;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
import org.springframework.security.web.SecurityFilterChain;
import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;
import org.springframework.security.crypto.password.PasswordEncoder;

@Configuration
// @EnableWebSecurity
public class SecurityConfig {

    @Bean
    public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
        http
                .csrf(csrf -> csrf.disable())

                .authorizeHttpRequests(auth -> auth
                        // Cấp quyền cho API
                        .requestMatchers("/api/**", "/login.html", "/error", "/css/**", "/js/**").permitAll()
                        .anyRequest().authenticated())

                // 2. Cấu hình Form Login
                .formLogin(form -> form
                        .loginPage("/login.html") // Trỏ thẳng vào file tĩnh trong thư mục static
                        .loginProcessingUrl("/login") // Báo cho Spring biết form HTML sẽ gửi dữ liệu POST vào đây
                        .defaultSuccessUrl("/index.html", true) // Đăng nhập xong thì vào bảng điều khiển
                        .permitAll())

                // 3. Cho phép Đăng xuất
                .logout(logout -> logout.permitAll());

        return http.build();
    }

    @Bean
    public PasswordEncoder passwordEncoder() {
        return new BCryptPasswordEncoder();
    }

}