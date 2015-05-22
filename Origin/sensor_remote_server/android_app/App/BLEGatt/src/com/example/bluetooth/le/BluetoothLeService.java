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

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattServer;
import android.bluetooth.BluetoothGattServerCallback;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

/**
 * Service for managing connection and data communication with a GATT server
 * hosted on a given Bluetooth LE device.
 */
public class BluetoothLeService extends Service {
	private final static String TAG = BluetoothLeService.class.getSimpleName();

	private BluetoothManager mBluetoothManager;
	private BluetoothAdapter mBluetoothAdapter;
	private String mBluetoothDeviceAddress;
	private BluetoothGatt mBluetoothGatt;
	private BluetoothGattServer mBluetoothGattServer;
	private int mConnectionState = STATE_DISCONNECTED;

	private static final int STATE_DISCONNECTED = 0;
	private static final int STATE_CONNECTING = 1;
	private static final int STATE_CONNECTED = 2;

	public final static String ACTION_GATT_CONNECTED = "com.example.bluetooth.le.ACTION_GATT_CONNECTED";
	public final static String ACTION_GATT_DISCONNECTED = "com.example.bluetooth.le.ACTION_GATT_DISCONNECTED";
	public final static String ACTION_GATT_SERVICES_DISCOVERED = "com.example.bluetooth.le.ACTION_GATT_SERVICES_DISCOVERED";
	public final static String ACTION_DATA_AVAILABLE = "com.example.bluetooth.le.ACTION_DATA_AVAILABLE";
	public final static String EXTRA_DATA = "com.example.bluetooth.le.EXTRA_DATA";

