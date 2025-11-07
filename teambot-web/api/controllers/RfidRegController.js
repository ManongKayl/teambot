/**
 * RfidRegController
 *
 * @description :: Server-side actions for handling incoming requests.
 * @help        :: See https://sailsjs.com/docs/concepts/actions
 */

module.exports = {

  /**
   * `RfidRegController.find()`
   *
   * Get all registered RFID cards from database
   */
  find: async function (req, res) {
    try {
      // Fetch all registered RFIDs
      const regs = await RfidReg.find();

      // Return the data in JSON format
      return res.json({
        success: true,
        count: regs.length,
        data: regs
      });

    } catch (err) {
      // Error handling
      return res.serverError({
        success: false,
        message: 'Error retrieving RFID registrations',
        error: err.message
      });
    }
  },

  /**
   * `RfidRegController.findByStatus()`
   *
   * Filter registered RFIDs by status (active/inactive)
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

      // Convert to boolean
      const boolStatus = status === 'true' || status === true;

      // Query the database
      const regs = await RfidReg.find({ rfid_status: boolStatus });

      // Return filtered data
      return res.json({
        success: true,
        count: regs.length,
        data: regs
      });

    } catch (err) {
      // Error handling
      return res.serverError({
        success: false,
        message: 'Error retrieving RFID registrations',
        error: err.message
      });
    }
  },

  /**
   * `RfidRegController.updateStatus()`
   *
   * Update RFID status and create log entry
   */
  updateStatus: async function (req, res) {
    try {
      const rfidData = req.param('rfid_data');
      const newStatus = req.param('status');

      // Validate required parameters
      if (!rfidData) {
        return res.badRequest({
          success: false,
          message: 'rfid_data parameter is required'
        });
      }

      if (newStatus === undefined) {
        return res.badRequest({
          success: false,
          message: 'status parameter is required'
        });
      }

      // Convert status to boolean
      const boolStatus = newStatus === 'true' || newStatus === true || newStatus === 1;

      // Update the RFID registration status
      const updatedReg = await RfidReg.updateOne({ rfid_data: rfidData })
        .set({ rfid_status: boolStatus });

      if (!updatedReg) {
        return res.notFound({
          success: false,
          message: 'RFID not found'
        });
      }

      // Create a log entry for this status change with GMT+8 timestamp
      const now = new Date();
      const gmt8Time = new Date(now.getTime() + (8 * 60 * 60 * 1000)); // Add 8 hours for GMT+8
      const timeString = gmt8Time.toISOString().slice(0, 19).replace('T', ' ');

      const newLog = await RfidLog.create({
        time_log: timeString,
        rfid_data: rfidData,
        rfid_status: boolStatus
      }).fetch();

      // Return success response
      return res.json({
        success: true,
        message: 'RFID status updated successfully',
        data: {
          registration: updatedReg,
          log: newLog
        }
      });

    } catch (err) {
      // Error handling
      return res.serverError({
        success: false,
        message: 'Error updating RFID status',
        error: err.message
      });
    }
  }

};