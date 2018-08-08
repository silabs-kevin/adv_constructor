#include "adv_cons.h"
#ifdef LOG_SUPPORT
#include "log.h"
#endif

/**
 * Do not forget to modify the second parameter of gecko_cmd_le_gap_start_advertising function to le_gap_user_data
 */

void setup_adv(void) {
	const uint8_t flag_data = 0x6;
	const uint8_t local_name_data[] = "AdvC";
	const uint8_t *service_uuid = bg_gattdb_data.uuidtable_128;

	/* https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers - To get your company ID*/
	/* Below is an example to construct your manufacturer specific data with payload set to "KBA - Adv Constructor" */
	uint16_t company_id = 0x02FF;	// 0x02FF - Silicon Labs' company ID
	uint8_t manu_data[sizeof("KBA - Adv Constructor") + 1];
	memcpy(manu_data, (uint8_t *)&company_id, 2);
	memcpy(manu_data+2, (uint8_t *)("KBA - Adv Constructor"), sizeof("KBA - Adv Constructor") - 1);

	ad_element_t ad_elements[] = {
			{
					.ad_type = flags,
					.len = 1,
					.data = &flag_data },
			{
					.ad_type = complete_local_name,
					.len = sizeof(local_name_data) - 1,
					.data = local_name_data },
			{
					.ad_type = more_128_uuids,
					.len = 16,
					.data = service_uuid },
			{
					.ad_type = manufacturer_specific_data,
					.len = sizeof(manu_data),
					.data = manu_data } };

	/*Set up advertisement payload*/
	adv_t adv = {
			.adv_handle = 0,
			.adv_packet_type = adv_packet,
			.ele_no = 3,
			.p_element = ad_elements };
	construct_adv(adv);

	/*Set up scan response payload*/
	adv.adv_handle = 0;
	adv.adv_packet_type = scan_rsp;
	adv.ele_no = 1;
	adv.p_element = &ad_elements[3];
	construct_adv(adv);
}

adv_cons_ret_t construct_adv(adv_t adv) {
	uint8_t amout_bytes = 0, i = 0;
	uint8_t buf[31];
	memset(buf, 0, 31);
	for (i = 0; i < adv.ele_no; i++) {
		amout_bytes += adv.p_element[i].len + 2;
	}
	if (amout_bytes > MAX_ADV_DATA_LENGTH) {
#ifdef LOG_SUPPORT
		LOGE("Adv data too long [length = %d], aborting.\r\n", amout_bytes);
#endif
		return too_long;
	}

	amout_bytes = 0;

	for (i = 0; i < adv.ele_no; i++) {
		buf[amout_bytes++] = adv.p_element[i].len + 1;
		buf[amout_bytes++] = adv.p_element[i].ad_type;
		memcpy(buf + amout_bytes, adv.p_element[i].data, adv.p_element[i].len);
		amout_bytes += adv.p_element[i].len;
	}

	struct gecko_msg_le_gap_bt5_set_adv_data_rsp_t *ret = gecko_cmd_le_gap_bt5_set_adv_data(adv.adv_handle, adv.adv_packet_type, amout_bytes, buf);
#ifdef LOG_SUPPORT
	if (error_checking(ret->result, 0)) {
		ERROR_ADDRESSING();
#else
		if(ret->result) {
#endif
		return err_bg;
	}
#ifdef LOG_SUPPORT
	if (adv.adv_packet_type == adv_packet) {
		LOGD("ADV Data Set to = ");
	} else if (adv.adv_packet_type == scan_rsp) {
		LOGD("Scan response Set to = ");
	}
	UINT8_ARRAY_DUMP(buf, amout_bytes);
	LOG("\r\n");
#endif
	return success;
}

