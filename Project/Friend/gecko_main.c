/***************************************************************************//**
 * @file
 * @brief Silicon Labs BT Mesh Empty Example Project
 * This example demonstrates the bare minimum needed for a Blue Gecko BT Mesh C application.
 * The application starts unprovisioned Beaconing after boot
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"
#include <gecko_configuration.h>
#include <mesh_sizes.h>

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"
#include <em_gpio.h>

/* Device initialization header */
#include "hal-config.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif
#include "src/ble_mesh_device_type.h"

#include <stdio.h>
#include <string.h>
#include "src/display.h"
#include "src/gpio.h"
#include "src/log.h"
#include "mesh_generic_model_capi_types.h"
#include "mesh_lib.h"
/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

// bluetooth stack heap
#define MAX_CONNECTIONS 2

uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS) + BTMESH_HEAP_SIZE + 1760];

// Bluetooth advertisement set configuration
//
// At minimum the following is required:
// * One advertisement set for Bluetooth LE stack (handle number 0)
// * One advertisement set for Mesh data (handle number 1)
// * One advertisement set for Mesh unprovisioned beacons (handle number 2)
// * One advertisement set for Mesh unprovisioned URI (handle number 3)
// * N advertisement sets for Mesh GATT service advertisements
// (one for each network key, handle numbers 4 .. N+3)
//
#define MAX_ADVERTISERS (4 + MESH_CFG_MAX_NETKEYS)

static gecko_bluetooth_ll_priorities linklayer_priorities = GECKO_BLUETOOTH_PRIORITIES_DEFAULT;

// bluetooth stack configuration
extern const struct bg_gattdb_def bg_gattdb_data;

// Flag for indicating DFU Reset must be performed
uint8_t boot_to_dfu = 0;

const gecko_configuration_t config =
{
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.max_advertisers = MAX_ADVERTISERS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap) - BTMESH_HEAP_SIZE,
  .bluetooth.sleep_clock_accuracy = 100,
  .bluetooth.linklayer_priorities = &linklayer_priorities,
  .gattdb = &bg_gattdb_data,
  .btmesh_heap_size = BTMESH_HEAP_SIZE,
#if (HAL_PA_ENABLE)
  .pa.config_enable = 1, // Set this to be a valid PA config
#if defined(FEATURE_PA_INPUT_FROM_VBAT)
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
#else
  .pa.input = GECKO_RADIO_PA_INPUT_DCDC,
#endif // defined(FEATURE_PA_INPUT_FROM_VBAT)
#endif // (HAL_PA_ENABLE)
  .max_timers = 16,
};

void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt);
void mesh_native_bgapi_init(void);
bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);


/* Global variables */
char device_name[15];
uint16_t result;
uint16_t primary_elem_addr;

/* Project defines */
#define TIMER_FACTORY_RESET 200


/* Function declarations */
void data_subscriber_init();
void init_all_models();
void onoff_publish();
/**
 * See light switch app.c file definition
 */
void gecko_bgapi_classes_init_server_friend(void)
{
	gecko_bgapi_class_dfu_init();
	gecko_bgapi_class_system_init();
	gecko_bgapi_class_le_gap_init();
	gecko_bgapi_class_le_connection_init();
	//gecko_bgapi_class_gatt_init();
	gecko_bgapi_class_gatt_server_init();
	gecko_bgapi_class_hardware_init();
	gecko_bgapi_class_flash_init();
	gecko_bgapi_class_test_init();
	//gecko_bgapi_class_sm_init();
	//mesh_native_bgapi_init();
	gecko_bgapi_class_mesh_node_init();
	//gecko_bgapi_class_mesh_prov_init();
	gecko_bgapi_class_mesh_proxy_init();
	gecko_bgapi_class_mesh_proxy_server_init();
	//gecko_bgapi_class_mesh_proxy_client_init();
	//gecko_bgapi_class_mesh_generic_client_init();
	gecko_bgapi_class_mesh_generic_server_init();
	gecko_bgapi_class_mesh_vendor_model_init();
	//gecko_bgapi_class_mesh_health_client_init();
	//gecko_bgapi_class_mesh_health_server_init();
	//gecko_bgapi_class_mesh_test_init();
//	gecko_bgapi_class_mesh_lpn_init();
	gecko_bgapi_class_mesh_friend_init();
	mesh_native_bgapi_init();
}


