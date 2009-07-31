package com.dsrts.xdata;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.Types;
import java.util.HashMap;
import java.util.Map;

public class XDataImport extends XDataBase {

	public static void main(String[] args) throws Exception {
		importData(args[0]);
	}
	
	private static void importData(String file) throws Exception {

		final String insert_usgs = "insert into nationaldata (featureid,featurename,class,st_alpha,st_num,county,county_num,lat,lon,s_lat,s_lon,height,map_name) values (?,?,?,?,?,?,?,?,?,?,?,?,?)";
		
		Connection c = getConnection(file);
		
		PreparedStatement stmt = c.prepareStatement(insert_usgs);
		
		File fInData = new File(file);
		FileReader fReader = new FileReader(fInData);
		BufferedReader reader = new BufferedReader(fReader);
		reader.readLine();
		
		boolean loop = true;
		int pointsProcessed = 0;
		int pointsKept = 0;

		while(loop) {
			String data = reader.readLine();
			if(data == null) {
				loop = false;
				continue;
			}
			
			pointsProcessed++;
			
			StringBuilder row = new StringBuilder(1024);
			String[] values = data.split("\t");

			Integer featureId = new Integer(values[0]);
			String pointName = values[1];
			String pointClass = values[2].toUpperCase();
			String st_alpha = values[3];
			Integer st_num = new Integer(values[4]);
			String county = values[5];
			Integer county_num = new Integer(values[6]);
			
			String s_pointLat = values[9];
			Double pointLat = null;
			try {
				pointLat = new Double(s_pointLat);
			} catch (Exception e) {
				; // nothing
			}
			String s_pointLon = values[10];
			Double pointLon = null;
			try {
				pointLon = new Double(s_pointLon);
			} catch (Exception e) {
				; // nothing
			}
			
			String s_pointSourceLat = values[13];
			Double pointSourceLat = null;
			try {
				pointSourceLat = new Double(s_pointSourceLat);
			} catch (Exception e) {
				; // nothing
			}
			String s_pointSourceLon = values[14];
			Double pointSourceLon = null;
			try {
				pointSourceLon = new Double(s_pointSourceLon);
			} catch (Exception e) {
				; // nothing
			}

			String s_height = values[15];
			Integer height = null;
			try {
				height = new Integer(s_height);
			} catch (Exception e) {
				; // nothing
			}
			
			String mapName = values[16];
			
			stmt.setInt(1, featureId);
			stmt.setString(2,pointName);
			stmt.setString(3,pointClass);
			stmt.setString(4,st_alpha);
			stmt.setInt(5,st_num);
			stmt.setString(6,county);
			stmt.setInt(7,county_num);
			
			if(pointLat == null) { stmt.setNull(8, Types.DECIMAL); } else { stmt.setDouble(8,pointLat); }
			if(pointLon == null) { stmt.setNull(9, Types.DECIMAL); } else { stmt.setDouble(9,pointLon); }
			
			if(pointSourceLat == null) { stmt.setNull(10, Types.DECIMAL); } else { stmt.setDouble(10,pointSourceLat); }
			if(pointSourceLon == null) { stmt.setNull(11, Types.DECIMAL); } else { stmt.setDouble(11,pointSourceLon); }

			if(height == null) { stmt.setNull(12, Types.DECIMAL); } else { stmt.setInt(12,height); }

			stmt.setString(13,mapName);
			
			stmt.execute();
			pointsKept++;
			
			if(pointsKept % 2000 == 0) {
				c.commit();
			}
		}
		
		c.commit();
		stmt.close();
		c.close();
		
//		System.out.println("class types dropped : " + unmappedClass.keySet());
		System.out.println("points processed : " + pointsProcessed);
//		System.out.println("points kept : " + pointsKept);
		
	}	
}
