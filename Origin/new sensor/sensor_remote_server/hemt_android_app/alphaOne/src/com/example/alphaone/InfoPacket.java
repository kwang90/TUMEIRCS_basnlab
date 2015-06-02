package com.example.alphaone;

import java.io.Serializable;

public class InfoPacket implements Serializable
{
	private static final long serialVersionUID = 100;
	
	private String m_oSensorName;
	private int m_iSensorVersion;
	private String m_oSensorId;
	private int m_iParameterId;
	private int m_iSamplingRate;
	private long m_oTime;
	private int m_iQuality;
	private int m_iEfficency;
	
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
	
	public void setParameterId(int id)
	{
		m_iParameterId = id;
	}
	
	public int getParameterId()
	{
		return m_iParameterId;
	}
	
	public void setSamplingRate(int samplingRate)
	{
		m_iSamplingRate = samplingRate;
	}
	
	public int getSamplingRate()
	{
		return m_iSamplingRate;
	}
	
	public void setTime(long time)
	{
		m_oTime = time;
	}
	
	public long getTime()
	{
		return m_oTime;
	}
	
	public void setQuality(int quality)
	{
		m_iQuality = quality;
	}
	
	public int getQuality()
	{
		return m_iQuality;
	}
	
	public void setEfficency(int efficency)
	{
		m_iEfficency = efficency;
	}
	
	public int getEfficency()
	{
		return m_iEfficency;
	}
}
