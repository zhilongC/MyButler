import java.io.*;
import java.lang.String;
import java.net.Socket;
import java.net.SocketException;
interface MsgProcess {
    public void method(String json);
}
public class NetProc extends Thread {
    private Socket mSock;
    private OutputStream mOutSt;
    private InputStream mInSt;
    private MsgProcess mProc;
    private static NetUtils mNetUtils;
    public void setMsgCallBack(MsgProcess proc) {
        this.mProc = proc;
    }
    public NetProc(String name) {
        super(name);
        try {
            mSock = new Socket("127.0.0.1", 8888) ;
            mInSt = mSock.getInputStream();
            mOutSt = mSock.getOutputStream();
        } catch(Exception e) {
            System.out.println("create sock error");
        }
    }
    public void run() {
        while(true) {
            int recLength = 0;
            int recLenTmp = 0;
            int offset = 0;
            byte[] recLen = new byte[4];
            byte[] recBuf;
            byte[] recBufTmp;
            try {
                if(mInSt.read(recLen) <= 0) {
                    sleep(500);
                    continue;
                }
                recLength = mNetUtils.toInt32(recLen);
                recBuf = new byte[recLength];
                recBufTmp = new byte[1024];
                while(recLength > 0) {
                    recLenTmp = mInSt.read(recBufTmp);
                    System.arraycopy(recBufTmp, 0, recBuf, offset, recLenTmp);
                    offset += recLenTmp;
                    recLength -= recLenTmp;
                }
            } catch(Exception e) {
                System.out.println("read error");
                continue;
            }
            String msg = new String(recBuf);
            this.mProc.method(msg);
        }
    }
    public void sendMsg(String msg) {
        try {
            byte[] strLen = mNetUtils.toBytes(msg.getBytes().length);
            byte[] strArr = msg.getBytes();
            byte[] sendMsg = mNetUtils.byteMerger(strLen, strArr);
            mOutSt.write(sendMsg);
        } catch (Exception e) {
            System.out.println("read error");
        }
    }
    public void destory() {
        try {
            mSock.close();
        } catch (Exception e) {
            System.out.println("read error");
        }
    }
}
