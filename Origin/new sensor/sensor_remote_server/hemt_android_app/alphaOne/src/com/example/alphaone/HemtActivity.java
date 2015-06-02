package com.example.alphaone;


import java.io.File;
import java.nio.ByteBuffer;
import java.util.List;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.Application;
import android.app.ApplicationErrorReport.RunningServiceInfo;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.media.MediaScannerConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewDebug.IntToString;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.RadioButton;
import android.widget.TextView;

public class HemtActivity extends Activity
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
    private static final String BLE_CONNECT = "BleConnect";
    private static final String BLE_DISCONNECT = "BleDisconnect";
    private static final String SET_START_RECORDING = "SetStartRecording";
    private static final String SET_STOP_RECORDING = "SetStopRecording";
    private static final String SET_ACC_PREDICTION = "SetPrediction";
    private static final String SET_INFO_PACKET = "SetInfoPacket";
    private static final String SET_DATA_PACKET = "SetDataPacket";
    
    private static int STAND = 0;
    private static int WALK = 1;
    private static int RUN = 2;
    
	private String mDeviceName;
	private String mDeviceAddress;
	
	private int m_iRecordingMode = STAND;
	
	private boolean m_bRecording = false;
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_hemt);
		
		ImageView ivC = (ImageView)findViewById(R.id.imageViewBluetoothConnection);
		ivC.setImageResource(R.drawable.bluetoothred);
		
        if (hemtActivityBroadcastReceiver != null)
        {
        	//Create an intent filter to listen to the broadcast sent with the action "ACTION_STRING_ACTIVITY"
            IntentFilter intentFilter = new IntentFilter(ACTION_STRING_ACTIVITY);
            intentFilter.addAction(ACTION_STRING_DATABASE_SERVICE);
            //Map the intent filter to the receiver
            registerReceiver(hemtActivityBroadcastReceiver, intentFilter);
            Log.d("HEMT", "activityReceiver registered");
        }
        
        Bundle extras = getIntent().getExtras();
        
        if(extras != null)
        {
        	mDeviceAddress = extras.getString(SET_DEVICE_ADDRESS);
        	mDeviceName = extras.getString(SET_DEVICE_NAME);
        	startService(new Intent(this,hemtService.class));
        }
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.hemt, menu);
		return true;
	}
	
	@Override
	public void onDestroy() {
	    super.onDestroy();  // Always call the superclass
	    
	    // Stop method tracing that the activity started during onCreate()
	    Log.d("HEMT", "activity destroyed");
	  //STEP3: Unregister the receiver
		if(hemtActivityBroadcastReceiver != null)
		{
			//unregisterReceiver(hemtActivityBroadcastReceiver);
			//hemtActivityBroadcastReceiver = null;
		}
        //stopService(new Intent(this,hemtService.class));
	    stopService(new Intent(this,hemtService.class));
	}
	
	@Override
	public void onPause() {
	    super.onPause();
		if(hemtActivityBroadcastReceiver != null)
		{
			//unregisterReceiver(hemtActivityBroadcastReceiver);
			//hemtActivityBroadcastReceiver = null;
		}
	    Log.d("HEMT", "activity paused");
	}
	
	@Override
	public void onResume() {
	    super.onResume();
	    Log.d("HEMT", "activity resumed");
	}
	
	@Override
	public void onBackPressed() {
		super.onBackPressed();
		Log.d("HEMT", "onBackPressed Called");
		
		if(hemtActivityBroadcastReceiver != null)
		{
			unregisterReceiver(hemtActivityBroadcastReceiver);
			hemtActivityBroadcastReceiver = null;
			stopService(new Intent(this,hemtService.class));
		}
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}
    
    private BroadcastReceiver hemtActivityBroadcastReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            //Toast.makeText(getApplicationContext(), "received message in activity..!", Toast.LENGTH_SHORT).show();
        	//Log.d("HemtActivity", "received broadcast");
        	
        	if(intent.hasExtra(GET))
        	{       		
        		if(intent.getStringExtra(GET).equals(GET_DEVICE_NAME))
        		{
        			broadcastToHemtService(SET_DEVICE_NAME, mDeviceName);
        		}
        		
        		if(intent.getStringExtra(GET).equals(GET_DEVICE_ADDRESS))
        		{
        			broadcastToHemtService(SET_DEVICE_ADDRESS, mDeviceAddress);
        		}
        	}
        	
        	if(intent.hasExtra(SET_ACC_X))
        	{       		
        		double x = intent.getDoubleExtra(SET_ACC_X, 0.0);
        		
        		TextView tvX =(TextView)findViewById(R.id.textViewAccX);
        	    tvX.setText(Double.toString(x));
        	}
        	
        	if(intent.hasExtra(SET_ACC_Y))
        	{       		
        		double y = intent.getDoubleExtra(SET_ACC_Y, 0.0);
        		
        		TextView tvY =(TextView)findViewById(R.id.textViewAccY);
        	    tvY.setText(Double.toString(y));
        	}
        	
        	if(intent.hasExtra(SET_ACC_Z))
        	{       		
        		double z = intent.getDoubleExtra(SET_ACC_Z, 0.0);
        		
        		TextView tvZ =(TextView)findViewById(R.id.textViewAccZ);
        	    tvZ.setText(Double.toString(z));
        	}
        	
        	if(intent.hasExtra(SET_ACC_MEAN_X))
        	{       		
        		double mean = intent.getDoubleExtra(SET_ACC_MEAN_X, 0.0);
        		
        		TextView tv =(TextView)findViewById(R.id.textViewAccMeanX);
        	    tv.setText(Double.toString(mean));
        	}
        	
        	if(intent.hasExtra(SET_ACC_MEAN_Y))
        	{       		
        		double mean = intent.getDoubleExtra(SET_ACC_MEAN_Y, 0.0);
        		
        		TextView tv =(TextView)findViewById(R.id.textViewAccMeanY);
        	    tv.setText(Double.toString(mean));
        	}
        	
        	if(intent.hasExtra(SET_ACC_MEAN_Z))
        	{       		
        		double mean = intent.getDoubleExtra(SET_ACC_MEAN_Z, 0.0);
        		
        		TextView tv =(TextView)findViewById(R.id.textViewAccMeanZ);
        	    tv.setText(Double.toString(mean));
        	}
        	
        	if(intent.hasExtra(SET_ACC_COR_XY))
        	{       		
        		double cor = intent.getDoubleExtra(SET_ACC_COR_XY, 0.0);
        		
        		TextView tv =(TextView)findViewById(R.id.textViewAccCorXY);
        	    tv.setText(Double.toString(cor));
        	}
        	
        	if(intent.hasExtra(SET_ACC_COR_XZ))
        	{       		
        		double cor = intent.getDoubleExtra(SET_ACC_COR_XZ, 0.0);
        		
        		TextView tv =(TextView)findViewById(R.id.textViewAccCorXZ);
        	    tv.setText(Double.toString(cor));
        	}
        	
        	if(intent.hasExtra(SET_ACC_COR_YZ))
        	{       		
        		double cor = intent.getDoubleExtra(SET_ACC_COR_YZ, 0.0);
        		
        		TextView tv =(TextView)findViewById(R.id.textViewAccCorYZ);
        	    tv.setText(Double.toString(cor));
        	}
        	
        	if(intent.hasExtra(SET))
        	{
        		if(intent.getStringExtra(SET).equals(SET_BLE_CONNECTED))
        		{
            		ImageView ivC = (ImageView)findViewById(R.id.imageViewBluetoothConnection);
            		ivC.setImageResource(R.drawable.bluetoothblue);
        		}
        		if(intent.getStringExtra(SET).equals(SET_BLE_DISCONNECTED))
        		{
            		ImageView ivC = (ImageView)findViewById(R.id.imageViewBluetoothConnection);
            		ivC.setImageResource(R.drawable.bluetoothred);
        		}
        	}
        	
        	if(intent.hasExtra(SET_ACC_PREDICTION))
        	{
        		double prediction = intent.getDoubleExtra(SET_ACC_PREDICTION, -1.0);
        		if(prediction == 0.0)
        		{
        			TextView tv =(TextView)findViewById(R.id.textViewPrediction);
            	    tv.setText(Double.toString(prediction));
        		}
        		else if(prediction == 1.0)
        		{
        			TextView tv =(TextView)findViewById(R.id.textViewPrediction);
            	    tv.setText(Double.toString(prediction));
        		}
        		else
        		{
        			TextView tv =(TextView)findViewById(R.id.textViewPrediction);
            	    tv.setText(Double.toString(prediction));
        		}
        	}
        	
        	if(intent.hasExtra(SET_INFO_PACKET))
        	{
        		Bundle bundle=intent.getExtras();
        		InfoPacket m_oInfoPacket = (InfoPacket) bundle.getSerializable(SET_INFO_PACKET);
        		int f = m_oInfoPacket.getSamplingRate();
        		int id = m_oInfoPacket.getParameterId();
        	}
        	
        	if(intent.hasExtra(SET_DATA_PACKET))
        	{
        		Bundle bundle=intent.getExtras();
        		DataPacket m_oDataPacket = (DataPacket) bundle.getSerializable(SET_DATA_PACKET);
        		
        		if(m_oDataPacket.getParameterId() == 1)
        		{
        			
        			byte[] buffer = m_oDataPacket.getByteBuffer();

        			double[] x = new double[750];
        			double[] y = new double[250];
        			double[] z = new double[250];
        			
        			for (int i = 0; i < 750; i++)
        			{	
        				byte[] value = new byte[8];
        				value[0] = buffer[8*i];
        				value[1] = buffer[8*i+1];
        				value[2] = buffer[8*i+2];
        				value[3] = buffer[8*i+3];
        				value[4] = buffer[8*i+4];
        				value[5] = buffer[8*i+5];
        				value[6] = buffer[8*i+6];
        				value[7] = buffer[8*i+7];
        				
        				x[i] = ByteBuffer.wrap(value).getDouble();
        				//y[i] = buffer[3 * i + 1];
        				//z[i] = buffer[3 * i + 2];
        			}

        			int num = 0;
        			num++;
        		}

        	}
        }
    };
    
    private void broadcastToHemtService(String action, String value)
    {
        Intent newIntent = new Intent();
        newIntent.setAction(ACTION_STRING_SERVICE);
        newIntent.putExtra(action, value);
        sendBroadcast(newIntent);
    }
    
    private void broadcastToHemtService(String action, int value)
    {
        Intent newIntent = new Intent();
        newIntent.setAction(ACTION_STRING_SERVICE);
        newIntent.putExtra(action, value);
        sendBroadcast(newIntent);
    }
    
    public void onRadioButtonClicked(View view)
    {
        // Is the button now checked?
        boolean checked = ((RadioButton) view).isChecked();
        
        // Check which radio button was clicked
        switch(view.getId())
        {
            case R.id.radioButtonStand:
                if (checked)
                {
                    RadioButton rOne = (RadioButton) findViewById(R.id.radioButtonWalk);
                    rOne.setChecked(false);
                    RadioButton rTwo = (RadioButton) findViewById(R.id.radioButtonRun);
                    rTwo.setChecked(false);
                    m_iRecordingMode = STAND;
                } 	
                break;
            case R.id.radioButtonWalk:
                if (checked)
                {
                    RadioButton rOne = (RadioButton) findViewById(R.id.radioButtonStand);
                    rOne.setChecked(false);
                    RadioButton rTwo = (RadioButton) findViewById(R.id.radioButtonRun);
                    rTwo.setChecked(false);
                    m_iRecordingMode = WALK;
                }
                break;
            case R.id.radioButtonRun:
                if (checked)
                {
                    RadioButton rOne = (RadioButton) findViewById(R.id.radioButtonStand);
                    rOne.setChecked(false);
                    RadioButton rTwo = (RadioButton) findViewById(R.id.radioButtonWalk);
                    rTwo.setChecked(false);
                    m_iRecordingMode = RUN;
                }
                break;
        }
    }
    
	 public void recordData(View view)
	 {
		 if(m_bRecording == false)
		 {
			 broadcastToHemtService(SET_START_RECORDING, m_iRecordingMode);
			 m_bRecording = true;
			 Button button = (Button) findViewById(R.id.buttonRecord);
			 button.setText("Stop Recording");
		 }
		 else
		 {
			 broadcastToHemtService(SET_STOP_RECORDING, m_iRecordingMode);
			 m_bRecording = false;
			 
			 Button button = (Button) findViewById(R.id.buttonRecord);
			 button.setText("Record");
			 
			 File root = android.os.Environment.getExternalStorageDirectory();
			 File dir = new File (root.getAbsolutePath() + "/Download");
		     dir.mkdirs();
		     File file = new File(dir, "Data.txt");
			 MediaScannerConnection.scanFile(this, new String[] { file.getAbsolutePath() }, null, null);
		 }
	 }
	 
	 
	
}
