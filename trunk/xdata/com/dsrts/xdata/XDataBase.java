/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.dsrts.xdata;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

/**
 *
 * @author tpelton
 */
public class XDataBase {
	static boolean load_driver = true;
	static Connection getConnection() throws ClassNotFoundException, SQLException {
		if(load_driver) { Class.forName("org.hsqldb.jdbcDriver"); load_driver = false; }
		Connection c = DriverManager.getConnection("jdbc:hsqldb:file:testdb", "sa", "");
		c.setAutoCommit(false);
		return c;
	}
}
