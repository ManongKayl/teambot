/**
 * RfidRegController
 *
 * Provides access to registered RFID cards.
 */

module.exports = {

  /**
    * `RfidRegController.find()`
    *
    * Return all registered RFID cards with their status.
    */
  find: async function (req, res) {
    try {
      const registered = await RfidReg.find();

      return res.json({
        success: true,
        count: registered.length,
        data: registered,
      });
    } catch (err) {
      return res.serverError({
        success: false,
        message: 'Error retrieving registered RFID cards',
        error: err.message,
      });
    }
  },

  /**
    * `RfidRegController.update()`
    *
    * Update the status of a specific RFID card.
    */
  update: async function (req, res) {
    try {
      const rfidData = req.param('id');
      const newStatus = req.body.rfid_status;

      if (!rfidData) {
        return res.badRequest({
          success: false,
          message: 'RFID data is required'
        });
      }

      if (newStatus === undefined || newStatus === null) {
        return res.badRequest({
          success: false,
          message: 'rfid_status is required'
        });
      }

      // Convert to 1 or 0 for database storage
      const numericStatus = (newStatus === true || newStatus === 1 || newStatus === '1' || newStatus === 'true') ? 1 : 0;

      const updated = await RfidReg.updateOne({ rfid_data: rfidData }).set({ rfid_status: numericStatus });

      if (!updated) {
        return res.notFound({
          success: false,
          message: 'RFID card not found'
        });
      }

      return res.json({
        success: true,
        message: 'RFID status updated successfully',
        data: updated
      });
    } catch (err) {
      return res.serverError({
        success: false,
        message: 'Error updating RFID status',
        error: err.message,
      });
    }
  },

};
