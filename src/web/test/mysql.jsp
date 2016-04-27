<%@ page language="java" import="java.sql.*"%>
<html>
<head><title>Read</title>
</head>
<body>

<p align="center"><b>Following records are selected from the 'jakartaproject' table.</b><br>&nbsp;</p>
<%
Connection con=null;
ResultSet rst=null;
Statement stmt=null;
try{
String url="jdbc:mysql://localhost:3306/db_mysec?user=root&password=123456";
Class.forName("com.mysql.jdbc.Driver").newInstance();
int i=1;
con=DriverManager.getConnection(url);
stmt=con.createStatement();
rst=stmt.executeQuery("select * from website_table");
while(rst.next()){
out.println(rst.getInt("accountNum")+" ");
out.println(rst.getString(1));
}
rst.close();
stmt.close();
con.close();
}catch(Exception e){
out.println(e.getMessage());
}
%>
</body>
</html>
