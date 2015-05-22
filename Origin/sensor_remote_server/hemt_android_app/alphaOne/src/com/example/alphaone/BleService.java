package com.example.alphaone;

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


public class BleService extends Service{

	   private final static String TAG = BleService.class.getSimpleName();

	    private BluetoothManager mBluetoothManager;
	    private BluetoothAdapter mBluetoothAdapter;
	    private String mBluetoothDeviceAddress;
	    private BluetoothGatt mBluetoothGatt;
	    private BluetoothGattServer mBluetoothGattServer;
	    private int mConnectionState = STATE_DISCONNECTED;

	    private static final int STATE_DISCONNECTED = 0;
	    private static final int STATE_CONNECTING = 1;
	    private static final int STATE_CONNECTED = 2;

	    public final static String ACTION_GATT_CONNECTED = "com.example.testapp.ACTION_GATT_CONNECTED";
	    public final static String ACTION_GATT_DISCONNECTED = "com.example.testapp.ACTION_GATT_DISCONNECTED";
	    public final static String ACTION_GATT_SERVICES_DISCOVERED = "com.example.testapp.ACTION_GATT_SERVICES_DISCOVERED";
	    public final static String ACTION_DATA_AVAILABLE = "com.example.testapp.ACTION_DATA_AVAILABLE";
	    public final static String EXTRA_DATA = "com.example.testapp.EXTRA_DATA";

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

	    // Various callback methods defined by the BLE API.
	    private final BluetoothGattCallback mGattCallback = new BluetoothGattCallback()
	    {
	        @Override
	        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState)
	        {
	            String intentAction;
	            if (newState == BluetoothProfile.STATE_CONNECTED)
	            {
	                intentAction = ACTION_GATT_CONNECTED;
	                mConnectionState = STATE_CONNECTED;
	                broadcastUpdate(intentAction);
	                Log.i(TAG, "Connected to GATT server.");
	                Log.i(TAG, "Attempting to start service discovery:" + mBluetoothGatt.discoverServices());
	            }
	            else if(newState == BluetoothProfile.STATE_DISCONNECTED)
	            {
	                intentAction = ACTION_GATT_DISCONNECTED;
	                mConnectionState = STATE_DISCONNECTED;
	                Log.i(TAG, "Disconnected from GATT server.");
	                broadcastUpdate(intentAction);
	            }
	        }

	        @Override
	        // New services discovered
	        public void onServicesDiscovered(BluetoothGatt gatt, int status)
	        {
	            if (status == BluetoothGatt.GATT_SUCCESS)
	            {
	                broadcastUpdate(ACTION_GATT_SERVICES_DISCOVERED);
	            }
	            else
	            {
	                Log.w(TAG, "onServicesDiscovered received: " + status);
	            }
	        }