	public final static UUID UUID_ALERT_CATEGORY_ID = UUID.fromString(GattAttributes.CH_ALERT_CATEGORY_ID);
	public final static UUID UUID_ALERT_CATEGORY_ID_BIT_MASK = UUID.fromString(GattAttributes.CH_ALERT_CATEGORY_ID_BIT_MASK);
	public final static UUID UUID_ALERT_LEVEL = UUID.fromString(GattAttributes.CH_ALERT_LEVEL);
	public final static UUID UUID_ALERT_NOTIFICATION_CONTROL_POINT = UUID.fromString(GattAttributes.CH_ALERT_NOTIFICATION_CONTROL_POINT);
	public final static UUID UUID_ALERT_STATUS = UUID.fromString(GattAttributes.CH_ALERT_STATUS);
	public final static UUID UUID_APPEARANCE = UUID.fromString(GattAttributes.CH_APPEARANCE);
	public final static UUID UUID_BATTERY_LEVEL = UUID.fromString(GattAttributes.CH_BATTERY_LEVEL);
	public final static UUID UUID_BLOOD_PRESSURE_FEATURE = UUID.fromString(GattAttributes.CH_BLOOD_PRESSURE_FEATURE);
	public final static UUID UUID_BLOOD_PRESSURE_MEASUREMENT = UUID.fromString(GattAttributes.CH_BLOOD_PRESSURE_MEASUREMENT);
	public final static UUID UUID_BODY_SENSOR_LOCATION = UUID.fromString(GattAttributes.CH_BODY_SENSOR_LOCATION);
	public final static UUID UUID_BOOT_KEYBOARD_INPUT_REPORT = UUID.fromString(GattAttributes.CH_BOOT_KEYBOARD_INPUT_REPORT);
	public final static UUID UUID_BOOT_KEYBOARD_OUTPUT_REPORT = UUID.fromString(GattAttributes.CH_BOOT_KEYBOARD_OUTPUT_REPORT);
	public final static UUID UUID_BOOT_MOUSE_INPUT_REPORT = UUID.fromString(GattAttributes.CH_BOOT_MOUSE_INPUT_REPORT);
	public final static UUID UUID_CSC_FEATURE = UUID.fromString(GattAttributes.CH_CSC_FEATURE);
	public final static UUID UUID_CSC_MEASUREMENT = UUID.fromString(GattAttributes.CH_CSC_MEASUREMENT);
	public final static UUID UUID_CURRENT_TIME = UUID.fromString(GattAttributes.CH_CURRENT_TIME);
	public final static UUID UUID_CYCLING_POWER_CONTROL_POINT = UUID.fromString(GattAttributes.CH_CYCLING_POWER_CONTROL_POINT);
	public final static UUID UUID_CYCLING_POWER_FEATURE = UUID.fromString(GattAttributes.CH_CYCLING_POWER_FEATURE);
	public final static UUID UUID_CYCLING_POWER_MEASUREMENT = UUID.fromString(GattAttributes.CH_CYCLING_POWER_MEASUREMENT);
	public final static UUID UUID_CYCLING_POWER_VECTOR = UUID.fromString(GattAttributes.CH_CYCLING_POWER_VECTOR);
	public final static UUID UUID_DATE_TIME = UUID.fromString(GattAttributes.CH_DATE_TIME);
	public final static UUID UUID_DAY_DATE_TIME = UUID.fromString(GattAttributes.CH_DAY_DATE_TIME);
	public final static UUID UUID_DAY_OF_WEEK = UUID.fromString(GattAttributes.CH_DAY_OF_WEEK);
	public final static UUID UUID_DEVICE_NAME = UUID.fromString(GattAttributes.CH_DEVICE_NAME);
	public final static UUID UUID_DST_OFFSET = UUID.fromString(GattAttributes.CH_DST_OFFSET);
	public final static UUID UUID_EXACT_TIME_256 = UUID.fromString(GattAttributes.CH_EXACT_TIME_256);
	public final static UUID UUID_FIRMWARE_REVISION_STRING = UUID.fromString(GattAttributes.CH_FIRMWARE_REVISION_STRING);
	public final static UUID UUID_GLUCOSE_FEATURE = UUID.fromString(GattAttributes.CH_GLUCOSE_FEATURE);
	public final static UUID UUID_GLUCOSE_MEASUREMENT = UUID.fromString(GattAttributes.CH_GLUCOSE_MEASUREMENT);
	public final static UUID UUID_GLUCOSE_MEASUREMENT_CONTEXT = UUID.fromString(GattAttributes.CH_GLUCOSE_MEASUREMENT_CONTEXT);
	public final static UUID UUID_HARDWARE_REVISION_STRING = UUID.fromString(GattAttributes.CH_HARDWARE_REVISION_STRING);
	public final static UUID UUID_HEART_RATE_CONTROL_POINT = UUID.fromString(GattAttributes.CH_HEART_RATE_CONTROL_POINT);
	public final static UUID UUID_HEART_RATE_MEASUREMENT = UUID.fromString(GattAttributes.CH_HEART_RATE_MEASUREMENT);
	public final static UUID UUID_HID_CONTROL_POINT = UUID.fromString(GattAttributes.CH_HID_CONTROL_POINT);
	public final static UUID UUID_HID_INFORMATION = UUID.fromString(GattAttributes.CH_HID_INFORMATION);
	public final static UUID UUID_IEEE_11073_20601_REGULATORY_CERTIFICATION_DATA_LIST = UUID
			.fromString(GattAttributes.CH_IEEE_11073_20601_REGULATORY_CERTIFICATION_DATA_LIST);
	public final static UUID UUID_INTERMEDIATE_CUFF_PRESSURE = UUID.fromString(GattAttributes.CH_INTERMEDIATE_CUFF_PRESSURE);
	public final static UUID UUID_INTERMEDIATE_TEMPERATURE = UUID.fromString(GattAttributes.CH_INTERMEDIATE_TEMPERATURE);
	public final static UUID UUID_LN_CONTROL_POINT = UUID.fromString(GattAttributes.CH_LN_CONTROL_POINT);
	public final static UUID UUID_LN_FEATURE = UUID.fromString(GattAttributes.CH_LN_FEATURE);
	public final static UUID UUID_LOCAL_TIME_INFORMATION = UUID.fromString(GattAttributes.CH_LOCAL_TIME_INFORMATION);
	public final static UUID UUID_LOCATION_AND_SPEED = UUID.fromString(GattAttributes.CH_LOCATION_AND_SPEED);
	public final static UUID UUID_MANUFACTURER_NAME_STRING = UUID.fromString(GattAttributes.CH_MANUFACTURER_NAME_STRING);
	public final static UUID UUID_MEASUREMENT_INTERVAL = UUID.fromString(GattAttributes.CH_MEASUREMENT_INTERVAL);
	public final static UUID UUID_MODEL_NUMBER_STRING = UUID.fromString(GattAttributes.CH_MODEL_NUMBER_STRING);
	public final static UUID UUID_NAVIGATION = UUID.fromString(GattAttributes.CH_NAVIGATION);
	public final static UUID UUID_NEW_ALERT = UUID.fromString(GattAttributes.CH_NEW_ALERT);
	public final static UUID UUID_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS = UUID.fromString(GattAttributes.CH_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS);
	public final static UUID UUID_PERIPHERAL_PRIVACY_FLAG = UUID.fromString(GattAttributes.CH_PERIPHERAL_PRIVACY_FLAG);
	public final static UUID UUID_PNP_ID = UUID.fromString(GattAttributes.CH_PNP_ID);
	public final static UUID UUID_POSITION_QUALITY = UUID.fromString(GattAttributes.CH_POSITION_QUALITY);
	public final static UUID UUID_PROTOCOL_MODE = UUID.fromString(GattAttributes.CH_PROTOCOL_MODE);
	public final static UUID UUID_RECONNECTION_ADDRESS = UUID.fromString(GattAttributes.CH_RECONNECTION_ADDRESS);
	public final static UUID UUID_RECORD_ACCESS_CONTROL_POINT = UUID.fromString(GattAttributes.CH_RECORD_ACCESS_CONTROL_POINT);
	public final static UUID UUID_REFERENCE_TIME_INFORMATION = UUID.fromString(GattAttributes.CH_REFERENCE_TIME_INFORMATION);
	public final static UUID UUID_REPORT = UUID.fromString(GattAttributes.CH_REPORT);
	public final static UUID UUID_REPORT_MAP = UUID.fromString(GattAttributes.CH_REPORT_MAP);
	public final static UUID UUID_RINGER_CONTROL_POINT = UUID.fromString(GattAttributes.CH_RINGER_CONTROL_POINT);
	public final static UUID UUID_RINGER_SETTING = UUID.fromString(GattAttributes.CH_RINGER_SETTING);
	public final static UUID UUID_RSC_FEATURE = UUID.fromString(GattAttributes.CH_RSC_FEATURE);
	public final static UUID UUID_RSC_MEASUREMENT = UUID.fromString(GattAttributes.CH_RSC_MEASUREMENT);
	public final static UUID UUID_SC_CONTROL_POINT = UUID.fromString(GattAttributes.CH_SC_CONTROL_POINT);
	public final static UUID UUID_SCAN_INTERVAL_WINDOW = UUID.fromString(GattAttributes.CH_SCAN_INTERVAL_WINDOW);
	public final static UUID UUID_SCAN_REFRESH = UUID.fromString(GattAttributes.CH_SCAN_REFRESH);
	public final static UUID UUID_SENSOR_LOCATION = UUID.fromString(GattAttributes.CH_SENSOR_LOCATION);
	public final static UUID UUID_SERIAL_NUMBER_STRING = UUID.fromString(GattAttributes.CH_SERIAL_NUMBER_STRING);
	public final static UUID UUID_SERVICE_CHANGED = UUID.fromString(GattAttributes.CH_SERVICE_CHANGED);
	public final static UUID UUID_SOFTWARE_REVISION_STRING = UUID.fromString(GattAttributes.CH_SOFTWARE_REVISION_STRING);
	public final static UUID UUID_SUPPORTED_NEW_ALERT_CATEGORY = UUID.fromString(GattAttributes.CH_SUPPORTED_NEW_ALERT_CATEGORY);
	public final static UUID UUID_SUPPORTED_UNREAD_ALERT_CATEGORY = UUID.fromString(GattAttributes.CH_SUPPORTED_UNREAD_ALERT_CATEGORY);
	public final static UUID UUID_SYSTEM_ID = UUID.fromString(GattAttributes.CH_SYSTEM_ID);
	public final static UUID UUID_TEMPERATURE_MEASUREMENT = UUID.fromString(GattAttributes.CH_TEMPERATURE_MEASUREMENT);
	public final static UUID UUID_TEMPERATURE_TYPE = UUID.fromString(GattAttributes.CH_TEMPERATURE_TYPE);
	public final static UUID UUID_TIME_ACCURACY = UUID.fromString(GattAttributes.CH_TIME_ACCURACY);
	public final static UUID UUID_TIME_SOURCE = UUID.fromString(GattAttributes.CH_TIME_SOURCE);
	public final static UUID UUID_TIME_UPDATE_CONTROL_POINT = UUID.fromString(GattAttributes.CH_TIME_UPDATE_CONTROL_POINT);
	public final static UUID UUID_TIME_UPDATE_STATE = UUID.fromString(GattAttributes.CH_TIME_UPDATE_STATE);
	public final static UUID UUID_TIME_WITH_DST = UUID.fromString(GattAttributes.CH_TIME_WITH_DST);
	public final static UUID UUID_TIME_ZONE = UUID.fromString(GattAttributes.CH_TIME_ZONE);
	public final static UUID UUID_TX_POWER_LEVEL = UUID.fromString(GattAttributes.CH_TX_POWER_LEVEL);
	public final static UUID UUID_UNREAD_ALERT_STATUS = UUID.fromString(GattAttributes.CH_UNREAD_ALERT_STATUS);

