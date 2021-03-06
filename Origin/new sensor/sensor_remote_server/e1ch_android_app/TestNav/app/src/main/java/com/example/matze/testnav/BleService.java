package com.example.matze.testnav;

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattServer;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.List;
import java.util.UUID;

/**
 * Created by Mathias Gopp on 24.02.2015.
 *
 * This class is Service which handels the Bluetooth connection.
 */

public class BleService extends Service {

    private static final String TAG = BleService.class.getSimpleName();

    //declaration of various strings
    private static final String BLE_DEVICE = "BleDevice";
    private static final String ACC_DATA_AVAILABLE = "ACC_DATA_AVAILABLE";
    private static final String BLUETOOTH_STATUS = "BluetoothStatus";

    private static final String ACTION_STRING_SERVICE = "ToHemtService";
    private static final String ACTION_STRING_MOTIONCONTROL = "ToMotionControl";
    private static final String ACTION_STRING_DEVELOPMENT = "ToDevelopment";
    public final static String ACTION_GATT_CONNECTED = "ACTION_GATT_CONNECTED";
    public final static String ACTION_GATT_DISCONNECTED = "ACTION_GATT_DISCONNECTED";

    private BluetoothManager m_oBluetoothManager;
    private BluetoothAdapter m_oBluetoothAdapter;
    private String m_sDeviceAddress;
    private String m_sDeviceName;
    private BluetoothGatt m_oBluetoothGatt;
    private BleDevice.ConnectionState m_oConnectionState = BleDevice.ConnectionState.DISCONNECTED;
    private Handler m_oSensorHandler;
    private Handler m_oNotificationHandler;
    private final IBinder m_oBinder = new LocalBinder();

    public BleService() {
    }

    /**
     * @brief This function handles the different states of the Bluetooth connection (Disconnected, Connecting, Conncted)
     */
    private final BluetoothGattCallback m_oGattCallback = new BluetoothGattCallback()
    {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState)
        {
            String intentAction;
            BleDevice device = new BleDevice();

            device.setDeviceName(m_sDeviceName);
            device.setDeviceAddress(m_sDeviceAddress);

            if (newState == BluetoothProfile.STATE_CONNECTED)
            {
                intentAction = BLE_DEVICE;
                m_oConnectionState = BleDevice.ConnectionState.CONNECTED;
                broadcastUpdate(intentAction);
                Log.d(TAG, "Attempting to start service discovery:" + m_oBluetoothGatt.discoverServices());

                //*********************************************************************************************************************
                //RCS
                //*********************************************************************************************************************

                //activate the characteristic notification to receive data from the sensorboard
                //due to timing problems the characteristic notification and the start sensor data are transmitted with delays

                if(Global.getSensorId() == Global.RCS_SENSOR_ID) {
                    m_oNotificationHandler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            setCharacteristicNotification(true);
                        }
                    }, 500);

