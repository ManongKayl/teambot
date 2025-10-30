/**
 * RfidLog.js
 *
 * Model for the rfid_logs table in the database
 * This connects to the existing MySQL table
 */

module.exports = {

  attributes: {

    // Defining the columns that match our database table
    // Had to match these exactly with the database schema

    // time_log - stores when the RFID was scanned
    time_log: {
      type: 'string',
      columnType: 'datetime',
      required: true
    },

    // rfid_data - the actual RFID card number
    rfid_data: {
      type: 'string',
      columnType: 'varchar(255)',
      required: true
    },

    // rfid_status - whether the scan was successful (1) or not (0)
    // Database stores this as TINYINT(1)
    rfid_status: {
      type: 'number',
      columnType: 'tinyint(1)',
      required: true
    },

  },

  // Important: Tell Sails which table to use
  // Without this, it would look for a table called "rfidlog" instead of "rfid_logs"
  tableName: 'rfid_logs',

  // Using time_log as primary key since the table doesn't have an id column
  // Not the best practice but it works for our existing database
  primaryKey: 'time_log',

};
