package com.example.alphaone;

import java.util.ArrayList;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.Color;
import android.graphics.PorterDuff.Mode;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;

public class ScanActivity extends Activity {
	
    private static final String SET_DEVICE_NAME = "SetDeviceName";
    private static final String SET_DEVICE_ADDRESS = "SetDeviceAddress";
    
    private LeDeviceListAdapter mLeDeviceListAdapter;
    private BluetoothAdapter mBluetoothAdapter;
    private boolean mScanning;
    private static final long SCAN_PERIOD = 10000;
    private Handler mHandler;
    private final static int REQUEST_ENABLE_BT = 1;
    
	private String mDeviceName;
	private String mDeviceAddress;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_scan);
		
		mHandler = new Handler();
		
		final BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = bluetoothManager.getAdapter();
        
        if (mBluetoothAdapter == null) {
            Toast.makeText(this, "not supported" , Toast.LENGTH_SHORT).show();
            finish();
            return;
        }
        
        mLeDeviceListAdapter = new LeDeviceListAdapter();
        ListView listView = (ListView) findViewById(R.id.listViewBleDevices);
        listView.setAdapter(mLeDeviceListAdapter);
        listView.setOnItemClickListener(mDeviceClickedHandler);
        listView.setChoiceMode(ListView.CHOICE_MODE_SINGLE);
        listView.setSelector(android.R.color.holo_blue_bright);
        
        ProgressBar progBar = (ProgressBar) findViewById(R.id.progressBarScan);
        progBar.setVisibility(View.INVISIBLE);
        
        mScanning = false;
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.scan, menu);
		return true;
	}

	@Override
	public void onDestroy() {
	    super.onDestroy();  // Always call the superclass
	    
	    // Stop method tracing that the activity started during onCreate()
	    Log.d("Scan", "activity destroyed");
	    //unbindService(mServiceConnection);
	}
	
	@Override
	public void onPause() {
	    super.onPause();  // Always call the superclass method first

	    // Release the Camera because we don't need it when paused
	    // and other activities might need to use it.
	    Log.d("Scan", "activity paused");
	    //unbindService(mServiceConnection);
	}
	
	@Override
	public void onResume() {
	    super.onResume();  // Always call the superclass method first

	    Log.d("Scan", "activity resumed");
	    //Intent intent= new Intent(this, HemtService.class);
	    //bindService(intent, mServiceConnection, Context.BIND_AUTO_CREATE);
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
	
	/** Called when the user touches the button scan*/
	public void startScan(View view)
	{
		Log.d("ScanActivity", "scan button pressed");
		
		if(mScanning == false)
		{
			scanForBleDevices(true);
		}
		else
		{
			scanForBleDevices(false);
		}
	}
	
	public void changeActivity(View view)
	{
		Intent intent = new Intent(this, HemtActivity.class);
		startActivity(intent);
	}
	 
	 public void handleProgressBar()
	 {
		 if(mScanning == true)
		 {
		        ProgressBar progBar = (ProgressBar) findViewById(R.id.progressBarScan);
		        progBar.getIndeterminateDrawable().setColorFilter(0xff0000ff, android.graphics.PorterDuff.Mode.SRC_ATOP);
		        progBar.setVisibility(View.VISIBLE);
		 }
		 else
		 {
		        ProgressBar progBar = (ProgressBar) findViewById(R.id.progressBarScan);
		        progBar.setVisibility(View.INVISIBLE);
		 }
	 }
	 
    private void scanForBleDevices(final boolean enable)
    {
    	if (!mBluetoothAdapter.isEnabled())
    	{
    	    Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
    	    startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
    	}
    	else
    	{
	        if(enable)
	        {
	            mHandler.postDelayed(new Runnable()
	            {
	                @Override
	                public void run() {
	                    mScanning = false;
	                    mBluetoothAdapter.stopLeScan(mBleScanCallback);
	                    handleProgressBar();
	                }
	            }, SCAN_PERIOD);
	            mLeDeviceListAdapter.clear();
	            mLeDeviceListAdapter.notifyDataSetChanged();
	            mScanning = true;
	            mBluetoothAdapter.startLeScan(mBleScanCallback);
	            this.handleProgressBar();
	        }
	        else
	        {
	            mScanning = false;
	            mBluetoothAdapter.stopLeScan(mBleScanCallback);
	            this.handleProgressBar();
	        }
	        
	        invalidateOptionsMenu();
    	}
    }
    
    private BluetoothAdapter.LeScanCallback mBleScanCallback = new BluetoothAdapter.LeScanCallback() {
        @Override
        public void onLeScan(final BluetoothDevice device,final int rssi, byte[] scanRecord) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    mLeDeviceListAdapter.addDevice(device, rssi);
                    mLeDeviceListAdapter.notifyDataSetChanged();
                }
            });
        }
    };
    
	private OnItemClickListener mDeviceClickedHandler = new OnItemClickListener()
	{
	    public void onItemClick(AdapterView parent, View v, int position, long id)
	    {
	    	final BluetoothDevice device = mLeDeviceListAdapter.getDevice(position);
	        if(device == null)
        	{
	        	return;
        	}

            mDeviceName = device.getName();
            mDeviceAddress = device.getAddress();
            
	        if (mScanning)
	        {
	            mBluetoothAdapter.stopLeScan(mBleScanCallback);
	            mScanning = false;
	        }
	        
			Intent intent = new Intent(v.getContext(), HemtActivity.class);
			intent.putExtra(SET_DEVICE_NAME, mDeviceName);
			intent.putExtra(SET_DEVICE_ADDRESS, mDeviceAddress);
			startActivity(intent);
	    }
	};
	 
	 private class LeDeviceListAdapter extends BaseAdapter
	 {
	        private ArrayList<BluetoothDevice> mLeDevices;
	        private ArrayList<Integer> mRssi;
	        private LayoutInflater mInflator;

	        public LeDeviceListAdapter()
	        {
	            super();
	            mLeDevices = new ArrayList<BluetoothDevice>();
	            mRssi = new ArrayList<Integer>();
	            mInflator = ScanActivity.this.getLayoutInflater();
	        }

	        public void addDevice(BluetoothDevice device, int rssi)
	        {
	            if(!mLeDevices.contains(device))
	            {
	                mLeDevices.add(device);
	                mRssi.add(Integer.valueOf(rssi));
	            }
	        }

	        public BluetoothDevice getDevice(int position)
	        {
	            return mLeDevices.get(position);
	        }

	        public void clear()
	        {
	            mLeDevices.clear();
	            mRssi.clear();
	        }

	        @Override
	        public int getCount()
	        {
	            return mLeDevices.size();
	        }

	        @Override
	        public Object getItem(int i)
	        {
	            return mLeDevices.get(i);
	        }

	        @Override
	        public long getItemId(int i)
	        {
	            return i;
	        }

	        @Override
	        public View getView(int i, View view, ViewGroup viewGroup)
	        {
	            ViewHolder viewHolder;
	            // General ListView optimization code.
	            if (view == null)
	            {
	                view = mInflator.inflate(R.layout.listitem_device, null);
	                viewHolder = new ViewHolder();
	                viewHolder.deviceAddress = (TextView) view.findViewById(R.id.textViewListItemDevice);
	                viewHolder.deviceName = (TextView) view.findViewById(R.id.textViewListItemName);
	                viewHolder.deviceRssi = (TextView) view.findViewById(R.id.textViewListItemRssi);
	                view.setTag(viewHolder);
	            }
	            else
	            {
	                viewHolder = (ViewHolder) view.getTag();
	            }

	            BluetoothDevice device = mLeDevices.get(i);
	            final String deviceName = device.getName();
	            if (deviceName != null && deviceName.length() > 0)
	            {
	                viewHolder.deviceName.setText(deviceName);
	            }
	            else
	            {
	                viewHolder.deviceName.setText("unknown");
	            }
	            viewHolder.deviceAddress.setText(device.getAddress());
	            viewHolder.deviceRssi.setText(String.valueOf(mRssi.get(i))+"dBm");

	            return view;
	        }
	    }
	   
	    static class ViewHolder
	    {
	        TextView deviceName;
	        TextView deviceAddress;
	        TextView deviceRssi;
	    }
}