/**
 * See main function list in soc-btmesh-switch project file
 */
void gecko_bgapi_classes_init_client_lpn(void)
{
	gecko_bgapi_class_dfu_init();
	gecko_bgapi_class_system_init();
	gecko_bgapi_class_le_gap_init();
	gecko_bgapi_class_le_connection_init();
	//gecko_bgapi_class_gatt_init();
	gecko_bgapi_class_gatt_server_init();
	gecko_bgapi_class_hardware_init();
	gecko_bgapi_class_flash_init();
	gecko_bgapi_class_test_init();
	//gecko_bgapi_class_sm_init();
	//mesh_native_bgapi_init();
	gecko_bgapi_class_mesh_node_init();
	//gecko_bgapi_class_mesh_prov_init();
	gecko_bgapi_class_mesh_proxy_init();
	gecko_bgapi_class_mesh_proxy_server_init();
	//gecko_bgapi_class_mesh_proxy_client_init();
	gecko_bgapi_class_mesh_generic_client_init();
	//gecko_bgapi_class_mesh_generic_server_init();
	//gecko_bgapi_class_mesh_vendor_model_init();
	//gecko_bgapi_class_mesh_health_client_init();
	//gecko_bgapi_class_mesh_health_server_init();
	//gecko_bgapi_class_mesh_test_init();
	gecko_bgapi_class_mesh_lpn_init();
//	gecko_bgapi_class_mesh_friend_init();

}

/*
 * Function name: onoff_request()
 * Description: This function is called when the request is received from the client
 * parameters: This function is called back fuction where bluetooth stack passes all parameters
 * return: void*/
void onoff_request(uint16_t model_id,
        		   uint16_t element_index,
				   uint16_t client_addr,
				   uint16_t server_addr,
				   uint16_t appkey_index,
				   const struct mesh_generic_request *request,
				   uint32_t transition_ms,
				   uint16_t delay_ms,
				   uint8_t request_flags)
{
	LOG_INFO("Onoff request received");
	if(request->on_off == MESH_GENERIC_ON_OFF_STATE_ON)
	{
		displayPrintf(DISPLAY_ROW_TEMPVALUE,"Button Pressed");
		gpioLed0SetOn();
	}
	else
	{
		displayPrintf(DISPLAY_ROW_TEMPVALUE,"Button Released");
		gpioLed0SetOff();
	}
}

void gecko_main_init()
{
  // Initialize device
  initMcu();
  // Initialize board
  initBoard();
  // Initialize application
  initApp();

  // Minimize advertisement latency by allowing the advertiser to always
  // interrupt the scanner.
  linklayer_priorities.scan_max = linklayer_priorities.adv_min + 1;

  gecko_stack_init(&config);

  if( DeviceUsesClientModel() ) {
	  gecko_bgapi_classes_init_client_lpn();
  } else {
	  gecko_bgapi_classes_init_server_friend();
  }

  // Initialize coexistence interface. Parameters are taken from HAL config.
  gecko_initCoexHAL();

}

