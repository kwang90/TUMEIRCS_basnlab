package com.example.alphaone;

import java.io.File;
import java.nio.ByteBuffer;
import java.util.Date;

import com.example.alphaone.BleService;
import com.example.alphaone.BleService.OnDataAvailableListener;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;

public class hemtService extends Service
{
	//Strings to register to create intent filter for registering the recivers
    private static final String ACTION_STRING_SERVICE = "ToHemtService";
    private static final String ACTION_STRING_ACTIVITY = "ToHemtActivity";
    private static final String ACTION_STRING_DATABASE_SERVICE = "ToDatabaseService";
    private static final String GET_DEVICE_NAME = "GetDeviceName";
    private static final String GET_DEVICE_ADDRESS = "GetDeviceAddress";
    private static final String SET_DEVICE_NAME = "SetDeviceName";
    private static final String SET_DEVICE_ADDRESS = "SetDeviceAddress";
    private static final String GET = "get";
    private static final String SET = "set";
    private static final String SET_ACC_X = "SetAccX";
    private static final String SET_ACC_Y = "SetAccY";
    private static final String SET_ACC_Z = "SetAccZ";
    private static final String SET_ACC_MEAN_X = "SetAccMeanX";
    private static final String SET_ACC_MEAN_Y = "SetAccMeanY";
    private static final String SET_ACC_MEAN_Z = "SetAccMeanZ";
    private static final String SET_ACC_COR_XY = "SetAccCorXY";
    private static final String SET_ACC_COR_XZ = "SetAccCorXZ";
    private static final String SET_ACC_COR_YZ = "SetAccCorYZ";
    private static final String SET_BLE_CONNECTED = "BleConnected";
    private static final String SET_BLE_DISCONNECTED = "BleDisConnected";
    private static final String SET_START_RECORDING = "SetStartRecording";
    private static final String SET_STOP_RECORDING = "SetStopRecording";
    private static final String SET_ACC_PREDICTION = "SetPrediction";
    private static final String SET_INFO_PACKET = "SetInfoPacket";
    private static final String SET_DATA_PACKET = "SetDataPacket";
    
    private BleService mBluetoothLeService = null;
    private ServiceConnection mServiceConnection = null;
    private BroadcastReceiver mGattUpdateReceiver = null;
	
	private String mDeviceName = null;
	private String mDeviceAddress = null;
	
	private ProcessData m_oProcessData = new ProcessData();
	
	private ByteBuffer m_oByteBuffer;
	
	private int m_iDataCounter;
    
    @Override
    public void onCreate()
    {
        super.onCreate();
        
        Log.d("Service", "serviceReceiver");
        
        m_oByteBuffer = ByteBuffer.allocate(250 * 8 * 3);
        m_iDataCounter = 0;
        
        if (hemtServiceBroadcastReceiver != null)
        {
        	//Create an intent filter to listen to the broadcast sent with the action "ACTION_STRING_SERVICE"
            IntentFilter intentFilter = new IntentFilter(ACTION_STRING_SERVICE);
            //Map the intent filter to the receiver
            registerReceiver(hemtServiceBroadcastReceiver, intentFilter);
            Log.d("Service", "serviceReceiver");
        }
        
        broadcastToHemtActivity(GET, GET_DEVICE_NAME);
        broadcastToHemtActivity(GET, GET_DEVICE_ADDRESS);
        
		File root = android.os.Environment.getExternalStorageDirectory();
		File dir = new File (root.getAbsolutePath() + "/Download");
		dir.mkdirs();
		File file = new File(dir, "model.txt");
	    
        m_oProcessData.init(file.getAbsolutePath().toString());
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
        Log.d("Service", "onDestroy");
        //STEP3: Unregister the receiver
        this.stopBleService();
        unregisterReceiver(hemtServiceBroadcastReceiver);
    }

    private BroadcastReceiver hemtServiceBroadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
        	
        	if(intent.hasExtra(SET_DEVICE_NAME))
        	{
    			mDeviceName = intent.getStringExtra(SET_DEVICE_NAME);
    			Log.d("HemtService", mDeviceName);
        	}
        	
    		if(intent.hasExtra(SET_DEVICE_ADDRESS))
    		{
    			mDeviceAddress = intent.getStringExtra(SET_DEVICE_ADDRESS);
    			Log.d("HemtService", mDeviceAddress);
    			startBleService();
    		}
    		
    		if(intent.hasExtra(SET_START_RECORDING))
    		{
    			int type = intent.getIntExtra(SET_START_RECORDING, 10);
    			Log.d("HemtService", "Start recording");
    			m_oProcessData.startRecording(type);
    		}
    		
