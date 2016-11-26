import java.io.*;
import java.lang.String;
import java.net.Socket;
import java.net.SocketException;

interface MsgProcess{
    public void method(String json);
}
public class NetProc extends Thread{
    private Socket mSock;
    private OutputStream mOutSt;
    private InputStream mInSt;
    private MsgProcess mProc;
    private static NetUtils mNetUtils;
    private static boolen isFirstPkg = true;
    private static int mRecLength = 0;

    public void setMsgCallBack(MsgProcess proc){
       this.mProc = proc; 
    }
    public NetProc(String name){
        super(name);

        try{

            mSock = new Socket("127.0.0.1", 8888) ;
            mInSt = mSock.getInputStream();
            mOutSt = mSock.getOutputStream();
        }catch(Exception e){

            System.out.println("create sock error");
        }
    }

    public void run(){
        int recLength = 0;
        while(true){
           byte[] recLen = new byte[4]; 
           try{

               if(isFirstPkg){
                   mInSt.read(recLen);
                   mRecLength = mNetUtils.toInt32(recLen); 
               } else {
                   
                     
                   mRecLength -= mInSt.read(mRecLength);
               }
               byte[] buf = new byte[recLength];


           }catch(Exception e){

                System.out.println("read error");
                continue;
           }
           String msg = new String(recMsg);

           this.mProc.method(msg); 
        }
    }
    public void sendMsg(String msg){
        try{
            byte[] strLen = mNetUtils.toBytes(msg.getBytes().length);
            byte[] strArr = msg.getBytes();
            byte[] sendMsg = mNetUtils.byteMerger(strLen, strArr);
            mOutSt.write(sendMsg);
        }catch (Exception e) {
            
            System.out.println("read error");
        }
    }

    public void destory(){
        try{

            mSock.close();
        }catch (Exception e) {

            System.out.println("read error");
        }
    }
}
