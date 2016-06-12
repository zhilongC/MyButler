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
</center>
    <table class=siteListTable>
    <tr> 
        <th align=left> <input type="checkbox" name="selectAll" /> </th>
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
        <td align=left> <input type="checkbox" name="<%=info.getSiteName() %>" /> </td>
        <td> <%=info.getSiteName() %> </td>
        <td> <%=info.getSiteAccount() %> </td>
        <td> <input class="btn" type= "button" data-clipboard-action="copy" data-clipboard-text="<%=info.getPassword() %>" value = "复制密码" /> </td>
    </tr>
<%
    }
%>
<script src="js/clipboard.min.js"></script>

<script>
var clipboard = new Clipboard('.btn');

clipboard.on('success', function(e) {
        console.log(e);
        });

clipboard.on('error', function(e) {
        console.log(e);
        });
</script>

    </table>
    <br>
    <input type="button" value="添加">
    <input type="button" value="编辑">
    <input type="button" value="删除">
    <input type="button" value="导入">
    <input type="button" value="导出">
</body>

</html>

