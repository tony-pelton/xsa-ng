/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.dsrts.xdata;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.HashMap;
import java.util.Map;

/**
 *
 * @author tpelton
 */
public class XDataExport extends XDataBase {
	
	final private static Integer TYPE_MUNICIPAL = 21;
	final private static Integer TYPE_TERRAIN = 22;
	final private static Integer TYPE_CIVIL = 23;
	
	final private static Map<String,Integer> mapTypeToClass = new HashMap<String,Integer>();
	
	static {
//		mapTypeToClass.put("CEMETERY", TYPE_MUNICIPAL);
//		mapTypeToClass.put("CHURCH", TYPE_MUNICIPAL);
//		mapTypeToClass.put("SCHOOL", TYPE_MUNICIPAL);
//		mapTypeToClass.put("HOSPITAL", TYPE_MUNICIPAL);
//		mapTypeToClass.put("BUILDING", TYPE_MUNICIPAL);
		mapTypeToClass.put("TOWER", TYPE_MUNICIPAL);
		mapTypeToClass.put("CIVIL", TYPE_MUNICIPAL);
//		mapTypeToClass.put("PARK", TYPE_MUNICIPAL);
		mapTypeToClass.put("MILITARY", TYPE_MUNICIPAL);
		mapTypeToClass.put("OILFIELD", TYPE_MUNICIPAL);
//		mapTypeToClass.put("TUNNEL", TYPE_MUNICIPAL);
//		mapTypeToClass.put("LEVEE", TYPE_MUNICIPAL);
		mapTypeToClass.put("RESERVOIR", TYPE_MUNICIPAL);
//		mapTypeToClass.put("MINE", TYPE_MUNICIPAL);
		mapTypeToClass.put("BRIDGE", TYPE_MUNICIPAL);
//		mapTypeToClass.put("DAM", TYPE_MUNICIPAL);
//		mapTypeToClass.put("ARCH", TYPE_MUNICIPAL);

		// ----------------------------
		mapTypeToClass.put("SUMMIT", TYPE_TERRAIN);
//		mapTypeToClass.put("CRATER", TYPE_TERRAIN);
//		mapTypeToClass.put("GEYSER", TYPE_TERRAIN);
//		mapTypeToClass.put("ARROYO", TYPE_TERRAIN);
//		mapTypeToClass.put("LAVA", TYPE_TERRAIN);
//		mapTypeToClass.put("WOODS", TYPE_TERRAIN);
		mapTypeToClass.put("LAKE", TYPE_TERRAIN);
//		mapTypeToClass.put("STREAM", TYPE_TERRAIN);
		mapTypeToClass.put("BAY", TYPE_TERRAIN);
		mapTypeToClass.put("HARBOR", TYPE_TERRAIN);
		mapTypeToClass.put("CAPE", TYPE_TERRAIN);
//		mapTypeToClass.put("SWAMP", TYPE_TERRAIN);
//		mapTypeToClass.put("CLIFF", TYPE_TERRAIN);
		mapTypeToClass.put("VALLEY", TYPE_TERRAIN);
//		mapTypeToClass.put("RAPIDS", TYPE_TERRAIN);
		mapTypeToClass.put("FALLS", TYPE_TERRAIN);
//		mapTypeToClass.put("GAP", TYPE_TERRAIN);
//		mapTypeToClass.put("FLAT", TYPE_TERRAIN);
//		mapTypeToClass.put("BENCH", TYPE_TERRAIN);
//		mapTypeToClass.put("BASIN", TYPE_TERRAIN);
//		mapTypeToClass.put("RANGE", TYPE_TERRAIN);
//		mapTypeToClass.put("FOREST", TYPE_TERRAIN);
//		mapTypeToClass.put("SPRING", TYPE_TERRAIN);
//		mapTypeToClass.put("PILLAR", TYPE_TERRAIN);
//		mapTypeToClass.put("BEND", TYPE_TERRAIN);
		mapTypeToClass.put("RESERVE", TYPE_TERRAIN);
//		mapTypeToClass.put("GUT", TYPE_TERRAIN);
//		mapTypeToClass.put("CANAL", TYPE_TERRAIN);
//		mapTypeToClass.put("SLOPE", TYPE_TERRAIN);
//		mapTypeToClass.put("PLAIN", TYPE_TERRAIN);
		mapTypeToClass.put("RIDGE", TYPE_TERRAIN);
//		mapTypeToClass.put("BAR", TYPE_TERRAIN);
//		mapTypeToClass.put("TRAIL", TYPE_TERRAIN);
//		mapTypeToClass.put("BEACH", TYPE_TERRAIN);

		// ----------------------------
		mapTypeToClass.put("POPULATED PLACE", TYPE_CIVIL);
//		mapTypeToClass.put("LOCALE", TYPE_CIVIL);
		mapTypeToClass.put("CROSSING", TYPE_CIVIL);
		mapTypeToClass.put("ISLAND", TYPE_CIVIL);
		mapTypeToClass.put("AREA", TYPE_CIVIL);
//		mapTypeToClass.put("CHANNEL", TYPE_CIVIL);
	}
	
	public static void main(String argv[]) throws ClassNotFoundException, SQLException, IOException {		
		exportData();		
	}
	
	private static void exportData() throws ClassNotFoundException, SQLException, IOException {

		File fOutData = new File("xdata.dlm");
		if(fOutData.exists()) {
			fOutData.delete();
		}		
		
		FileWriter fWriter = new FileWriter(fOutData);
		BufferedWriter writer = new BufferedWriter(fWriter);
		
		// layer|lat|lon|height|type|id|name
		// final String insert_usgs = "select lat,lon,height,class,featurename from data_v where st_alpha = 'HI' and pri_good = 1";
		final String select_usgs = "select lat,lon,height,class,featurename from data_v where pri_good = 1 order by lat,lon";
		Connection c = getConnection();		
		Statement stmt = c.createStatement();		
		ResultSet rs = stmt.executeQuery(select_usgs);
		
		int pointsRendered = 0;
		int pointsProcessed = 0;
		StringBuilder row = new StringBuilder(2048);
		while(rs.next()) {
			pointsProcessed++;
			String landclass = rs.getString("class");
			Integer type = mapTypeToClass.get(landclass);
			if(type == null) {
				continue;
			}
			String featurename = rs.getObject("featurename").toString();
			if(featurename.contains("(historical)")) { continue; }
			
			row.append("1|");
			row.append(rs.getObject("lat").toString());
			row.append("|");
			row.append(rs.getObject("lon").toString());
			row.append("|");
			row.append(rs.getObject("height").toString());
			row.append("|");
			row.append(type.toString());
			row.append("|");			
			row.append(landclass);
			row.append("|");			
			row.append(new String(featurename.getBytes("US-ASCII")));
			
			row.append("\n");
			writer.write(row.toString());
			pointsRendered++;
			row.setLength(0);
		}
		
		writer.flush();
		writer.close();
		c.commit();
		c.setAutoCommit(true);
		stmt.close();
		c.close();
		System.out.println("points processed : " + pointsProcessed);
		System.out.println("points rendered : " + pointsRendered);
	}
}
