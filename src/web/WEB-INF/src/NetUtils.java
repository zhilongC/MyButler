//package com.zhilongc.utils;

public class NetUtils{
    public static byte[] toBytes(int value){
        byte[] arr = new byte[4];

        arr[0] = (byte) (value >>> 24 & 0xff);
        arr[1] = (byte) (value >> 16 & 0xff);
        arr[2] = (byte) (value >> 8 & 0xff);
        arr[3] = (byte) (value & 0xff);

        return arr;
    }
    public static byte[] toBytes(short value){
        byte[] arr = new byte[2];

        arr[0] = (byte) (value >> 8 & 0xff);
        arr[1] = (byte) (value & 0xff);

        return arr;
    }
    public static byte[] byteMerger(byte[] byte_front, byte[] byte_back){
        byte[] result = new byte[byte_front.length+byte_back.length];
        System.arraycopy(byte_front, 0, result, 0, byte_front.length);
        System.arraycopy(byte_back, 0, result, byte_front.length, byte_back.length);
        return result;
    }
}
