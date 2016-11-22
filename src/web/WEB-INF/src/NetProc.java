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
        while(true){
           byte[] recMsg = new byte[1024]; 
           try{
           
               mInSt.read(recMsg);
           }catch(Exception e){

                System.out.println("read error");
           }
           String msg = new String(recMsg);
           this.mProc.method(msg); 
        }
    }
    public void SendMsg(String msg){
        try{

            mOutSt.write(msg.getBytes());
        }catch (Exception e) {
            
            System.out.println("read error");
        }
    }

    public void Destory(){
        try{

            mSock.close();
        }catch (Exception e) {

            System.out.println("read error");
        }
    }
}
