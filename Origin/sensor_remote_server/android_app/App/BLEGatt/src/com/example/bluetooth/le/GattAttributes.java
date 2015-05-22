/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.example.bluetooth.le;

import java.util.HashMap;

/**
 * This class includes a subset of standard GATT attributes for demonstration
 * purposes.
 */
public class GattAttributes {
	private static HashMap<String, String> attributes = new HashMap();
	
	public static String DESC_CHARACTERISTIC_EXTENDED_PROPERTIES = "00002900-0000-1000-8000-00805f9b34fb";
	public static String DESC_CHARACTERISTIC_USER_DESCRIPTION = "00002901-0000-1000-8000-00805f9b34fb";
	public static String DESC_CLIENT_CHARACTERISTIC_CONFIGURATION = "00002902-0000-1000-8000-00805f9b34fb";
	public static String DESC_SERVER_CHARACTERISTIC_CONFIGURATION = "00002903-0000-1000-8000-00805f9b34fb";
	public static String DESC_CHARACTERISTIC_PRESENTATION_FORMAT = "00002904-0000-1000-8000-00805f9b34fb";
	public static String DESC_CHARACTERISTIC_AGGREGATE_FORMAT = "00002905-0000-1000-8000-00805f9b34fb";
	public static String DESC_VALID_RANGE = "00002906-0000-1000-8000-00805f9b34fb";
	public static String DESC_EXTERNAL_REPORT_REFERENCE = "00002907-0000-1000-8000-00805f9b34fb";
	public static String DESC_REPORT_REFERENCE = "00002908-0000-1000-8000-00805f9b34fb";


