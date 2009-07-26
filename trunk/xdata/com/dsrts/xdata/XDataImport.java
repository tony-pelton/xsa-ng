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
		importData();		
    }
	
	private static void importData() throws Exception {

		/*
create table nationaldata (
id int NOT NULL GENERATED ALWAYS AS IDENTITY,
featureid int,
featurename varchar(255),
class varchar(30),
st_alpha varchar(30),
st_num int,
county varchar(30),
county_num int,
lat numeric(10,6),
lon numeric(10,6),
s_lat numeric(10,6),
s_lon numeric(10,6),
height numeric,
map_name varchar(30)
);
		 */

		final String insert_usgs = "insert into nationaldata (featureid,featurename,class,st_alpha,st_num,county,county_num,lat,lon,s_lat,s_lon,height,map_name) values (?,?,?,?,?,?,?,?,?,?,?,?,?)";
		
		Connection c = getConnection();
		
		PreparedStatement stmt = c.prepareStatement(insert_usgs);
		
		File fInData = new File("/gnisdata/NationalFile_combined.csv");
		FileReader fReader = new FileReader(fInData);
		BufferedReader reader = new BufferedReader(fReader);
		reader.readLine();		

//		Map<String,Boolean> unmappedClass = new HashMap<String,Boolean>();
		
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
			
	/*
0 [Feature_ID]
0 [Feature_Name]
0 [Class]
0 [ST_alpha]
0 [ST_num]
0 [County]
0 [County_num]
0 [Primary_lat_DMS]
0 [Primary_lon_DMS]
0 [Primary_lat_dec]
0 [Primary_lon_dec]
0 [Source_lat_DMS]
0 [Source_lon_DMS]
0 [Source_lat_dec]
0 [Source_lon_dec]
0 [Elev(Meters)]
0 [Map_Name]
	 */

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
			
//			if(pointsKept > 20000) { loop = false; }
		}
		
		c.commit();
		stmt.close();
		c.close();
		
//		System.out.println("class types dropped : " + unmappedClass.keySet());
		System.out.println("points processed : " + pointsProcessed);
//		System.out.println("points kept : " + pointsKept);
		
	}	
}
