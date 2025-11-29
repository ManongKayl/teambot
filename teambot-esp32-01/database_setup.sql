CREATE DATABASE IF NOT EXISTS it414_db_teambot;
USE it414_db_teambot;

"Mao ni diri makita ang logs"
CREATE TABLE rfid_logs (
    time_log DATETIME NOT NULL,
    rfid_data VARCHAR(255) NOT NULL,
    rfid_status BOOLEAN NOT NULL
);


"Mao ni para sa registered RFID"
CREATE TABLE rfid_reg (
    rfid_data VARCHAR(255) PRIMARY KEY,
    rfid_status BOOLEAN NOT NULL
);