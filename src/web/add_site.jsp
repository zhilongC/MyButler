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
    DBManager db = new DBManager();
    db.insert(siteName, siteAccount, password);
%>
</head>

<body>
</body>

</html>

