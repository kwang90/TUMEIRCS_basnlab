package com.example.alphaone;

import java.io.Serializable;

public class DataPacket implements Serializable
{
	private static final long serialVersionUID = 200;

	private String m_oSensorName;
	private int m_iSensorVersion;
	private String m_oSensorId;
	private int m_iParameterId;
	private byte[] m_oByteBuffer;
	
	
	public void setSensorName(String name)
	{
		m_oSensorName = name;
	}
	
	public String getSensorName()
	{
		return m_oSensorName;
	}
	
	public void setSensorVersion(int version)
	{
		m_iSensorVersion = version;
	}
	
	public int getSensorVersion()
	{
		return m_iSensorVersion;
	}
	
	public void setSensorId(String id)
	{
		m_oSensorId = id;
	}
	
	public String getSensorId()
	{
		return m_oSensorId;
	}
	
	public void setParamterId(int id)
	{
		m_iParameterId = id;
	}
	
	public void setByteBuffer(byte[] buffer)
	{
		m_oByteBuffer = buffer;
	}
	
	public int getParameterId()
	{
		return m_iParameterId;
	}
	
	public byte[] getByteBuffer()
	{
		return m_oByteBuffer;
	}
	
}