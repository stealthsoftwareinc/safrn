package com.stealthsoftwareinc.commercial.safrn.dashboard.misc;

import java.io.InputStream;

import org.springframework.core.io.InputStreamResource;

public class StreamResource extends InputStreamResource{
  private long contentLength;
  
  public StreamResource(InputStream inputStream, long length) {
    super(inputStream);
    this.contentLength = length;
  }
  
  @Override
  public long contentLength() {
    return this.contentLength;
  }
}