                    m_oSensorHandler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            startSensor();
                        }
                    }, 750);
                }

                //*********************************************************************************************************************
                //IIS
                //*********************************************************************************************************************

                if(Global.getSensorId() == Global.IIS_SENSOR_ID) {
                    m_oNotificationHandler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            setCharacteristicNotification(true);
                        }
                    }, 500);
                }

                //*********************************************************************************************************************

                device.setConnectionState(m_oConnectionState);

            }
            else if(newState == BluetoothProfile.STATE_DISCONNECTED)
            {
                intentAction = BLE_DEVICE;
                m_oConnectionState = BleDevice.ConnectionState.DISCONNECTED;
                Log.d(TAG, "Disconnected from GATT server.");
                device.setConnectionState(m_oConnectionState);
                broadcastUpdate(intentAction);

            }
            else if(newState == BluetoothProfile.STATE_CONNECTING)
            {
                m_oConnectionState = BleDevice.ConnectionState.CONNECTING;
                device.setConnectionState(m_oConnectionState);
            }

            broadcastBluetoothStatusToHE2mtService(BLUETOOTH_STATUS,device);
        }

        /**
         * @brief This function handles the incoming data from the bluetooth sensor
         */
        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, final BluetoothGattCharacteristic characteristic) {

            byte[] rawData = characteristic.getValue();

            //*********************************************************************************************************************
            //RCS
            //*********************************************************************************************************************

            if (Global.getSensorId() == Global.RCS_SENSOR_ID) {

                SensorData data = new SensorData();

                long index = rawData[0];
                data.setIndex(index);

                long numberOfMeasurements = rawData[1];
                data.setNumberOfMeasurements(1);

                for (int i = 0; i < numberOfMeasurements; i++) {

                    ByteBuffer bufx = ByteBuffer.allocate(2);
                    bufx.order(ByteOrder.LITTLE_ENDIAN);

                    ByteBuffer bufy = ByteBuffer.allocate(2);
                    bufy.order(ByteOrder.LITTLE_ENDIAN);

                    ByteBuffer bufz = ByteBuffer.allocate(2);
                    bufz.order(ByteOrder.LITTLE_ENDIAN);

                    bufx.put(rawData[i * 6 + 2]);
                    bufx.put(rawData[i * 6 + 3]);

                    data.setAxisValueX(bufx.getShort(0));


                    bufy.put(rawData[i * 6 + 4]);
                    bufy.put(rawData[i * 6 + 5]);

                    data.setAxisValueY(bufy.getShort(0));

                    bufz.put(rawData[i * 6 + 6]);
                    bufz.put(rawData[i * 6 + 7]);

                    data.setAxisValueZ(bufz.getShort(0));

                    broadcastSensorDataToHE2mtService(ACC_DATA_AVAILABLE, data);
                    broadcastSensorDataToDevelopment(ACC_DATA_AVAILABLE, data);
                }
            }

            //*********************************************************************************************************************
            //IIS
            //*********************************************************************************************************************

            // handle received data here

            //*********************************************************************************************************************
        }

    };

    /**
     * @brief This function broadcasts the status of bluetooth device to other services or activities
     */
    private void broadcastUpdate(final String action) {
        final Intent intent = new Intent(action);
        sendBroadcast(intent);
    }

    /**
     * @brief This function broadcasts the sensordata to the he2mt service
     */
    private void broadcastSensorDataToHE2mtService(final String action, final SensorData sensordata)
    {
        final Intent intent = new Intent(action);
        intent.setAction(ACTION_STRING_SERVICE);
        intent.putExtra(ACC_DATA_AVAILABLE, sensordata);
        sendBroadcast(intent);
    }

    /**
     * @brief This function broadcasts the sensordata to the developement class
     */
    private void broadcastSensorDataToDevelopment(final String action, final SensorData sensordata)
    {
        final Intent intent = new Intent(action);
        intent.setAction(ACTION_STRING_DEVELOPMENT);
        intent.putExtra(ACC_DATA_AVAILABLE, sensordata);
        sendBroadcast(intent);
    }

    /**
     * @brief This function broadcasts the bluetooth status to the he2mt service
     */
    private void broadcastBluetoothStatusToHE2mtService(final String action, final BleDevice device)
    {
        final Intent intent = new Intent(action);
        intent.setAction(ACTION_STRING_SERVICE);
        intent.putExtra(BLUETOOTH_STATUS, device);
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
        // TODO: Return the communication channel to the service.
        //throw new UnsupportedOperationException("Not yet implemented");
        return m_oBinder;
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

    /**
     * Initializes a reference to the local Bluetooth adapter.
     *
     * @return Return true if the initialization is successful.
     */
    public boolean initialize() {
        // For API level 18 and above, get a reference to BluetoothAdapter
        // through
        // BluetoothManager.
        if (m_oBluetoothManager == null)
        {
            m_oBluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
            if (m_oBluetoothManager == null)
            {
                Log.e(TAG, "Unable to initialize BluetoothManager.");
                return false;
            }
        }

        m_oBluetoothAdapter = m_oBluetoothManager.getAdapter();
        if (m_oBluetoothAdapter == null)
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
    public boolean connect(final String address, final String name)
    {

        m_oSensorHandler = new Handler();
        m_oNotificationHandler = new Handler();

        if (m_oBluetoothAdapter == null || address == null)
        {
            Log.w(TAG, "BluetoothAdapter not initialized or unspecified address.");
            return false;
        }

        // Previously connected device. Try to reconnect.
        if (m_sDeviceAddress != null && address.equals(m_sDeviceAddress) && m_oBluetoothGatt != null)
        {
            //Log.d(TAG, "Trying to use an existing mBluetoothGatt for connection.");

            if (m_oBluetoothGatt.connect())
            {
                m_oConnectionState = BleDevice.ConnectionState.CONNECTING;
                return true;
            }
            else
            {
                return false;
            }
        }

        final BluetoothDevice device = m_oBluetoothAdapter.getRemoteDevice(address);
        if (device == null)
        {
            Log.w(TAG, "Device not found.  Unable to connect.");
            return false;
        }
        // We want to directly connect to the device, so we are setting the
        // autoConnect
        // parameter to false.

        m_oBluetoothGatt = device.connectGatt(this, false, m_oGattCallback);
        m_sDeviceAddress = address;
        m_sDeviceName = name;
        m_oConnectionState = BleDevice.ConnectionState.CONNECTING;

        BleDevice status = new BleDevice();
        status.setConnectionState(m_oConnectionState);
        status.setDeviceAddress(address);
        status.setDeviceName(name);

        broadcastBluetoothStatusToHE2mtService(BLUETOOTH_STATUS, status);
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
        if (m_oBluetoothAdapter == null || m_oBluetoothGatt == null)
        {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        }
        m_oBluetoothGatt.disconnect();
    }

    /**
     * After using a given BLE device, the app must call this method to ensure
     * resources are released properly.
     */
    public void close()
    {
        if (m_oBluetoothGatt == null)
        {
            return;
        }
        m_oBluetoothGatt.close();
        m_oBluetoothGatt = null;
    }

    /**
     * @brief This function sends a message to the sensor board that starts the acceleration sensor on the board
     */
    public boolean startSensor()
    {
        if (m_oBluetoothAdapter == null || m_oBluetoothGatt == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return false;
        }

        if (Global.getSensorId() == Global.RCS_SENSOR_ID) {
            BluetoothGattCharacteristic characteristic = m_oBluetoothGatt.getService(UUID.fromString(GattAttributes.CH_RCS_ACC_SERVICE)).getCharacteristic(UUID.fromString(GattAttributes.CH_RCS_ACC_RAW_DATA_WRITE));
            characteristic.setValue(new byte[]{0x10, 0x01});
            return m_oBluetoothGatt.writeCharacteristic(characteristic);
        }

        return false;
    }

    /**
     * @brief This function stops the sensor on the sensor board
     */
    public boolean stopSensor()
    {
        if (m_oBluetoothAdapter == null || m_oBluetoothGatt == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return false;
        }

        BluetoothGattCharacteristic characteristic = m_oBluetoothGatt.getService(UUID.fromString(GattAttributes.CH_RCS_ACC_SERVICE)).getCharacteristic(UUID.fromString(GattAttributes.CH_RCS_ACC_RAW_DATA_WRITE));
        characteristic.setValue(new byte[]{0x00, 0x00});
        return m_oBluetoothGatt.writeCharacteristic(characteristic);
    }

    /**
     * @brief This function sets the characteristic notification of the bluetooth module on the sensor board
     */
    public boolean setCharacteristicNotification(boolean enable)
    {
        //*********************************************************************************************************************
        //RCS
        //*********************************************************************************************************************
        if(Global.getSensorId() == Global.RCS_SENSOR_ID) {
            BluetoothGattCharacteristic characteristic = m_oBluetoothGatt.getService(UUID.fromString(GattAttributes.CH_RCS_ACC_SERVICE)).getCharacteristic(UUID.fromString(GattAttributes.CH_RCS_ACC_RAW_DATA_READ));
            BluetoothGattDescriptor descriptor = characteristic.getDescriptor(UUID.fromString(GattAttributes.CHARACTERISTIC_UPDATE_NOTIFICATION));
            descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
            boolean success = m_oBluetoothGatt.writeDescriptor(descriptor);
            if (success) {
                return m_oBluetoothGatt.setCharacteristicNotification(characteristic, enable);
            }
        }

        //*********************************************************************************************************************
        //IIS
        //*********************************************************************************************************************

        if(Global.getSensorId() == Global.IIS_SENSOR_ID) {
            BluetoothGattCharacteristic characteristic = m_oBluetoothGatt.getService(UUID.fromString(GattAttributes.CH_IIS_RAW_SERVICE)).getCharacteristic(UUID.fromString(GattAttributes.CH_IIS_RAW_DATA_READ));
            BluetoothGattDescriptor descriptor = characteristic.getDescriptor(UUID.fromString(GattAttributes.CHARACTERISTIC_UPDATE_NOTIFICATION));
            descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
            boolean success = m_oBluetoothGatt.writeDescriptor(descriptor);
            if (success) {
                return m_oBluetoothGatt.setCharacteristicNotification(characteristic, enable);
            }
        }

        //*********************************************************************************************************************

        return false;
    }
}