    		if(intent.hasExtra(SET_STOP_RECORDING))
    		{
    			int type = intent.getIntExtra(SET_STOP_RECORDING, 10);
    			Log.d("HemtService", "Stop recording");
    			m_oProcessData.stopRecording();
    		}
        }
    };

    @Override
    public IBinder onBind(Intent arg0)
    {
        return null;
    }
    
    private void broadcastToHemtActivity(String action, String value)
    {
        Intent new_intent = new Intent();
        new_intent.setAction(ACTION_STRING_ACTIVITY);
        new_intent.putExtra(action, value);
        sendBroadcast(new_intent);
    }
    
    private void broadcastToHemtActivity(String action, double value)
    {
        Intent new_intent = new Intent();
        new_intent.setAction(ACTION_STRING_ACTIVITY);
        new_intent.putExtra(action, value);
        sendBroadcast(new_intent);
    }
    
    private void broadcastToDatabaseService(String action, InfoPacket value)
    {
        Intent new_intent = new Intent();
        Bundle bundle = new Bundle();
        bundle.putSerializable(action, value);
        new_intent.setAction(ACTION_STRING_DATABASE_SERVICE);
        new_intent.putExtras(bundle); 
        sendBroadcast(new_intent);
    }
    
    private void broadcastToDatabaseService(String action, DataPacket value)
    {
        Intent new_intent = new Intent();
        Bundle bundle = new Bundle();
        bundle.putSerializable(action, value);
        new_intent.setAction(ACTION_STRING_DATABASE_SERVICE);
        new_intent.putExtras(bundle); 
        sendBroadcast(new_intent);
    }
	
    public boolean startBleService()
	{
		mServiceConnection = new ServiceConnection()
		{
			@Override
			public void onServiceConnected(ComponentName componentName, IBinder service)
			{
				
				mBluetoothLeService = ((BleService.LocalBinder) service).getService();
				if(!mBluetoothLeService.initialize())
				{
					Log.e("BLE", "Unable to initialize Bluetooth");
				}
				if (mBluetoothLeService.getServer() == null)
				{
					mBluetoothLeService.startServer();
				}
				mBluetoothLeService.connect(mDeviceAddress);
				mBluetoothLeService.registerListener(onDataAvailableListener);
			}

			@Override
			public void onServiceDisconnected(ComponentName componentName)
			{
				mBluetoothLeService.unregisterListener(onDataAvailableListener);
				mBluetoothLeService = null;
			}
		};
		
		mGattUpdateReceiver = new BroadcastReceiver()
		{
			@Override
			public void onReceive(Context context, Intent intent)
			{
				final String action = intent.getAction();
				if(BleService.ACTION_GATT_CONNECTED.equals(action))
				{
					broadcastToHemtActivity(SET, SET_BLE_CONNECTED);
				}
				else if(BleService.ACTION_GATT_DISCONNECTED.equals(action))
				{
					broadcastToHemtActivity(SET, SET_BLE_DISCONNECTED);
					startBleService();
				}
			}
		};
		
		Intent gattServiceIntent = new Intent(this, BleService.class);
		bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);
		
		registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());
		
		return true;
	}
    
    public void stopBleService()
    {
    	mBluetoothLeService.close();
    	unbindService(mServiceConnection);
    	unregisterReceiver(mGattUpdateReceiver);
    }
    
	private OnDataAvailableListener onDataAvailableListener = new OnDataAvailableListener()
	{
		@Override
		public void onDataAvailable(final byte[] data)
		{
			for (int i = 0; i < 3; i++)
			{				
				double x = data[3 * i];
				double y = data[3 * i + 1];
				double z = data[3 * i + 2];
				
				m_oByteBuffer.putDouble(x);
				m_oByteBuffer.putDouble(y);
				m_oByteBuffer.putDouble(z);
				
				m_iDataCounter++;
				
				if(m_iDataCounter == 250)
				{
					m_iDataCounter = 0;
					
					double a = 0.0;
					
					
					DataPacket dataPacket = new DataPacket();
					dataPacket.setParamterId(1);
					
					byte[] bytes = m_oByteBuffer.array();
					
					dataPacket.setByteBuffer(bytes);
					
					broadcastToDatabaseService(SET_DATA_PACKET, dataPacket);
					
					m_oByteBuffer.clear();
					
				}
				
				boolean newCalculationCompleted = m_oProcessData.appendValues(x, y, z);
				
				if(newCalculationCompleted)
				{
					broadcastToHemtActivity(SET_ACC_MEAN_X, m_oProcessData.getMeanValueOfX());
					broadcastToHemtActivity(SET_ACC_MEAN_Y, m_oProcessData.getMeanValueOfY());
					broadcastToHemtActivity(SET_ACC_MEAN_Z, m_oProcessData.getMeanValueOfZ());
					
					broadcastToHemtActivity(SET_ACC_COR_XY, m_oProcessData.getCorrelationXY());
					broadcastToHemtActivity(SET_ACC_COR_XZ, m_oProcessData.getCorrelationXZ());
					broadcastToHemtActivity(SET_ACC_COR_YZ, m_oProcessData.getCorrelationYZ());
					
					broadcastToHemtActivity(SET_ACC_PREDICTION, m_oProcessData.getPrediction());
					
					InfoPacket infoPacket = new InfoPacket();
					
					infoPacket.setSamplingRate(20);
					infoPacket.setQuality(1);
					infoPacket.setParameterId(3);
					Date date = new Date();
					infoPacket.setTime(date.getTime());
					
					broadcastToDatabaseService(SET_INFO_PACKET, infoPacket);
					
				}
				
				broadcastToHemtActivity(SET_ACC_X, x);
				broadcastToHemtActivity(SET_ACC_Y, y);
				broadcastToHemtActivity(SET_ACC_Z, z);
			}
		}
	};
	
	private static IntentFilter makeGattUpdateIntentFilter() {
		final IntentFilter intentFilter = new IntentFilter();
		intentFilter.addAction(BleService.ACTION_GATT_CONNECTED);
		intentFilter.addAction(BleService.ACTION_GATT_DISCONNECTED);
		return intentFilter;
	}

}
