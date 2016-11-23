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
    //java 合并两个byte数组
    public static byte[] byteMerger(byte[] byte_front, byte[] byte_back){
        byte[] result = new byte[byte_front.length+byte_back.length];
        System.arraycopy(byte_front, 0, result, 0, byte_front.length);
        System.arraycopy(byte_back, 0, result, byte_front.length, byte_back.length);
        return result;
    }
    public static byte[] int2byte(int v){
        byte[] arr = new byte[4];
        arr[0] = (byte)(v >>> 24 & 0xff);
        arr[1] = (byte)(v >> 16 & 0xff);
        arr[2] = (byte)(v >> 8 & 0xff);
        arr[3] = (byte)(v & 0xff);
        return arr;
    }
    public void sendMsg(String msg){
        try{
            byte[] strLen = int2byte(msg.getBytes().length);
            byte[] strArr = msg.getBytes();
            byte[] sendMsg = byteMerger(strLen, strArr);
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
