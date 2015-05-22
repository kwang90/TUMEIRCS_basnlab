package com.example.matze.broadcastreceiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;

//Import hemt library for Info- and Datapacket
import java.nio.ByteBuffer;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

import hemt.*;


public class MainActivity extends ActionBarActivity {

    private static final String TAG = MainActivity.class.getSimpleName();

    private static final String ACTION_BROADCAST_DATABASE = "ToDataBaseService";
    private static final String INFOPACKET = "InfoPacket";
    private static final String DATAPACKET = "DataPacket";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (ServiceBroadcastReceiverDatabase != null)
        {
            //Create an intent filter to listen to the broadcast sent with the action "ACTION_STRING_SERVICE"
            IntentFilter intentFilter = new IntentFilter(ACTION_BROADCAST_DATABASE);
            //Map the intent filter to the receiver
            registerReceiver(ServiceBroadcastReceiverDatabase, intentFilter);
            Log.d(TAG, "Hemt serviceReceiver");
        }
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private BroadcastReceiver ServiceBroadcastReceiverDatabase = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {

            if(intent.hasExtra(INFOPACKET))
            {
                Bundle bundle=intent.getExtras();
                InfoPacket infoPacket = (InfoPacket) bundle.getSerializable(INFOPACKET);

                Date date = new Date(infoPacket.getTime());
                DateFormat df = new SimpleDateFormat("HH:mm:ss dd/MM/yyyy");
                String reportDate = df.format(date);

                TextView viewSensorName = (TextView) findViewById(R.id.textViewInfoSensorName);
                viewSensorName.setText(infoPacket.getSensorName());

                TextView viewSensorVersion = (TextView) findViewById(R.id.textViewInfoSensorVersion);
                viewSensorVersion.setText(Integer.toString(infoPacket.getSensorVersion()));

                TextView viewSensorId = (TextView) findViewById(R.id.textViewInfoSensorId);
                viewSensorId.setText(infoPacket.getSensorId());

                TextView viewParameterId = (TextView) findViewById(R.id.textViewInfoParameterId);
                viewParameterId.setText(Integer.toString(infoPacket.getParameterId()));

                TextView viewSamplingRate = (TextView) findViewById(R.id.textViewInfoSamplingRate);
                viewSamplingRate.setText(Integer.toString(infoPacket.getSamplingRate()));

                TextView viewTime = (TextView) findViewById(R.id.textViewInfoTime);
                viewTime.setText(reportDate);

                TextView viewQuality = (TextView) findViewById(R.id.textViewInfoQuality);
                viewQuality.setText(Integer.toString(infoPacket.getQuality()));

                TextView viewEfficiency = (TextView) findViewById(R.id.textViewInfoEfficiency);
                viewEfficiency.setText(Integer.toString(infoPacket.getEfficiency()));
            }

            if(intent.hasExtra(DATAPACKET))
            {
                Bundle bundle=intent.getExtras();
                DataPacket dataPacket = (DataPacket) bundle.getSerializable(DATAPACKET);

                TextView viewSensorName = (TextView) findViewById(R.id.textViewDataSensorName);
                viewSensorName.setText(dataPacket.getSensorName());

                TextView viewSensorVersion = (TextView) findViewById(R.id.textViewDataSensorVersion);
                viewSensorVersion.setText(Integer.toString(dataPacket.getSensorVersion()));

                TextView viewParameterId = (TextView) findViewById(R.id.textViewDataParameterId);
                viewParameterId.setText(Integer.toString(dataPacket.getParameterId()));

                short[] x = new short[250];
                short[] y = new short[250];
                short[] z = new short[250];

                if(dataPacket.getParameterId() == 1)
                {

                    byte[] buffer = dataPacket.getByteBuffer();

                    for (int i = 0; i < 250; i++)
                    {
                        byte[] valueX = new byte[2];
                        byte[] valueY = new byte[2];
                        byte[] valueZ = new byte[2];

                        valueX[0] = buffer[6*i];
                        valueX[1] = buffer[6*i+1];
                        valueY[0] = buffer[6*i+2];
                        valueY[1] = buffer[6*i+3];
                        valueZ[0] = buffer[6*i+4];
                        valueZ[1] = buffer[6*i+5];

                        x[i] = ByteBuffer.wrap(valueX).getShort();
                        y[i] = ByteBuffer.wrap(valueY).getShort();
                        z[i] = ByteBuffer.wrap(valueZ).getShort();

                        TextView viewX = (TextView) findViewById(R.id.textViewDataAccX);
                        viewX.setText(Short.toString(x[i]));

                        TextView viewY = (TextView) findViewById(R.id.textViewDataAccY);
                        viewY.setText(Short.toString(y[i]));

                        TextView viewZ = (TextView) findViewById(R.id.textViewDataAccZ);
                        viewZ.setText(Short.toString(z[i]));
                    }
                }
            }
        }
    };
}
