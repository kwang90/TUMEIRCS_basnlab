package com.example.matze.testnav;

/**
 * Created by Mathias Gopp on 15.05.2015.
 *
 * In this class the global variables are registered.
 * The Sensor Types are:
 *      0   No Sensor
 *      1   RCS Sensor
 *      2   IIS Sensor
 */
public class Global
{
    public final static String RCS_SENSOR_NAME = "RCS E1CH";
    public final static String IIS_SENSOR_NAME = "";

    public final static int RCS_SENSOR_ID = 1;
    public final static int IIS_SENSOR_ID = 2;

    private static int SENSOR = 0;

    public static int getSensorId()
    {
        return SENSOR;
    }

    //If you do not want to change the var ever then do not include this
    public static void setSensorId(int sensor){
        SENSOR = sensor;
    }

    //public strings for the shared preferences
    public static final String HE2MT_PREFERENCES = "HE2mt_Preferences";
    public static final String DEVELOPER_OPTIONS = "Developer_Options";
    public static final String AUTOCONNECT_BLE = "Autoconnect_Ble";
    public static final String AUTOCONNECT_BLE_DEVICEADDRESS = "Autoconnect_Ble_Deviceaddress";
    public static final String AUTOCONNECT_BLE_DEVICENAME = "Autoconnect_Ble_Devicename";
    public static final String AUTOCONNECT_BLE_SENSOR_ID = "Autoconnect_Ble_Id";
}
