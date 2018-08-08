#ifndef _ADV_CONS_H_
#define _ADV_CONS_H_

#include "native_gecko.h"
#include "bg_types.h"
#include "gatt_db.h"

#define LOG_SUPPORT
#define MAX_ADV_DATA_LENGTH						31

typedef enum {
	success,
	too_long,									/* Too many bytes of data to fit into the advertisement payload */
	err_bg										/* Error return while calling APIs */
} adv_cons_ret_t;

typedef enum {
	adv_packet,
	scan_rsp,
	ota_adv_packet,
	ota_scan_rsp
} adv_packet_type_t;

typedef enum {
	/* Flags */
	flags = 0x1,
	/* Service Class UUIDs */
	more_16_uuids = 0x2,
	complete_16_uuids = 0x3,
	more_32_uuids = 0x4,
	complete_32_uuids = 0x5,
	more_128_uuids = 0x6,
	complete_128_uuids = 0x7,
	/* Local Name */
	shortened_local_name = 0x8,
	complete_local_name = 0x9,
	/* TX Power Level */
	tx_power = 0xA,												// 1 byte, 0xXX: -127 to +127 dBm
	/* Secure Simple Pairing Out of Band */
	class_of_device = 0xD,									// 3 bytes
	simple_pairing_hash_c = 0xE,									// 16 bytes
	simple_pairing_randomizer_r = 0xF,							// 16 bytes
	/* Device ID */
	extended_inquiry_response_record = 0x10,
	/* Manufacturer Specific Data */
	manufacturer_specific_data = 0xFF						// Format should be 2 bytes company ID + N bytes data
} ad_type_t;

/**
 * The advertising data is composed by elements, and each element should have below structure
 */
typedef struct {
	/* refer to above enums */
	ad_type_t ad_type;
	/* Length of the data field */
	uint8_t len;
	/* Payload */
	const uint8_t *data;
} ad_element_t;

typedef struct {
	/* Used to identify the advertiser instance. If there is only one, use 0. */
	uint8_t adv_handle;
	adv_packet_type_t adv_packet_type;
	/* Explicitly specify the number of elements */
	uint8_t ele_no;
	/* Pointer to the elements array */
	ad_element_t *p_element;
} adv_t;

adv_cons_ret_t construct_adv(adv_t adv);
void setup_adv(void);
#endif
