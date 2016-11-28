class MsgP implements MsgProcess{
    public void method(String json){
        System.out.println(json);
        System.out.println("comme here");
    }
    
}

public class Test{
    public static void main(String[] args){
        
        NetProc netProc = new NetProc("net");
        MsgP msgP = new MsgP();
        netProc.setMsgCallBack(msgP);
        netProc.start();
        netProc.sendMsg("{\"TYPE\":2, \"VER\":1, \"ACCOUNT\":\"HELLO\", \"PWD\":\"12345\"}");
        
    }
}
