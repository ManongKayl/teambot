CREATE DATABASE IF NOT EXISTS it414_db_teambot;
USE it414_db_teambot;

CREATE TABLE rfid_logs (
    time_log DATETIME NOT NULL,
    rfid_data VARCHAR(255) NOT NULL,
    rfid_status BOOLEAN NOT NULL
);

CREATE TABLE rfid_reg (
    rfid_data VARCHAR(255) PRIMARY KEY,
    rfid_status BOOLEAN NOT NULL
);

insert into rfid_reg (rfid_data, rfid_status) values
('04A3B2C1D4E5', TRUE),
('1234567890AB', TRUE),
('0987654321CD', FALSE),
('FE6C0004', TRUE),
('23EE9014', FALSE);