/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Tool to put the contents of the sketch's "data" subfolder
  into an SPIFFS partition image and upload it to an ESP8266 MCU

  Copyright (c) 2015 Hristo Gochkov (ficeto at ficeto dot com)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

package com.esp8266.mkspiffs;

import java.io.File;
import java.io.BufferedReader;
import java.io.InputStreamReader;

import java.text.SimpleDateFormat;
import java.util.Date;

import javax.swing.JOptionPane;

import processing.app.PreferencesData;
import processing.app.Editor;
import processing.app.Base;
import processing.app.Platform;
import processing.app.Sketch;
import processing.app.tools.Tool;
import processing.app.helpers.ProcessUtils;
import processing.app.debug.TargetPlatform;


/**
 * Example Tools menu entry.
 */
public class ESP8266FS implements Tool {
  Editor editor;


  public void init(Editor editor) {
    this.editor = editor;
  }


  public String getMenuTitle() {
    return "ESP8266 Sketch Data Upload";
  }
  
  private int listenOnProcess(String[] arguments){
    try {
      final Process p = ProcessUtils.exec(arguments);
      Thread thread = new Thread() {
        public void run() {
          try {
            String line;
            BufferedReader input = new BufferedReader (new InputStreamReader(p.getInputStream()));
            while ((line = input.readLine()) != null) System.out.println(line);
            input.close();
          } catch (Exception e){}
        }
      };
      thread.start();
      int res = p.waitFor();
      thread.join();
      return res;
    } catch (Exception e){
      return -1;
    }
  }
  
  private void sysExec(final String[] arguments){
    Thread thread = new Thread() {
      public void run() {
        try {
          if(listenOnProcess(arguments) != 0){
            editor.statusError("SPIFFS Upload failed!");
          } else {
            editor.statusNotice("SPIFFS Image Uploaded");
          }
        } catch (Exception e){
          editor.statusError("SPIFFS Upload failed!");
        }
      }
    };
    thread.start();
  }
  
  
  private long getIntPref(String name){
    String data = Base.getBoardPreferences().get(name);
    if(data == null || data.contentEquals("")) return 0;
    if(data.startsWith("0x")) return Long.parseLong(data.substring(2), 16);
    else return Integer.parseInt(data);
  }
  
  private void createAndUpload(){
    if(!PreferencesData.get("target_platform").contentEquals("esp8266")){
      System.err.println();
      editor.statusError("SPIFFS Not Supported on "+PreferencesData.get("target_platform"));
      return;
    }
    
    if(!Base.getBoardPreferences().containsKey("build.spiffs_start") || !Base.getBoardPreferences().containsKey("build.spiffs_end")){
      System.err.println();
      editor.statusError("SPIFFS Not Defined for "+Base.getBoardPreferences().get("name"));
      return;
    }
    long spiStart, spiEnd, spiPage, spiBlock;
    try {
      spiStart = getIntPref("build.spiffs_start");
      spiEnd = getIntPref("build.spiffs_end");
      spiPage = getIntPref("build.spiffs_pagesize");
      if(spiPage == 0) spiPage = 256;
      spiBlock = getIntPref("build.spiffs_blocksize");
      if(spiBlock == 0) spiBlock = 4096;
    } catch(Exception e){
      editor.statusError(e);
      return;
    }
    
    TargetPlatform platform = Base.getTargetPlatform();
    
    File esptool;
    if(!PreferencesData.get("runtime.os").contentEquals("windows")) esptool = new File(platform.getFolder()+"/tools", "esptool");
    else esptool = new File(platform.getFolder()+"/tools", "esptool.exe");
    if(!esptool.exists()){
      System.err.println();
      editor.statusError("SPIFFS Error: esptool not found!");
      return;
    }
    
    File tool;
    if(!PreferencesData.get("runtime.os").contentEquals("windows")) tool = new File(platform.getFolder()+"/tools", "mkspiffs");
    else tool = new File(platform.getFolder()+"/tools", "mkspiffs.exe");
    if(!tool.exists()){
      System.err.println();
      editor.statusError("SPIFFS Error: mkspiffs not found!");
      return;
    }

    int fileCount = 0;
    File dataFolder = editor.getSketch().prepareDataFolder();
    if(dataFolder.exists() && dataFolder.isDirectory()){
      File[] files = dataFolder.listFiles();
      if(files.length > 0){
        for(File file : files){
          if(!file.isDirectory() && file.isFile() && !file.getName().startsWith(".")) fileCount++;
        }
      }
    }
    
    String dataPath = dataFolder.getAbsolutePath();
    String toolPath = tool.getAbsolutePath();
    String esptoolPath = esptool.getAbsolutePath();
    String sketchName = editor.getSketch().getName();
    String buildPath = Base.getBuildFolder().getAbsolutePath();
    String imagePath = buildPath+"/"+sketchName+".spiffs.bin";
    String serialPort = PreferencesData.get("serial.port");
    String resetMethod = Base.getBoardPreferences().get("upload.resetmethod");
    String uploadSpeed = Base.getBoardPreferences().get("upload.speed");
    String uploadAddress = Base.getBoardPreferences().get("build.spiffs_start");
    
    Object[] options = { "Yes", "No" };
    String title = "SPIFFS Create";
    String message = "No files have been found in your data folder!\nAre you sure you want to create an empty SPIFFS image?";
    
    if(fileCount == 0 && JOptionPane.showOptionDialog(editor, message, title, JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, options, options[1]) != JOptionPane.YES_OPTION){
      System.err.println();
      editor.statusError("SPIFFS Warning: mkspiffs canceled!");
      return;
    }
    
    editor.statusNotice("SPIFFS Creating Image...");
    System.out.println("[SPIFFS] data   : "+dataPath);
    System.out.println("[SPIFFS] size   : "+((spiEnd - spiStart)/1024));
    System.out.println("[SPIFFS] page   : "+spiPage);
    System.out.println("[SPIFFS] block  : "+spiBlock);
    
    try {
      if(listenOnProcess(new String[]{toolPath, "-c", dataPath, "-p", spiPage+"", "-b", spiBlock+"", "-s", (spiEnd - spiStart)+"", imagePath}) != 0){
        System.err.println();
        editor.statusError("SPIFFS Create Failed!");
        return;
      }
    } catch (Exception e){
      editor.statusError(e);
      editor.statusError("SPIFFS Create Failed!");
      return;
    }
    
    editor.statusNotice("SPIFFS Uploading Image...");
    System.out.println("[SPIFFS] upload : "+imagePath);
    System.out.println("[SPIFFS] reset  : "+resetMethod);
    System.out.println("[SPIFFS] port   : "+serialPort);
    System.out.println("[SPIFFS] speed  : "+uploadSpeed);
    System.out.println("[SPIFFS] address: "+uploadAddress);
    System.out.println();
    
    sysExec(new String[]{esptoolPath, "-cd", resetMethod, "-cb", uploadSpeed, "-cp", serialPort, "-ca", uploadAddress, "-cf", imagePath});
  }
  
  public void run() {
    createAndUpload();
  }
}