void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt)
{
  switch (evt_id) {
    case gecko_evt_system_boot_id:
    	LOG_INFO("evt::gecko_evt_system_boot_id");
    	struct gecko_msg_system_get_bt_address_rsp_t *Addr = gecko_cmd_system_get_bt_address();
    	displayPrintf(DISPLAY_ROW_NAME,"FRIEND");
    	sprintf(device_name,"Friend %02x%02x",Addr->address.addr[1],Addr->address.addr[0]);
    	displayPrintf(DISPLAY_ROW_BTADDR,device_name);
    	if(GPIO_PinInGet(BUTTON0_PORT, BUTTON0_PIN) == 0 || GPIO_PinInGet(BUTTON1_PORT, BUTTON1_PIN) == 0)
    	{
    		displayPrintf(DISPLAY_ROW_ACTION - 1, "***********");
    		displayPrintf(DISPLAY_ROW_ACTION, "Factory Reset");
    		displayPrintf(DISPLAY_ROW_ACTION + 1, "***********");
    		result = gecko_cmd_flash_ps_erase_all()->result;
    			if(result) LOG_ERROR("Flash erase failed");
    		result = gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_FACTORY_RESET, 1)->result;
    			if(result) LOG_ERROR("Timer set failed");
    	}
    	else
    	{
    		result = gecko_cmd_gatt_server_write_attribute_value(gattdb_device_name, 0, strlen(device_name), (uint8_t *)device_name)->result;
    		if(result) LOG_ERROR("Write attribute failed");
    		// Initialize Mesh stack in Node operation mode, wait for initialized event
    		result = gecko_cmd_mesh_node_init()->result;
    		if(result) LOG_ERROR("Mesh node init failed");
    	}
      break;


    case gecko_evt_hardware_soft_timer_id:
    	LOG_INFO("evt::gecko_evt_hardware_soft_timer_id");
    	switch(evt->data.evt_hardware_soft_timer.handle)
    	{
    	case TIMER_FACTORY_RESET:
    		gecko_cmd_system_reset(0);
    	}
    	break;

    case gecko_evt_mesh_node_initialized_id:
    	LOG_INFO("evt::gecko_evt_mesh_node_initialized_id");
    	struct gecko_msg_mesh_vendor_model_init_rsp_t* vendor_init_response;
    	uint8_t op_cd[1] = {1};
    	vendor_init_response = gecko_cmd_mesh_vendor_model_init(0, 0x02ff, 0x0001, false, 1, op_cd);
    	if(vendor_init_response->result != 0)
    	{
    		LOG_ERROR("0001 vendor model init failed with %d response",vendor_init_response->result);
    	}
    	LOG_INFO("Vendor model init success");
    	uint8_t op_cd1[1] = {1};
    	vendor_init_response = gecko_cmd_mesh_vendor_model_init(0, 0x02ff, 0x0002, false, 1, op_cd1);
    	if(vendor_init_response->result != 0)
    	{
    		LOG_ERROR("0002 vendor model init failed with %d response",vendor_init_response->result);
    	}
    	LOG_INFO("Vendor model init success");

    	if (!evt->data.evt_mesh_node_initialized.provisioned) {
    		displayPrintf(DISPLAY_ROW_ACTION,"Unprovisioned");
    		// The Node is now initialized, start unprovisioned Beaconing using PB-ADV and PB-GATT Bearers
    		gecko_cmd_mesh_node_start_unprov_beaconing(0x3);
    	}
    	if((evt->data.evt_mesh_node_initialized.provisioned))
    	{
    		primary_elem_addr = evt->data.evt_mesh_node_initialized.address;
    		LOG_INFO("Node is provisioned with %x address and %ld ivi",primary_elem_addr,evt->data.evt_mesh_node_initialized.ivi);
    		data_subscriber_init();
    	}
    	break;

    case gecko_evt_mesh_node_key_added_id:
    	LOG_INFO("evt::gecko_evt_mesh_node_key_added_id");
    	break;

    case gecko_evt_mesh_node_model_config_changed_id:
    	LOG_INFO("evt::gecko_evt_mesh_node_model_config_changed_id");
    	LOG_INFO("Element address is %x",evt->data.evt_mesh_node_model_config_changed.element_address);
    	LOG_INFO("Model id is %x",evt->data.evt_mesh_node_model_config_changed.model_id);
    	LOG_INFO("Vendor id is %x",evt->data.evt_mesh_node_model_config_changed.vendor_id);
        break;

    case gecko_evt_mesh_node_provisioning_started_id:
    	LOG_INFO("evt::gecko_evt_mesh_node_provisioning_started_id");
    	displayPrintf(DISPLAY_ROW_ACTION,"Provisioning");
    	break;

    case gecko_evt_mesh_node_provisioned_id:
    	LOG_INFO("evt::gecko_evt_mesh_node_provisioned_id");
    	displayPrintf(DISPLAY_ROW_ACTION, "Provisioned");
//    	displayPrintf(DISPLAY_ROW_PASSKEY,"Reset in 20 Sec");
//    	result = gecko_cmd_hardware_set_soft_timer(20 * 32768, TIMER_FACTORY_RESET, 1)->result;
//    	if(result) LOG_INFO("Reset failed\n\r");
    	break;

    case gecko_evt_mesh_node_provisioning_failed_id:
    	LOG_INFO("evt::gecko_evt_mesh_node_provisioning_failed_id");
    	displayPrintf(DISPLAY_ROW_ACTION, "Provisioning Failed");
    	gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_FACTORY_RESET, 1);
    	break;

    case gecko_evt_le_connection_opened_id:
    	LOG_INFO("evt::gecko_evt_le_connection_opened_id");
    	break;

    case gecko_evt_le_connection_closed_id:
    	LOG_INFO("evt::gecko_evt_le_connection_closed_id");
      /* Check if need to boot to dfu mode */
      if (boot_to_dfu) {
        /* Enter to DFU OTA mode */
        gecko_cmd_system_reset(2);
      }
      break;

    case gecko_evt_mesh_generic_server_client_request_id:
    	LOG_INFO("evt::gecko_evt_mesh_generic_server_client_request_id");
    	mesh_lib_generic_server_event_handler(evt);
    	break;

    case gecko_evt_mesh_generic_server_state_changed_id:
    	LOG_INFO("evt::gecko_evt_mesh_generic_server_state_changed_id");
    	mesh_lib_generic_server_event_handler(evt);
    	break;

    case gecko_evt_gatt_server_user_write_request_id:
    	LOG_INFO("evt::gecko_evt_gatt_server_user_write_request_id");
      if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control) {
        /* Set flag to enter to OTA mode */
        boot_to_dfu = 1;
        /* Send response to Write Request */
        gecko_cmd_gatt_server_send_user_write_response(
          evt->data.evt_gatt_server_user_write_request.connection,
          gattdb_ota_control,
          bg_err_success);

        /* Close connection to enter to DFU OTA mode */
        gecko_cmd_le_connection_close(evt->data.evt_gatt_server_user_write_request.connection);
      }
      break;

    case gecko_evt_mesh_friend_friendship_established_id:
    	LOG_INFO("evt::gecko_evt_mesh_friend_friendship_established_id");
    	LOG_INFO("lpn_address=%x", evt->data.evt_mesh_friend_friendship_established.lpn_address);
    	displayPrintf(DISPLAY_ROW_TEMPVALUE,"FRIEND");
    	break;

    case gecko_evt_mesh_friend_friendship_terminated_id:
    	LOG_INFO("evt::gecko_evt_mesh_friend_friendship_terminated_id");
    	LOG_INFO("reason=%x", evt->data.evt_mesh_friend_friendship_terminated.reason);
    	displayPrintf(DISPLAY_ROW_TEMPVALUE,"NO LPN");
    	break;

    case gecko_evt_system_external_signal_id:
    	LOG_INFO("evt::gecko_evt_system_external_signal_id");
    	//handle_external_events(evt->data.evt_system_external_signal.extsignals);
    	break;

    default:
      break;
  }
}


