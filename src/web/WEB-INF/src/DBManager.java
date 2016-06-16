package com.zhilongc.jsp;

import java.sql.*;
import java.util.*;

public class DBManager{

    private Connection con = null;
    private ResultSet rst = null;
    private Statement stmt = null;
    private PreparedStatement prestmt=null;
    final static private String url="jdbc:mysql://localhost:3306/db_mysec?user=root&password=123456";

    public DBManager(){
        try{
            Class.forName("com.mysql.jdbc.Driver").newInstance();
            con=DriverManager.getConnection(url);
        }catch(Exception e){
            System.out.println(e.getMessage());
        }
    }

    public List queryAll() throws SQLException{
        List list = new ArrayList();
        stmt=con.createStatement();
        rst=stmt.executeQuery("select * from website_table");

        while(rst.next()){
            WebsiteInfo info = new WebsiteInfo();
            info.setSiteName(rst.getString("siteName"));
            info.setSiteAccount(rst.getString("siteAccount"));
            info.setPassword(rst.getString("sitePwd"));
            System.out.println(info.getSiteName());
            System.out.println(info.getSiteAccount());
            System.out.println(info.getPassword());
            list.add(info);
        }

        return list;
    }
    public int insert(String siteName, String siteAccount, String password) throws SQLException{
        String preparedCmd = "insert into website_table(siteName, siteAccount, sitePwd) values(?,?,?)";
        prestmt=con.prepareStatement(preparedCmd);
        prestmt.setString(1, siteName);
        prestmt.setString(2, siteAccount);
        prestmt.setString(3, password);
        prestmt.execute();
        return 0;
    }
}
