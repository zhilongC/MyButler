<%@ page language="java" import="java.lang.*, java.io.*"%>
<!DOCTYPE HTML>
<html>
<head>
<title> count account number </title>
<body>
<%
String record = null;
BufferedReader br=null;
String FilePath="/home/bruce/works/a.txt";
br = new BufferedReader(new FileReader(FilePath));
record = br.readLine();

if(record == null)
{
    record = "1";
}
else
{
    record = (Integer.parseInt(record) + 1) + "";
}
%>
welcome to our site, you're the <%=record %> inventer.
<%
PrintWriter pw=null;
pw = new PrintWriter(new FileOutputStream(FilePath));
pw.write(record);
pw.close();
%>
</body>
</html>
