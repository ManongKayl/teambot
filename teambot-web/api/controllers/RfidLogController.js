/**
 * RfidLogController
 *
 * @description :: Server-side actions for handling incoming requests.
 * @help        :: See https://sailsjs.com/docs/concepts/actions
 */

module.exports = {

  /**
   * `RfidLogController.find()`
   *
   * TODO: Need to get all the RFID data from database
   * Trying to use RfidLog.find() to fetch everything
   */
  find: async function (req, res) {
    try {
      // Using .find() without parameters should get all records, right?
      const logs = await RfidLog.find();

      // Sending back the data in JSON format so frontend can use it
      return res.json({
        success: true,
        count: logs.length,
        data: logs
      });

    } catch (err) {
      // If something breaks, send error message back
      return res.serverError({
        success: false,
        message: 'Error retrieving RFID logs',
        error: err.message
      });
    }
  },

  /**
   * `RfidLogController.findByRfidData()`
   *
   * Filter logs by specific RFID card number
   * Getting the rfid_data from URL parameter
   */
  findByRfidData: async function (req, res) {
    try {
      // Get the rfid_data from the URL params
      const rfidData = req.param('rfid_data');

      // Make sure they actually provided an rfid_data
      if (!rfidData) {
        return res.badRequest({
          success: false,
          message: 'rfid_data parameter is required'
        });
      }

      // Search for logs that match this specific RFID
      const logs = await RfidLog.find({ rfid_data: rfidData });

      // Send back the filtered results
      return res.json({
        success: true,
        count: logs.length,
        data: logs
      });

    } catch (err) {
      // Error handling
      return res.serverError({
        success: false,
        message: 'Error retrieving RFID logs',
        error: err.message
      });
    }
  },

  /**
   * `RfidLogController.findByStatus()`
   *
   * Filter by status (1 or 0 / true or false)
   * Had to figure out how to convert the string param to boolean
   */
  findByStatus: async function (req, res) {
    try {
      const status = req.param('status');

      // Check if status was provided
      if (status === undefined) {
        return res.badRequest({
          success: false,
          message: 'status parameter is required'
        });
      }

      // Converting the status to boolean - took me a while to figure this out!
      // If it's the string 'true' or actual boolean true, set to true
      const boolStatus = status === 'true' || status === true;

      // Query the database with the boolean value
      const logs = await RfidLog.find({ rfid_status: boolStatus });

      // Return the filtered data
      return res.json({
        success: true,
        count: logs.length,
        data: logs
      });

    } catch (err) {
      // Catch any errors that might happen
      return res.serverError({
        success: false,
        message: 'Error retrieving RFID logs',
        error: err.message
      });
    }
  }

};