	public static String CH_DEVICE_NAME = "00002a00-0000-1000-8000-00805f9b34fb";
	public static String CH_APPEARANCE = "00002a01-0000-1000-8000-00805f9b34fb";
	public static String CH_PERIPHERAL_PRIVACY_FLAG = "00002a02-0000-1000-8000-00805f9b34fb";
	public static String CH_RECONNECTION_ADDRESS = "00002a03-0000-1000-8000-00805f9b34fb";
	public static String CH_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS = "00002a04-0000-1000-8000-00805f9b34fb";
	public static String CH_SERVICE_CHANGED = "00002a05-0000-1000-8000-00805f9b34fb";
	public static String CH_ALERT_LEVEL = "00002a06-0000-1000-8000-00805f9b34fb";
	public static String CH_TX_POWER_LEVEL = "00002a07-0000-1000-8000-00805f9b34fb";
	public static String CH_DATE_TIME = "00002a08-0000-1000-8000-00805f9b34fb";
	public static String CH_DAY_OF_WEEK = "00002a09-0000-1000-8000-00805f9b34fb";
	public static String CH_DAY_DATE_TIME = "00002a0a-0000-1000-8000-00805f9b34fb";
	public static String CH_EXACT_TIME_256 = "00002a0c-0000-1000-8000-00805f9b34fb";
	public static String CH_DST_OFFSET = "00002a0d-0000-1000-8000-00805f9b34fb";
	public static String CH_TIME_ZONE = "00002a0e-0000-1000-8000-00805f9b34fb";
	public static String CH_LOCAL_TIME_INFORMATION = "00002a0f-0000-1000-8000-00805f9b34fb";
	public static String CH_TIME_WITH_DST = "00002a11-0000-1000-8000-00805f9b34fb";
	public static String CH_TIME_ACCURACY = "00002a12-0000-1000-8000-00805f9b34fb";
	public static String CH_TIME_SOURCE = "00002a13-0000-1000-8000-00805f9b34fb";
	public static String CH_REFERENCE_TIME_INFORMATION = "00002a14-0000-1000-8000-00805f9b34fb";
	public static String CH_TIME_UPDATE_CONTROL_POINT = "00002a16-0000-1000-8000-00805f9b34fb";
	public static String CH_TIME_UPDATE_STATE = "00002a17-0000-1000-8000-00805f9b34fb";
	public static String CH_GLUCOSE_MEASUREMENT = "00002a18-0000-1000-8000-00805f9b34fb";
	public static String CH_BATTERY_LEVEL = "00002a19-0000-1000-8000-00805f9b34fb";
	public static String CH_TEMPERATURE_MEASUREMENT = "00002a1c-0000-1000-8000-00805f9b34fb";
	public static String CH_TEMPERATURE_TYPE = "00002a1d-0000-1000-8000-00805f9b34fb";
	public static String CH_INTERMEDIATE_TEMPERATURE = "00002a1e-0000-1000-8000-00805f9b34fb";
	public static String CH_MEASUREMENT_INTERVAL = "00002a21-0000-1000-8000-00805f9b34fb";
	public static String CH_BOOT_KEYBOARD_INPUT_REPORT = "00002a22-0000-1000-8000-00805f9b34fb";
	public static String CH_SYSTEM_ID = "00002a23-0000-1000-8000-00805f9b34fb";
	public static String CH_MODEL_NUMBER_STRING = "00002a24-0000-1000-8000-00805f9b34fb";
	public static String CH_SERIAL_NUMBER_STRING = "00002a25-0000-1000-8000-00805f9b34fb";
	public static String CH_FIRMWARE_REVISION_STRING = "00002a26-0000-1000-8000-00805f9b34fb";
	public static String CH_HARDWARE_REVISION_STRING = "00002a27-0000-1000-8000-00805f9b34fb";
	public static String CH_SOFTWARE_REVISION_STRING = "00002a28-0000-1000-8000-00805f9b34fb";
	public static String CH_MANUFACTURER_NAME_STRING = "00002a29-0000-1000-8000-00805f9b34fb";
	public static String CH_IEEE_11073_20601_REGULATORY_CERTIFICATION_DATA_LIST = "00002a2a-0000-1000-8000-00805f9b34fb";
	public static String CH_CURRENT_TIME = "00002a2b-0000-1000-8000-00805f9b34fb";
	public static String CH_SCAN_REFRESH = "00002a31-0000-1000-8000-00805f9b34fb";
	public static String CH_BOOT_KEYBOARD_OUTPUT_REPORT = "00002a32-0000-1000-8000-00805f9b34fb";
	public static String CH_BOOT_MOUSE_INPUT_REPORT = "00002a33-0000-1000-8000-00805f9b34fb";
	public static String CH_GLUCOSE_MEASUREMENT_CONTEXT = "00002a34-0000-1000-8000-00805f9b34fb";
	public static String CH_BLOOD_PRESSURE_MEASUREMENT = "00002a35-0000-1000-8000-00805f9b34fb";
	public static String CH_INTERMEDIATE_CUFF_PRESSURE = "00002a36-0000-1000-8000-00805f9b34fb";
	public static String CH_HEART_RATE_MEASUREMENT = "00002a37-0000-1000-8000-00805f9b34fb";
	public static String CH_BODY_SENSOR_LOCATION = "00002a38-0000-1000-8000-00805f9b34fb";
	public static String CH_HEART_RATE_CONTROL_POINT = "00002a39-0000-1000-8000-00805f9b34fb";
	public static String CH_ALERT_STATUS = "00002a3f-0000-1000-8000-00805f9b34fb";
	public static String CH_RINGER_CONTROL_POINT = "00002a40-0000-1000-8000-00805f9b34fb";
	public static String CH_RINGER_SETTING = "00002a41-0000-1000-8000-00805f9b34fb";
	public static String CH_ALERT_CATEGORY_ID_BIT_MASK = "00002a42-0000-1000-8000-00805f9b34fb";
	public static String CH_ALERT_CATEGORY_ID = "00002a43-0000-1000-8000-00805f9b34fb";
	public static String CH_ALERT_NOTIFICATION_CONTROL_POINT = "00002a44-0000-1000-8000-00805f9b34fb";
	public static String CH_UNREAD_ALERT_STATUS = "00002a45-0000-1000-8000-00805f9b34fb";
	public static String CH_NEW_ALERT = "00002a46-0000-1000-8000-00805f9b34fb";
	public static String CH_SUPPORTED_NEW_ALERT_CATEGORY = "00002a47-0000-1000-8000-00805f9b34fb";
	public static String CH_SUPPORTED_UNREAD_ALERT_CATEGORY = "00002a48-0000-1000-8000-00805f9b34fb";
	public static String CH_BLOOD_PRESSURE_FEATURE = "00002a49-0000-1000-8000-00805f9b34fb";
	public static String CH_HID_INFORMATION = "00002a4a-0000-1000-8000-00805f9b34fb";
	public static String CH_REPORT_MAP = "00002a4b-0000-1000-8000-00805f9b34fb";
	public static String CH_HID_CONTROL_POINT = "00002a4c-0000-1000-8000-00805f9b34fb";
	public static String CH_REPORT = "00002a4d-0000-1000-8000-00805f9b34fb";
	public static String CH_PROTOCOL_MODE = "00002a4e-0000-1000-8000-00805f9b34fb";
	public static String CH_SCAN_INTERVAL_WINDOW = "00002a4f-0000-1000-8000-00805f9b34fb";
	public static String CH_PNP_ID = "00002a50-0000-1000-8000-00805f9b34fb";
	public static String CH_GLUCOSE_FEATURE = "00002a51-0000-1000-8000-00805f9b34fb";
	public static String CH_RECORD_ACCESS_CONTROL_POINT = "00002a52-0000-1000-8000-00805f9b34fb";
	public static String CH_RSC_MEASUREMENT = "00002a53-0000-1000-8000-00805f9b34fb";
	public static String CH_RSC_FEATURE = "00002a54-0000-1000-8000-00805f9b34fb";
	public static String CH_SC_CONTROL_POINT = "00002a55-0000-1000-8000-00805f9b34fb";
	public static String CH_CSC_MEASUREMENT = "00002a5b-0000-1000-8000-00805f9b34fb";
	public static String CH_CSC_FEATURE = "00002a5c-0000-1000-8000-00805f9b34fb";
	public static String CH_SENSOR_LOCATION = "00002a5d-0000-1000-8000-00805f9b34fb";
	public static String CH_CYCLING_POWER_MEASUREMENT = "00002a63-0000-1000-8000-00805f9b34fb";
	public static String CH_CYCLING_POWER_VECTOR = "00002a64-0000-1000-8000-00805f9b34fb";
	public static String CH_CYCLING_POWER_FEATURE = "00002a65-0000-1000-8000-00805f9b34fb";
	public static String CH_CYCLING_POWER_CONTROL_POINT = "00002a66-0000-1000-8000-00805f9b34fb";
	public static String CH_LOCATION_AND_SPEED = "00002a67-0000-1000-8000-00805f9b34fb";
	public static String CH_NAVIGATION = "00002a68-0000-1000-8000-00805f9b34fb";
	public static String CH_POSITION_QUALITY = "00002a69-0000-1000-8000-00805f9b34fb";
	public static String CH_LN_FEATURE = "00002a6a-0000-1000-8000-00805f9b34fb";
	public static String CH_LN_CONTROL_POINT = "00002a6b-0000-1000-8000-00805f9b34fb";

