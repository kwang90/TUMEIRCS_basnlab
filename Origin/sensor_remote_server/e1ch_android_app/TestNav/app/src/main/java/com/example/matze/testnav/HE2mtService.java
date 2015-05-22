package com.example.matze.testnav;

import android.app.Notification;
import android.app.PendingIntent;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.support.v4.app.NotificationCompat;
import android.util.Log;

import hemt.*;

import java.io.File;
import java.nio.ByteBuffer;
import java.util.Date;

public class HE2mtService extends Service {

    private static final String TAG = HE2mtService.class.getSimpleName();

    private static final String ACTION_STRING_HE2MT_SERVICE = "ToHemtService";
    private static final String ACTION_STRING_MOTION_CONTROL = "ToMotionControl";
    private static final String ACTION_BROADCAST_DATABASE = "ToDataBaseService";
    private static final String ACC_DATA_AVAILABLE = "ACC_DATA_AVAILABLE";
    private static final String BLUETOOTH_STATUS = "BluetoothStatus";
    private static final String MOVEMENT = "Movement";
    private static final String INFOPACKET = "InfoPacket";
    private static final String DATAPACKET = "DataPacket";

    private static final String BLE_DEVICE = "BleDevice";
    private static final String DEVELPMENT_RECORDING = "DevelopmentRecording";
    private static final String DEVELPMENT_MOVEMENT_RECORDING = "DevelopmentMovementRecording";

    private BleService m_oBluetoothLeService = null;
    private ServiceConnection m_oServiceConnection = null;
    private BroadcastReceiver m_oGattUpdateReceiver = null;

    private String m_sDeviceName = null;
    private String m_sDeviceAddress = null;

    private ProcessSensorData m_oProcessSensorData = new ProcessSensorData();

    private int m_iSelectedMovement = 0;

    //only for testing
    private int m_iTestCounterMovement = 0;

    private ByteBuffer m_oByteBuffer;
    private int m_iDataCounter = 0;


    public HE2mtService() {
    }

    /**
     * @brief This function shows the notification that the he2mt app is running in background even if the activity is closed
     */
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        startForeground(12345, new Notification());

