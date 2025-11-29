"Mao ning database setup gihmo"

CREATE DATABASE IF NOT EXISTS it414_db_teambot;
USE it414_db_teambot;

CREATE TABLE rfid_logs (
    time_log DATETIME NOT NULL,
    rfid_data VARCHAR(255) NOT NULL,
    rfid_status BOOLEAN
);

CREATE TABLE rfid_reg (
    rfid_data VARCHAR(255) PRIMARY KEY,
    rfid_status BOOLEAN NOT NULL
);