void data_subscriber_init()
{
	/* Initialize the server */
	struct gecko_msg_mesh_generic_server_init_rsp_t* res = gecko_cmd_mesh_generic_server_init();
	if(res->result != 0)
	{
		LOG_ERROR("Server init failed with %d response",res);
	}

	result = mesh_lib_init(malloc, free, 9);
	if(result != 0)
	{
		LOG_ERROR("Mesh lib init failed with %d response",result);
	}

	/* Initialize the friend */
	LOG_INFO("Initializing friend");
	struct gecko_msg_mesh_friend_init_rsp_t* res1 = gecko_cmd_mesh_friend_init();
	if(res1->result != 0)
	{
		LOG_ERROR("Friend init failed with %d response",res);
	}

	init_all_models();
	onoff_publish();
}

void init_all_models()
{
	errorcode_t mesh_reg_response = mesh_lib_generic_server_register_handler(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
																			 0,
																			 onoff_request,
																			 NULL);
	if(mesh_reg_response != 0)
	{
		LOG_ERROR("Handler register failed with %d response",mesh_reg_response);
	}
}

void onoff_publish()
{
	errorcode_t error = mesh_lib_generic_server_publish(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
	                                        			0,
														mesh_generic_state_on_off);
	if(error != 0)
	{
		LOG_ERROR("Server publish failed with %x response",error);
	}
}