	        @Override
	        // Result of a characteristic read operation
	        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status)
	        {
	            if (status == BluetoothGatt.GATT_SUCCESS)
	            {
	                broadcastUpdate(ACTION_DATA_AVAILABLE, characteristic);
	            }
	        }
	    };
	    
	    private void broadcastUpdate(final String action) {
	        final Intent intent = new Intent(action);
	        sendBroadcast(intent);
	    }

	    private void broadcastUpdate(final String action, final BluetoothGattCharacteristic characteristic)
	    {
	        final Intent intent = new Intent(action);

	        // This is special handling for the Heart Rate Measurement profile. Data
	        // parsing is carried out as per profile specifications.
	        if (UUID_RCS_RAW_DATA_READ.equals(characteristic.getUuid()))
	        {
				final byte[] data = characteristic.getValue();
				int prop = characteristic.getProperties();
				if (data != null && data.length > 0)
				{
					final StringBuilder stringBuilder = new StringBuilder(data.length);
					for (byte byteChar : data)
						stringBuilder.append(String.format("%02X ", byteChar));
					intent.putExtra(EXTRA_DATA, "\"" + new String(data) + "\"<br><i>hex[" + data.length + "]:</i> " + stringBuilder.toString());
				}
				else if (data == null)
				{
					intent.putExtra(EXTRA_DATA, "characteristic.getValue() returns (null)<br/>" + String.format("Properties: 0x%02X", prop));
				}
				else if (data.length == 0)
				{
					intent.putExtra(EXTRA_DATA, "data length is 0<br/>" + String.format("Properties: 0x%02X", prop));
				}
			}
	        else
	        {
	            // For all other profiles, writes the data formatted in HEX.
	            final byte[] data = characteristic.getValue();
	            if (data != null && data.length > 0)
	            {
	                final StringBuilder stringBuilder = new StringBuilder(data.length);
	                for(byte byteChar : data)
	                {
	                    stringBuilder.append(String.format("%02X ", byteChar));
	                }
	                intent.putExtra(EXTRA_DATA, new String(data) + "\n" + stringBuilder.toString());
	            }
	        }
	        sendBroadcast(intent);
	    }

	    public class LocalBinder extends Binder
	    {
			BleService getService()
			{
				return BleService.this;
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
			if (mBluetoothManager == null)
			{
				mBluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
				if (mBluetoothManager == null)
				{
					Log.e(TAG, "Unable to initialize BluetoothManager.");
					return false;
				}
			}

			mBluetoothAdapter = mBluetoothManager.getAdapter();
			if (mBluetoothAdapter == null)
			{
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
		public boolean connect(final String address)
		{
			//Log.d("BLE", "in connect function");
			if (mBluetoothAdapter == null || address == null)
			{
				Log.w(TAG, "BluetoothAdapter not initialized or unspecified address.");
				return false;
			}

			// Previously connected device. Try to reconnect.
			if (mBluetoothDeviceAddress != null && address.equals(mBluetoothDeviceAddress) && mBluetoothGatt != null)
			{
				//Log.d(TAG, "Trying to use an existing mBluetoothGatt for connection.");
				if (mBluetoothGatt.connect())
				{
					mConnectionState = STATE_CONNECTING;
					return true;
				}
				else
				{
					return false;
				}
			}

			final BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
			if (device == null)
			{
				Log.w(TAG, "Device not found.  Unable to connect.");
				return false;
			}
			// We want to directly connect to the device, so we are setting the
			// autoConnect
			// parameter to false.
			Log.d("BLE", "connect to gatt");
			
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
		public void disconnect()
		{
			if (mBluetoothAdapter == null || mBluetoothGatt == null)
			{
				Log.w(TAG, "BluetoothAdapter not initialized");
				return;
			}
			mBluetoothGatt.disconnect();
		}

		/**
		 * After using a given BLE device, the app must call this method to ensure
		 * resources are released properly.
		 */
		public void close()
		{
			if (mBluetoothGatt == null)
			{
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
		public void setCharacteristicNotification(BluetoothGattCharacteristic characteristic, boolean enabled)
		{
			if (mBluetoothAdapter == null || mBluetoothGatt == null)
			{
				Log.w(TAG, "BluetoothAdapter not initialized");
				return;
			}
			mBluetoothGatt.setCharacteristicNotification(characteristic, enabled);

		}

		private BluetoothGattServerCallback mGattServerCallback = new BluetoothGattServerCallback()
		{

			private String getStateString(int state)
			{
				switch (state)
				{
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

				//Log.d(TAG, "GattServerCallback: onConnectionStateChange() - status=" + getStateString(status) + ", newState=" + getStateString(newState));
				super.onConnectionStateChange(device, status, newState);
			}

			@Override
			public void onServiceAdded(int status, BluetoothGattService service) {
				//Log.d(TAG, "GattServerCallback: onServiceAdded() - status=" + status + ", service=" + service);
				super.onServiceAdded(status, service);
			}

			@Override
			public void onCharacteristicReadRequest(BluetoothDevice device, int requestId, int offset, BluetoothGattCharacteristic characteristic) {
				//Log.d(TAG, "GattServerCallback: onCharacteristicReadRequest() - requestId=" + requestId + ", offset=" + offset + ", characteristic="
				//		+ characteristic.toString());

				mBluetoothGattServer.sendResponse(device, requestId, 0, offset, new byte[] { 0x00 });

				super.onCharacteristicReadRequest(device, requestId, offset, characteristic);
			}
			
			//hier werden die daten vom ble device empfangen
			@Override
			public void onCharacteristicWriteRequest(BluetoothDevice device, int requestId, BluetoothGattCharacteristic characteristic, boolean preparedWrite,
					boolean responseNeeded, int offset, byte[] value) {
//				Log.d(TAG, "GattServerCallback: onCharacteristicWriteRequest() - requestId=" + requestId + ", offset=" + offset + ", preparedWrite="
//						+ preparedWrite + ", responseNeeded=" + responseNeeded + ", value=" + new String(value) + ", characteristic=" + characteristic.toString());

				notifyListeners(value);
				
				if(responseNeeded)
					mBluetoothGattServer.sendResponse(device, requestId, 0, offset, new byte[] { 0x00 });

				super.onCharacteristicWriteRequest(device, requestId, characteristic, preparedWrite, responseNeeded, offset, value);
			}

			@Override
			public void onDescriptorReadRequest(BluetoothDevice device, int requestId, int offset, BluetoothGattDescriptor descriptor) {
				//Log.d(TAG,
				//		"GattServerCallback: onDescriptorReadRequest() - requestId=" + requestId + ", offset=" + offset + ", descriptor=" + descriptor.toString());

				mBluetoothGattServer.sendResponse(device, requestId, 0, offset, new byte[] { 0x00 });

				super.onDescriptorReadRequest(device, requestId, offset, descriptor);
			}

			@Override
			public void onDescriptorWriteRequest(BluetoothDevice device, int requestId, BluetoothGattDescriptor descriptor, boolean preparedWrite, boolean responseNeeded, int offset, byte[] value)
			{
				//Log.d(TAG, "GattServerCallback: onDescriptorWriteRequest() - requestId=" + requestId + ", offset=" + offset + ", preparedWrite=" + preparedWrite
				//		+ ", responseNeeded=" + responseNeeded + ", value=" + new String(value) + ", descriptor=" + descriptor.toString());

				if(responseNeeded)
					mBluetoothGattServer.sendResponse(device, requestId, 0, offset, new byte[] { 0x00 });

				super.onDescriptorWriteRequest(device, requestId, descriptor, preparedWrite, responseNeeded, offset, value);
			}

			@Override
			public void onExecuteWrite(BluetoothDevice device, int requestId, boolean execute) {
				//Log.d(TAG, "GattServerCallback: onExecuteWrite() - requestId=" + requestId + ", execute=" + execute);

				super.onExecuteWrite(device, requestId, execute);
			}

		};

		public BluetoothGattServer getServer()
		{
			return mBluetoothGattServer;
		}
		
		// Fire up a local Gatt server.
		public void startServer()
		{
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
			//Log.d(TAG, "addService(): UUID_RCS_RAW_DATA_WRITE - " + r);

		}

		/**
		 * Retrieves a list of supported GATT services on the connected device. This
		 * should be invoked only after {@code BluetoothGatt#discoverServices()}
		 * completes successfully.
		 * 
		 * @return A {@code List} of supported services.
		 */
		public List<BluetoothGattService> getSupportedGattServices()
		{
			if (mBluetoothGatt == null) return null;

			return mBluetoothGatt.getServices();
		}
}