	public static String CH_RCS_RAW_DATA_WRITE = "6a652ce5-c50a-4157-b990-d4e7d9cc0a53";
	public static String CH_RCS_RAW_DATA_READ = "d195b840-0333-4e7a-8e0d-429499d3baa1";

	
	static {
		// Services
		// https://developer.bluetooth.org/gatt/services/Pages/ServicesHome.aspx
		attributes.put("00001811-0000-1000-8000-00805f9b34fb", "Alert Notification Service");
		attributes.put("0000180f-0000-1000-8000-00805f9b34fb", "Battery Service");
		attributes.put("00001810-0000-1000-8000-00805f9b34fb", "Blood Pressure");
		attributes.put("00001805-0000-1000-8000-00805f9b34fb", "Current Time Service");
		attributes.put("00001818-0000-1000-8000-00805f9b34fb", "Cycling Power");
		attributes.put("00001816-0000-1000-8000-00805f9b34fb", "Cycling Speed and Cadence");
		attributes.put("0000180a-0000-1000-8000-00805f9b34fb", "Device Information");
		attributes.put("00001800-0000-1000-8000-00805f9b34fb", "Generic Access");
		attributes.put("00001801-0000-1000-8000-00805f9b34fb", "Generic Attribute");
		attributes.put("00001808-0000-1000-8000-00805f9b34fb", "Glucose");
		attributes.put("00001809-0000-1000-8000-00805f9b34fb", "Health Thermometer");
		attributes.put("0000180d-0000-1000-8000-00805f9b34fb", "Heart Rate");
		attributes.put("00001812-0000-1000-8000-00805f9b34fb", "Human Interface Device");
		attributes.put("00001803-0000-1000-8000-00805f9b34fb", "Immediate Alert");
		attributes.put("00001803-0000-1000-8000-00805f9b34fb", "Link Loss");
		attributes.put("00001819-0000-1000-8000-00805f9b34fb", "Location and Navigation");
		attributes.put("00001807-0000-1000-8000-00805f9b34fb", "Next DST Change Service");
		attributes.put("0000180e-0000-1000-8000-00805f9b34fb", "Phone Alert Status Service");
		attributes.put("00001806-0000-1000-8000-00805f9b34fb", "Reference Time Update Service");
		attributes.put("00001814-0000-1000-8000-00805f9b34fb", "Running Speed and Cadence");
		attributes.put("00001813-0000-1000-8000-00805f9b34fb", "Scan Parameters");
		attributes.put("00001804-0000-1000-8000-00805f9b34fb", "Tx Power");
		
		attributes.put("fc3b898f-fefa-4fb5-97dc-df8fab78e93e", "RCS Raw Data Transmission");
		
		
		// Characteristics.
		// https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicsHome.aspx
		attributes.put(CH_ALERT_CATEGORY_ID, "Alert Category ID");
		attributes.put(CH_ALERT_CATEGORY_ID_BIT_MASK, "Alert Category ID Bit Mask");
		attributes.put(CH_ALERT_LEVEL, "Alert Level");
		attributes.put(CH_ALERT_NOTIFICATION_CONTROL_POINT, "Alert Notification Control Point");
		attributes.put(CH_ALERT_STATUS, "Alert Status");
		attributes.put(CH_APPEARANCE, "Appearance");
		attributes.put(CH_BATTERY_LEVEL, "Battery Level");
		attributes.put(CH_BLOOD_PRESSURE_FEATURE, "Blood Pressure Feature");
		attributes.put(CH_BLOOD_PRESSURE_MEASUREMENT, "Blood Pressure Measurement");
		attributes.put(CH_BODY_SENSOR_LOCATION, "Body Sensor Location");
		attributes.put(CH_BOOT_KEYBOARD_INPUT_REPORT, "Boot Keyboard Input Report");
		attributes.put(CH_BOOT_KEYBOARD_OUTPUT_REPORT, "Boot Keyboard Output Report");
		attributes.put(CH_BOOT_MOUSE_INPUT_REPORT, "Boot Mouse Input Report");
		attributes.put(CH_CSC_FEATURE, "CSC Feature");
		attributes.put(CH_CSC_MEASUREMENT, "CSC Measurement");
		attributes.put(CH_CURRENT_TIME, "Current Time");
		attributes.put(CH_CYCLING_POWER_CONTROL_POINT, "Cycling Power Control Point");
		attributes.put(CH_CYCLING_POWER_FEATURE, "Cycling Power Feature");
		attributes.put(CH_CYCLING_POWER_MEASUREMENT, "Cycling Power Measurement");
		attributes.put(CH_CYCLING_POWER_VECTOR, "Cycling Power Vector");
		attributes.put(CH_DATE_TIME, "Date Time");
		attributes.put(CH_DAY_DATE_TIME, "Day Date Time");
		attributes.put(CH_DAY_OF_WEEK, "Day of Week");
		attributes.put(CH_DEVICE_NAME, "Device Name");
		attributes.put(CH_DST_OFFSET, "DST Offset");
		attributes.put(CH_EXACT_TIME_256, "Exact Time 256");
		attributes.put(CH_FIRMWARE_REVISION_STRING, "Firmware Revision String");
		attributes.put(CH_GLUCOSE_FEATURE, "Glucose Feature");
		attributes.put(CH_GLUCOSE_MEASUREMENT, "Glucose Measurement");
		attributes.put(CH_GLUCOSE_MEASUREMENT_CONTEXT, "Glucose Measurement Context");
		attributes.put(CH_HARDWARE_REVISION_STRING, "Hardware Revision String");
		attributes.put(CH_HEART_RATE_CONTROL_POINT, "Heart Rate Control Point");
		attributes.put(CH_HEART_RATE_MEASUREMENT, "Heart Rate Measurement");
		attributes.put(CH_HID_CONTROL_POINT, "HID Control Point");
		attributes.put(CH_HID_INFORMATION, "HID Information");
		attributes.put(CH_IEEE_11073_20601_REGULATORY_CERTIFICATION_DATA_LIST, "IEEE 11073-20601 Regulatory Certification Data List");
		attributes.put(CH_INTERMEDIATE_CUFF_PRESSURE, "Intermediate Cuff Pressure");
		attributes.put(CH_INTERMEDIATE_TEMPERATURE, "Intermediate Temperature");
		attributes.put(CH_LN_CONTROL_POINT, "LN Control Point");
		attributes.put(CH_LN_FEATURE, "LN Feature");
		attributes.put(CH_LOCAL_TIME_INFORMATION, "Local Time Information");
		attributes.put(CH_LOCATION_AND_SPEED, "Location and Speed");
		attributes.put(CH_MANUFACTURER_NAME_STRING, "Manufacturer Name String");
		attributes.put(CH_MEASUREMENT_INTERVAL, "Measurement Interval");
		attributes.put(CH_MODEL_NUMBER_STRING, "Model Number String");
		attributes.put(CH_NAVIGATION, "Navigation");
		attributes.put(CH_NEW_ALERT, "New Alert");
		attributes.put(CH_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS, "Peripheral Preferred Connection Parameters");
		attributes.put(CH_PERIPHERAL_PRIVACY_FLAG, "Peripheral Privacy Flag");
		attributes.put(CH_PNP_ID, "PnP ID");
		attributes.put(CH_POSITION_QUALITY, "Position Quality");
		attributes.put(CH_PROTOCOL_MODE, "Protocol Mode");
		attributes.put(CH_RECONNECTION_ADDRESS, "Reconnection Address");
		attributes.put(CH_RECORD_ACCESS_CONTROL_POINT, "Record Access Control Point");
		attributes.put(CH_REFERENCE_TIME_INFORMATION, "Reference Time Information");
		attributes.put(CH_REPORT, "Report");
		attributes.put(CH_REPORT_MAP, "Report Map");
		attributes.put(CH_RINGER_CONTROL_POINT, "Ringer Control Point");
		attributes.put(CH_RINGER_SETTING, "Ringer Setting");
		attributes.put(CH_RSC_FEATURE, "RSC Feature");
		attributes.put(CH_RSC_MEASUREMENT, "RSC Measurement");
		attributes.put(CH_SC_CONTROL_POINT, "SC Control Point");
		attributes.put(CH_SCAN_INTERVAL_WINDOW, "Scan Interval Window");
		attributes.put(CH_SCAN_REFRESH, "Scan Refresh");
		attributes.put(CH_SENSOR_LOCATION, "Sensor Location");
		attributes.put(CH_SERIAL_NUMBER_STRING, "Serial Number String");
		attributes.put(CH_SERVICE_CHANGED, "Service Changed");
		attributes.put(CH_SOFTWARE_REVISION_STRING, "Software Revision String");
		attributes.put(CH_SUPPORTED_NEW_ALERT_CATEGORY, "Supported New Alert Category");
		attributes.put(CH_SUPPORTED_UNREAD_ALERT_CATEGORY, "Supported Unread Alert Category");
		attributes.put(CH_SYSTEM_ID, "System ID");
		attributes.put(CH_TEMPERATURE_MEASUREMENT, "Temperature Measurement");
		attributes.put(CH_TEMPERATURE_TYPE, "Temperature Type");
		attributes.put(CH_TIME_ACCURACY, "Time Accuracy");
		attributes.put(CH_TIME_SOURCE, "Time Source");
		attributes.put(CH_TIME_UPDATE_CONTROL_POINT, "Time Update Control Point");
		attributes.put(CH_TIME_UPDATE_STATE, "Time Update State");
		attributes.put(CH_TIME_WITH_DST, "Time with DST");
		attributes.put(CH_TIME_ZONE, "Time Zone");
		attributes.put(CH_TX_POWER_LEVEL, "Tx Power Level");
		attributes.put(CH_UNREAD_ALERT_STATUS, "Unread Alert Status");
		
		attributes.put(CH_RCS_RAW_DATA_WRITE, "Raw Data Write");
		attributes.put(CH_RCS_RAW_DATA_READ, "Raw Data Read");

	}

	public static String lookup(String uuid, String defaultName) {
		String name = attributes.get(uuid);
		return name == null ? defaultName : name;
	}
}
