
DROP DATABASE IF EXISTS smartlockdb;
CREATE DATABASE smartlockdb CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE smartlockdb;

CREATE TABLE users (
    id INT PRIMARY KEY,
    full_name VARCHAR(255) NOT NULL,
    role VARCHAR(50) NOT NULL,
    username VARCHAR(50) UNIQUE,
    password VARCHAR(255)
);

CREATE TABLE access_logs (
    id INT AUTO_INCREMENT PRIMARY KEY,
    camera_name VARCHAR(50),
    confidence DOUBLE,
    status VARCHAR(50),
    timestamp DATETIME,
    user_id INT,
    user_name VARCHAR(255)
);

INSERT INTO users (id, full_name, role, username, password) 
VALUES (
    1, 
    'Lê Quốc Huy', 
    'ADMIN', 
    'admin', 
    '$2a$10$8.UnVuG9HHgffUDAlk8qfOuVGkqRzgVymGe07xd00DMxs.AQubh4a'
);
UPDATE users SET password = '$2a$10$4x/Hag3/pkoPC172bUGqTO0FblLy/0xEQp6.Fh85RW/nZqaj7JiQ2' WHERE username = 'admin';
COMMIT;

SELECT * FROM users;