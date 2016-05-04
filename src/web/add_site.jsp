<%@ page language="java" contentType="text/html; charset=UTF-8" pageEncoding="UTF-8"%>
<%@ page import="java.util.*,java.sql.*,com.zhilongc.jsp.*"%>

<!DOCTYPE HTML> 

<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title> 添加网站信息到数据库 </title>
<%
    String siteName = (String)request.getParameter("siteName");
    String siteAccount = (String)request.getParameter("siteAccount");
    String password = (String)request.getParameter("password");
    String preparedCmd = "insert into website_table(siteName, siteAccount, sitePwd) values(?,?,?)";
    out.println(preparedCmd);

    Connection con=null;
    ResultSet rst=null;
    PreparedStatement stmt=null;
    try{
        String url="jdbc:mysql://localhost:3306/db_mysec?user=root&password=123456";
        Class.forName("com.mysql.jdbc.Driver").newInstance();
        int i=1;
        con=DriverManager.getConnection(url);
        stmt=con.prepareStatement(preparedCmd);
        //stmt.setString(1, "2");
        stmt.setString(1, siteName);
        stmt.setString(2, siteAccount);
        stmt.setString(3, password);
        stmt.execute();
        rst.close();
        stmt.close();
        con.close();
    }catch(Exception e){
        out.println(e.getMessage());
    }
%>
</head>

<body>
</body>

</html>

