package com.zhilongc.jsp;

public class WebsiteInfo{
    private String siteName;
    private String siteAccount;
    private String password;
    public WebsiteInfo(){

    }
    public String getSiteName(){
        return siteName;
    }
    public String getSiteAccount(){
        return siteAccount;
    }
    public String getPassword(){
        return password;
    }
    public void setSiteName(String str){
        this.siteName = str;
    }
    public void setSiteAccount(String str){
        this.siteAccount = str;
    }
    public void setPassword(String str){
        this.password = str;
    }
}

