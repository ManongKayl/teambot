CREATE TABLE rfid_logs (
    time_log DATETIME NOT NULL,
    rfid_data VARCHAR(255) NOT NULL,
    rfid_status BOOLEAN NOT NULL
);

CREATE TABLE rfid_reg (
    rfid_data VARCHAR(255) PRIMARY KEY,
    rfid_status BOOLEAN NOT NULL
);