	public final static UUID UUID_RCS_RAW_DATA_READ = UUID.fromString(GattAttributes.CH_RCS_RAW_DATA_READ);
	public final static UUID UUID_RCS_RAW_DATA_WRITE = UUID.fromString(GattAttributes.CH_RCS_RAW_DATA_WRITE);

	
	
	// Data Provider
	private ArrayList<OnDataAvailableListener> listOfRegisteredListeners = new ArrayList<OnDataAvailableListener>();	
	public static interface OnDataAvailableListener {
		void onDataAvailable(byte[] bytes);
	}
	public void unregisterListener(OnDataAvailableListener l) {
		listOfRegisteredListeners.remove(l);
	}
	public void registerListener(OnDataAvailableListener l) {
		listOfRegisteredListeners.add(l);
	}
	private void notifyListeners(byte[] bytes) {
		for (OnDataAvailableListener l : listOfRegisteredListeners) {
			l.onDataAvailable(bytes);
		}
	}

	
	
	// Implements callback methods for GATT events that the app cares about. For
	// example,
	// connection change and services discovered.
	private final BluetoothGattCallback mGattCallback = new BluetoothGattCallback() {
		@Override
		public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
			String intentAction;
			if (newState == BluetoothProfile.STATE_CONNECTED) {
				intentAction = ACTION_GATT_CONNECTED;
				mConnectionState = STATE_CONNECTED;
				broadcastUpdate(intentAction);
				Log.i(TAG, "Connected to GATT server.");
				// Attempts to discover services after successful connection.
				Log.i(TAG, "Attempting to start service discovery:" + mBluetoothGatt.discoverServices());

			} else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
				intentAction = ACTION_GATT_DISCONNECTED;
				mConnectionState = STATE_DISCONNECTED;
				Log.i(TAG, "Disconnected from GATT server.");
				broadcastUpdate(intentAction);
			}
		}

		@Override
		public void onDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {

			Log.d(TAG, "onDescriptorWrite() callback: " + descriptor.toString() + ": Status " + status);

			super.onDescriptorWrite(gatt, descriptor, status);
		}

		@Override
		public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {

			Log.d(TAG, "onCharacteristicWrite() callback: " + characteristic.toString() + ": Status " + status);

			super.onCharacteristicWrite(gatt, characteristic, status);
		}

		@Override
		public void onServicesDiscovered(BluetoothGatt gatt, int status) {
			if (status == BluetoothGatt.GATT_SUCCESS) {
				broadcastUpdate(ACTION_GATT_SERVICES_DISCOVERED);

				// Log.d(TAG,
				// "Send DESC_CLIENT_CHARACTERISTIC_CONFIGURATION 3");
				// BluetoothGattDescriptor desc = new
				// BluetoothGattDescriptor(UUID.fromString(GattAttributes.DESC_CLIENT_CHARACTERISTIC_CONFIGURATION),
				// BluetoothGattDescriptor.PERMISSION_WRITE);
				// desc.setValue(new byte[]{0x03}); // enable notifications &
				// indications
				// mBluetoothGatt.writeDescriptor(desc);

			} else {
				Log.w(TAG, "onServicesDiscovered received: " + status);
			}
		}

		@Override
		public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
			if (status == BluetoothGatt.GATT_SUCCESS) {
				broadcastUpdate(ACTION_DATA_AVAILABLE, characteristic);
			}
		}

		@Override
		public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
			broadcastUpdate(ACTION_DATA_AVAILABLE, characteristic);
		}
	};

	private void broadcastUpdate(final String action) {
		final Intent intent = new Intent(action);
		sendBroadcast(intent);
	}

	private void broadcastUpdate(final String action, final BluetoothGattCharacteristic characteristic) {
		final Intent intent = new Intent(action);

		// This is special handling for the Heart Rate Measurement profile. Data
		// parsing is
		// carried out as per profile specifications:
		// http://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.heart_rate_measurement.xml
		if (UUID_HEART_RATE_MEASUREMENT.equals(characteristic.getUuid())) {
			int flag = characteristic.getProperties();
			int format = -1;
			if ((flag & 0x01) != 0) {
				format = BluetoothGattCharacteristic.FORMAT_UINT16;
				Log.d(TAG, "Heart rate format UINT16.");
			} else {
				format = BluetoothGattCharacteristic.FORMAT_UINT8;
				Log.d(TAG, "Heart rate format UINT8.");
			}
			final int heartRate = characteristic.getIntValue(format, 1);
			Log.d(TAG, String.format("Received heart rate: %d", heartRate));
			intent.putExtra(EXTRA_DATA, String.valueOf(heartRate));
		} else if (UUID_RCS_RAW_DATA_READ.equals(characteristic.getUuid())) {
			final byte[] data = characteristic.getValue();
			int prop = characteristic.getProperties();
			if (data != null && data.length > 0) {
				final StringBuilder stringBuilder = new StringBuilder(data.length);
				for (byte byteChar : data)
					stringBuilder.append(String.format("%02X ", byteChar));
				intent.putExtra(EXTRA_DATA, "\"" + new String(data) + "\"<br><i>hex[" + data.length + "]:</i> " + stringBuilder.toString());
			} else if (data == null) {
				intent.putExtra(EXTRA_DATA, "characteristic.getValue() returns (null)<br/>" + String.format("Properties: 0x%02X", prop));
			} else if (data.length == 0) {
				intent.putExtra(EXTRA_DATA, "data length is 0<br/>" + String.format("Properties: 0x%02X", prop));
			}
		} else {
			// For all other profiles, writes the data formatted in HEX.
			final byte[] data = characteristic.getValue();
			if (data != null && data.length > 0) {
				final StringBuilder stringBuilder = new StringBuilder(data.length);
				for (byte byteChar : data)
					stringBuilder.append(String.format("%02X ", byteChar));
				intent.putExtra(EXTRA_DATA, "\"" + new String(data) + "\"<br><i>hex[" + data.length + "]:</i> " + stringBuilder.toString());
			}
		}
		sendBroadcast(intent);
	}

	public class LocalBinder extends Binder {
		BluetoothLeService getService() {
			return BluetoothLeService.this;
		}
	}

	@Override
	public IBinder onBind(Intent intent) {
		return mBinder;
	}

	@Override
	public boolean onUnbind(Intent intent) {
		// After using a given device, you should make sure that
		// BluetoothGatt.close() is called
		// such that resources are cleaned up properly. In this particular
		// example, close() is
		// invoked when the UI is disconnected from the Service.
		close();
		return super.onUnbind(intent);
	}

	private final IBinder mBinder = new LocalBinder();

	/**
	 * Initializes a reference to the local Bluetooth adapter.
	 * 
	 * @return Return true if the initialization is successful.
	 */
	public boolean initialize() {
		// For API level 18 and above, get a reference to BluetoothAdapter
		// through
		// BluetoothManager.
		if (mBluetoothManager == null) {
			mBluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
			if (mBluetoothManager == null) {
				Log.e(TAG, "Unable to initialize BluetoothManager.");
				return false;
			}
		}

		mBluetoothAdapter = mBluetoothManager.getAdapter();
		if (mBluetoothAdapter == null) {
			Log.e(TAG, "Unable to obtain a BluetoothAdapter.");
			return false;
		}

		return true;
	}

	/**
	 * Connects to the GATT server hosted on the Bluetooth LE device.
	 * 
	 * @param address
	 *            The device address of the destination device.
	 * 
	 * @return Return true if the connection is initiated successfully. The
	 *         connection result is reported asynchronously through the
	 *         {@code BluetoothGattCallback#onConnectionStateChange(android.bluetooth.BluetoothGatt, int, int)}
	 *         callback.
	 */
	public boolean connect(final String address) {
		if (mBluetoothAdapter == null || address == null) {
			Log.w(TAG, "BluetoothAdapter not initialized or unspecified address.");
			return false;
		}

		// Previously connected device. Try to reconnect.
		if (mBluetoothDeviceAddress != null && address.equals(mBluetoothDeviceAddress) && mBluetoothGatt != null) {
			Log.d(TAG, "Trying to use an existing mBluetoothGatt for connection.");
			if (mBluetoothGatt.connect()) {
				mConnectionState = STATE_CONNECTING;
				return true;
			} else {
				return false;
			}
		}

		final BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
		if (device == null) {
			Log.w(TAG, "Device not found.  Unable to connect.");
			return false;
		}
		// We want to directly connect to the device, so we are setting the
		// autoConnect
		// parameter to false.
		mBluetoothGatt = device.connectGatt(this, false, mGattCallback);
		Log.d(TAG, "Trying to create a new connection.");
		mBluetoothDeviceAddress = address;
		mConnectionState = STATE_CONNECTING;
		return true;
	}

	/**
	 * Disconnects an existing connection or cancel a pending connection. The
	 * disconnection result is reported asynchronously through the
	 * {@code BluetoothGattCallback#onConnectionStateChange(android.bluetooth.BluetoothGatt, int, int)}
	 * callback.
	 */
	public void disconnect() {
		if (mBluetoothAdapter == null || mBluetoothGatt == null) {
			Log.w(TAG, "BluetoothAdapter not initialized");
			return;
		}
		mBluetoothGatt.disconnect();
	}

	/**
	 * After using a given BLE device, the app must call this method to ensure
	 * resources are released properly.
	 */
	public void close() {
		if (mBluetoothGatt == null) {
			return;
		}
		mBluetoothGatt.close();
		mBluetoothGatt = null;
	}

	/**
	 * Request a read on a given {@code BluetoothGattCharacteristic}. The read
	 * result is reported asynchronously through the
	 * {@code BluetoothGattCallback#onCharacteristicRead(android.bluetooth.BluetoothGatt, android.bluetooth.BluetoothGattCharacteristic, int)}
	 * callback.
	 * 
	 * @param characteristic
	 *            The characteristic to read from.
	 */
	public void readCharacteristic(BluetoothGattCharacteristic characteristic) {
		if (mBluetoothAdapter == null || mBluetoothGatt == null) {
			Log.w(TAG, "BluetoothAdapter not initialized");
			return;
		}
		mBluetoothGatt.readCharacteristic(characteristic);
	}

	public boolean writeCharacteristic(BluetoothGattCharacteristic characteristic) {
		if (mBluetoothAdapter == null || mBluetoothGatt == null) {
			Log.w(TAG, "BluetoothAdapter not initialized");
			return false;
		}
		return mBluetoothGatt.writeCharacteristic(characteristic);
	}

	/**
	 * Enables or disables notification on a give characteristic.
	 * 
	 * @param characteristic
	 *            Characteristic to act on.
	 * @param enabled
	 *            If true, enable notification. False otherwise.
	 */
	public void setCharacteristicNotification(BluetoothGattCharacteristic characteristic, boolean enabled) {
		if (mBluetoothAdapter == null || mBluetoothGatt == null) {
			Log.w(TAG, "BluetoothAdapter not initialized");
			return;
		}
		mBluetoothGatt.setCharacteristicNotification(characteristic, enabled);

		// if (UUID_HEART_RATE_MEASUREMENT.equals(characteristic.getUuid()) ||
		// UUID_RCS_RAW_DATA_READ.equals(characteristic.getUuid())) {
		// BluetoothGattDescriptor descriptor =
		// characteristic.getDescriptor(UUID.fromString(GattAttributes.DESC_CLIENT_CHARACTERISTIC_CONFIGURATION));
		// descriptor.setValue(new byte[]{0x03});
		// //
		// descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
		// mBluetoothGatt.writeDescriptor(descriptor);
		// Log.d(TAG,
		// "Send DESC_CLIENT_CHARACTERISTIC_CONFIGURATION ("+descriptor.getValue().toString()+")");
		// }
	}

	private BluetoothGattServerCallback mGattServerCallback = new BluetoothGattServerCallback() {

		private String getStateString(int state) {
			switch (state) {
			case BluetoothProfile.STATE_CONNECTED:
				return "STATE_CONNECTED";
			case BluetoothProfile.STATE_CONNECTING:
				return "STATE_CONNECTING";
			case BluetoothProfile.STATE_DISCONNECTED:
				return "STATE_DISCONNECTED";
			case BluetoothProfile.STATE_DISCONNECTING:
				return "STATE_DISCONNECTING";
			}
			return "UNKOWN";
		}
		
		@Override
		public void onConnectionStateChange(BluetoothDevice device, int status, int newState) {

			Log.d(TAG, "GattServerCallback: onConnectionStateChange() - status=" + getStateString(status) + ", newState=" + getStateString(newState));
			super.onConnectionStateChange(device, status, newState);
		}

		@Override
		public void onServiceAdded(int status, BluetoothGattService service) {
			Log.d(TAG, "GattServerCallback: onServiceAdded() - status=" + status + ", service=" + service);
			super.onServiceAdded(status, service);
		}

		@Override
		public void onCharacteristicReadRequest(BluetoothDevice device, int requestId, int offset, BluetoothGattCharacteristic characteristic) {
			Log.d(TAG, "GattServerCallback: onCharacteristicReadRequest() - requestId=" + requestId + ", offset=" + offset + ", characteristic="
					+ characteristic.toString());

			mBluetoothGattServer.sendResponse(device, requestId, 0, offset, new byte[] { 0x00 });

			super.onCharacteristicReadRequest(device, requestId, offset, characteristic);
		}

		@Override
		public void onCharacteristicWriteRequest(BluetoothDevice device, int requestId, BluetoothGattCharacteristic characteristic, boolean preparedWrite,
				boolean responseNeeded, int offset, byte[] value) {
//			Log.d(TAG, "GattServerCallback: onCharacteristicWriteRequest() - requestId=" + requestId + ", offset=" + offset + ", preparedWrite="
//					+ preparedWrite + ", responseNeeded=" + responseNeeded + ", value=" + new String(value) + ", characteristic=" + characteristic.toString());

			notifyListeners(value);
			
			if(responseNeeded)
				mBluetoothGattServer.sendResponse(device, requestId, 0, offset, new byte[] { 0x00 });

			super.onCharacteristicWriteRequest(device, requestId, characteristic, preparedWrite, responseNeeded, offset, value);
		}

		@Override
		public void onDescriptorReadRequest(BluetoothDevice device, int requestId, int offset, BluetoothGattDescriptor descriptor) {
			Log.d(TAG,
					"GattServerCallback: onDescriptorReadRequest() - requestId=" + requestId + ", offset=" + offset + ", descriptor=" + descriptor.toString());

			mBluetoothGattServer.sendResponse(device, requestId, 0, offset, new byte[] { 0x00 });

			super.onDescriptorReadRequest(device, requestId, offset, descriptor);
		}

		@Override
		public void onDescriptorWriteRequest(BluetoothDevice device, int requestId, BluetoothGattDescriptor descriptor, boolean preparedWrite,
				boolean responseNeeded, int offset, byte[] value) {
			Log.d(TAG, "GattServerCallback: onDescriptorWriteRequest() - requestId=" + requestId + ", offset=" + offset + ", preparedWrite=" + preparedWrite
					+ ", responseNeeded=" + responseNeeded + ", value=" + new String(value) + ", descriptor=" + descriptor.toString());

			if(responseNeeded)
				mBluetoothGattServer.sendResponse(device, requestId, 0, offset, new byte[] { 0x00 });

			super.onDescriptorWriteRequest(device, requestId, descriptor, preparedWrite, responseNeeded, offset, value);
		}

		@Override
		public void onExecuteWrite(BluetoothDevice device, int requestId, boolean execute) {
			Log.d(TAG, "GattServerCallback: onExecuteWrite() - requestId=" + requestId + ", execute=" + execute);

			super.onExecuteWrite(device, requestId, execute);
		}

	};

	public BluetoothGattServer getServer() {
		return mBluetoothGattServer;
	}
	
	// Fire up a local Gatt server.
	public void startServer() {
		boolean r;

		// get a reference to a local Gatt server
		mBluetoothGattServer = mBluetoothManager.openGattServer(getApplicationContext(), mGattServerCallback);

		// create rcs write service
		BluetoothGattService rawWriteServ = new BluetoothGattService(UUID_RCS_RAW_DATA_WRITE, BluetoothGattService.SERVICE_TYPE_PRIMARY);
		
		// attach rcs write charactersitic
		BluetoothGattCharacteristic rawWriteChar = new BluetoothGattCharacteristic(UUID_RCS_RAW_DATA_WRITE, BluetoothGattCharacteristic.PROPERTY_WRITE, BluetoothGattCharacteristic.PERMISSION_WRITE);
		rawWriteServ.addCharacteristic(rawWriteChar);
		
		// add rcs write service
		r = mBluetoothGattServer.addService(rawWriteServ);
		Log.d(TAG, "addService(): UUID_RCS_RAW_DATA_WRITE - " + r);

	}

	/**
	 * Retrieves a list of supported GATT services on the connected device. This
	 * should be invoked only after {@code BluetoothGatt#discoverServices()}
	 * completes successfully.
	 * 
	 * @return A {@code List} of supported services.
	 */
	public List<BluetoothGattService> getSupportedGattServices() {
		if (mBluetoothGatt == null) return null;

		return mBluetoothGatt.getServices();
	}

}
