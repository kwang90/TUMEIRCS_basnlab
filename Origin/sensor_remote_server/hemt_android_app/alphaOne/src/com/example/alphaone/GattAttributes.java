package com.example.alphaone;

import java.util.HashMap;

public class GattAttributes {
	private static HashMap<String, String> attributes = new HashMap();
    
	public static String CH_RCS_RAW_DATA_WRITE = "6a652ce5-c50a-4157-b990-d4e7d9cc0a53";
	public static String CH_RCS_RAW_DATA_READ = "d195b840-0333-4e7a-8e0d-429499d3baa1";

    static {
        //Services.
    	attributes.put("fc3b898f-fefa-4fb5-97dc-df8fab78e93e", "RCS Raw Data Transmission");
    	
        //Characteristics.
		attributes.put(CH_RCS_RAW_DATA_WRITE, "Raw Data Write");
		attributes.put(CH_RCS_RAW_DATA_READ, "Raw Data Read");
    }

    public static String lookup(String uuid, String defaultName) {
        String name = attributes.get(uuid);
        return name == null ? defaultName : name;
    }

}
