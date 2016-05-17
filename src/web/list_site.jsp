<%@ page language="java" contentType="text/html; charset=UTF-8" pageEncoding="UTF-8"%>
<%@ page import="java.util.*,java.sql.*,com.zhilongc.jsp.*" %>

<!DOCTYPE HTML> 

<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title> 从数据库查找信息并显示 </title>
<link href="css/table.css" rel="stylesheet" type="text/css" />
</head>

<body>
<center>
    <table class=siteListTable>
    <tr> 
        <th align=left> 名称 </th>
        <th align=left> 账号 </th>
        <th align=left> 密码 </th>
    </tr>
<%
    DBManager dbm = new DBManager();

    List list = dbm.queryAll();   

    for(int i=0; i<list.size(); i++){
        WebsiteInfo info = (WebsiteInfo)list.get(i);
%>
    <tr>
        <td> <%=info.getSiteName() %> </td>
        <td> <%=info.getSiteAccount() %> </td>
        <td> <%=info.getPassword() %> </td>
    </tr>
<%
    }
%>
    </table>
</center>
</body>

</html>