        return START_STICKY;
    }

    @Override
    public void onCreate()
    {
        super.onCreate();

        //allocate memory for 250 Acceleration values
        m_oByteBuffer = ByteBuffer.allocate(250 * 2 * 3);

        if (ServiceBroadcastReceiverHE2mt != null)
        {
            //Create an intent filter to listen to the broadcast sent with the action "ACTION_STRING_SERVICE"
            IntentFilter intentFilter = new IntentFilter(ACTION_STRING_HE2MT_SERVICE);
            //Map the intent filter to the receiver
            registerReceiver(ServiceBroadcastReceiverHE2mt, intentFilter);
        }


        // change download folder to he2mt folder
		File root = android.os.Environment.getExternalStorageDirectory();
		File dir = new File (root.getAbsolutePath() + "/HE2mt");
		dir.mkdirs();
		File file = new File(dir, "model_v_0_1.txt");

        m_oProcessSensorData.init(file.getAbsolutePath().toString());
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
        stopForeground(true);
        this.stopBleService();
        unregisterReceiver(ServiceBroadcastReceiverHE2mt);
    }

    private BroadcastReceiver ServiceBroadcastReceiverHE2mt = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {

            if(intent.hasExtra(BLE_DEVICE))
            {
                Bundle bundle=intent.getExtras();
                BleDevice device = (BleDevice) bundle.getSerializable(BLE_DEVICE);

                //connect to the bluetooth device if there is no connection running in the moment
                if(device.getConnectionState() == BleDevice.ConnectionState.DISCONNECTED) {
                    m_sDeviceName = device.getDeviceName();
                    m_sDeviceAddress = device.getDeviceAddress();
                    Log.d(TAG, "connect BLE");
                    startBleService();
                }
            }

            //processing of the received data
            if(intent.hasExtra(ACC_DATA_AVAILABLE))
            {
                Bundle bundle=intent.getExtras();
                SensorData data = (SensorData) bundle.getSerializable(ACC_DATA_AVAILABLE);

                m_oByteBuffer.putShort(data.getAxisValueX());
                m_oByteBuffer.putShort(data.getAxisValueY());
                m_oByteBuffer.putShort(data.getAxisValueZ());

                if(m_iDataCounter >= 249)
                {
                    Date date = new Date();

                    InfoPacket infoPacket = new InfoPacket();
                    infoPacket.setSensorId(m_sDeviceAddress);
                    infoPacket.setSensorName(m_sDeviceName);
                    infoPacket.setSensorVersion(1);
                    infoPacket.setParameterId(1);
                    infoPacket.setSamplingRate(150);
                    infoPacket.setTime(date.getTime());
                    infoPacket.setQuality(1);
                    infoPacket.setEfficiency(1);

                    broadcastInfoPacketToDataBaseService(INFOPACKET,infoPacket);

                    DataPacket dataPacket = new DataPacket();
                    dataPacket.setSensorName(m_sDeviceName);
                    dataPacket.setParameterId(1);
                    dataPacket.setSensorVersion(1);
                    dataPacket.setByteBuffer(m_oByteBuffer.array());

                    broadcastDataPacketToDataBaseService(DATAPACKET,dataPacket);

                    m_iDataCounter = 0;
                    m_oByteBuffer.clear();
                }
                else
                {
                    m_iDataCounter++;
                }

                boolean newCalculationCompleted = m_oProcessSensorData.appendValues(data.getAxisValueX(), data.getAxisValueY(), data.getAxisValueZ());

                if(newCalculationCompleted)
                {
                    //Log.d(TAG, "Calculation completed");
                    //Log.d(TAG, Double.toString(m_oProcessSensorData.getMeanValueOfX()));

                    switch (m_iTestCounterMovement)
                    {
                        case 0: data.setDetectedMovement(SensorData.Movement.STAND); m_iTestCounterMovement++; break;
                        case 1: data.setDetectedMovement(SensorData.Movement.WALK); m_iTestCounterMovement++; break;
                        case 2: data.setDetectedMovement(SensorData.Movement.RUN); m_iTestCounterMovement++; break;
                        case 3: data.setDetectedMovement(SensorData.Movement.FALL); m_iTestCounterMovement = 0; break;
                        default: break;
                    }

                    //Hier werden die erkannten Bewegungen dann versendet

                    broadcastMovementToMotionControl(MOVEMENT, data);
                }
            }

            if(intent.hasExtra(DEVELPMENT_MOVEMENT_RECORDING))
            {
                m_iSelectedMovement = intent.getIntExtra(DEVELPMENT_MOVEMENT_RECORDING, 0);
            }

            if(intent.hasExtra(DEVELPMENT_RECORDING))
            {
                //Log.d(TAG, "Recording");
                boolean recording = intent.getBooleanExtra(DEVELPMENT_RECORDING, false);
                if(recording) {
                    m_oProcessSensorData.startRecording(m_iSelectedMovement);
                }
                else {
                    m_oProcessSensorData.stopRecording();
                }
            }

            if(intent.hasExtra(BLUETOOTH_STATUS))
            {
                Bundle bundle=intent.getExtras();
                BleDevice device = (BleDevice) bundle.getSerializable(BLUETOOTH_STATUS);

                broadcastBluetoothStatusToMotionControl(BLUETOOTH_STATUS,device);

            }
        }
    };

    private void broadcastBluetoothStatusToMotionControl(final String action, final BleDevice device)
    {
        final Intent intent = new Intent(action);
        intent.setAction(ACTION_STRING_MOTION_CONTROL);
        intent.putExtra(BLUETOOTH_STATUS, device);
        sendBroadcast(intent);
    }

    private void broadcastMovementToMotionControl(final String action, final SensorData device)
    {
        final Intent intent = new Intent(action);
        intent.setAction(ACTION_STRING_MOTION_CONTROL);
        intent.putExtra(MOVEMENT, device);
        sendBroadcast(intent);
    }

    private void broadcastInfoPacketToDataBaseService(final String action, final InfoPacket infoPacket)
    {
        final Intent intent = new Intent(action);
        intent.setAction(ACTION_BROADCAST_DATABASE);
        intent.putExtra(INFOPACKET, infoPacket);
        sendBroadcast(intent);
    }

    private void broadcastDataPacketToDataBaseService(final String action, final DataPacket dataPacket)
    {
        final Intent intent = new Intent(action);
        intent.setAction(ACTION_BROADCAST_DATABASE);
        intent.putExtra(DATAPACKET, dataPacket);
        sendBroadcast(intent);
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        //throw new UnsupportedOperationException("Not yet implemented");
        return null;
    }

    /**
     * @brief This function starts the Bluetooth LE Service
     */
    public boolean startBleService()
    {
        m_oServiceConnection = new ServiceConnection()
        {
            @Override
            public void onServiceConnected(ComponentName componentName, IBinder service)
            {

                m_oBluetoothLeService = ((BleService.LocalBinder) service).getService();
                if(!m_oBluetoothLeService.initialize())
                {
                    Log.e("BLE", "Unable to initialize Bluetooth");
                }

                m_oBluetoothLeService.connect(m_sDeviceAddress, m_sDeviceName);
            }

            @Override
            public void onServiceDisconnected(ComponentName componentName)
            {
                m_oBluetoothLeService = null;
            }
        };

        m_oGattUpdateReceiver = new BroadcastReceiver()
        {
            @Override
            public void onReceive(Context context, Intent intent)
            {
                final String action = intent.getAction();
                if(BleService.ACTION_GATT_DISCONNECTED.equals(action))
                {
                    startBleService();
                }
            }
        };

        Intent gattServiceIntent = new Intent(this, BleService.class);
        bindService(gattServiceIntent, m_oServiceConnection, BIND_AUTO_CREATE);

        registerReceiver(m_oGattUpdateReceiver, makeGattUpdateIntentFilter());

        return true;
    }

    public void stopBleService()
    {
        m_oBluetoothLeService.close();
        unbindService(m_oServiceConnection);
        unregisterReceiver(m_oGattUpdateReceiver);
    }

    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BleService.ACTION_GATT_CONNECTED);
        intentFilter.addAction(BleService.ACTION_GATT_DISCONNECTED);
        return intentFilter;
    }
}
