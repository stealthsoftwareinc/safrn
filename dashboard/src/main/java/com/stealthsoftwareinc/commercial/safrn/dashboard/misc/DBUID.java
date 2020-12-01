package com.stealthsoftwareinc.commercial.safrn.dashboard.misc;

import org.springframework.security.crypto.codec.Hex;

import lombok.Getter;
import lombok.extern.slf4j.Slf4j;

@Slf4j
@Getter
public class DBUID { 
  private byte[] bytes;
  private int position;
  
  private DBUID() {
  
  }
  
  public DBUID(int size) {
    this();
    bytes = new byte[size]; //defaults to zero initial values
    position = size-1; //track from LSB to MSB in array
  }
  
  public byte[] plusOne()  throws Exception { 
    return this.plusOne(position);
  }
  
  public byte[] plusOne(int index)  throws Exception { 
    if(index < 0) {
      log.error("overflow");
      throw new Exception ("DBUID overflow.."+this.toString());
    }

    if((bytes[index]  & 0xff)  == 255){ 
      log.info("byte boundary reached at position "+index+"..current value is .."+this.toString());
      bytes[index]=0;
      this.plusOne(index-1);
    }
    else {
      bytes[index]++;
    }
    return bytes;
  }
  
  @Override
  public String toString() {
    char[] idInString = Hex.encode(bytes);
    return String.copyValueOf(idInString);
  } 
  
  public static void main(String[] args) throws Exception{
    int nBytes = 3;
    int upper = (int) Math.pow(2, 8);
    upper = (int) Math.pow(upper, nBytes);
    DBUID dbuid = new DBUID(nBytes);
    for (int i = 1; i < upper+1; i++) {
      byte[] bytes = dbuid.plusOne(dbuid.getPosition());   
      System.out.print(i+":"+Utils.convertToHex(bytes)+", "); 
      if(i%5 == 0)
        System.out.println();
    }
    
  }
}
