package processing.app;

import java.io.*;
import java.net.*;
import java.util.*;
import java.util.zip.*;

//import javax.swing.SwingUtilities;

/**
 * This code is placed here in anticipation of running the reference from an
 * internal web server that reads the docs from a zip file, instead of using
 * thousands of .html files on the disk, which is really inefficient.
 * <p/>
 * This is a very simple, multi-threaded HTTP server, originally based on 
 * <a href="http://j.mp/6BQwpI">this</a> article on java.sun.com.
 */
public class WebServer implements HttpConstants {

    /* Where worker threads stand idle */
    static Vector threads = new Vector();

    /* the web server's virtual root */
    //static File root;

    /* timeout on client connections */
    static int timeout = 10000;

    /* max # worker threads */
    static int workers = 5;

//    static PrintStream log = System.out; 


    /*
    static void loadProps() throws IOException {
        File f = new File
                (System.getProperty("java.home")+File.separator+
                    "lib"+File.separator+"www-server.properties");
        if (f.exists()) {
            InputStream is =new BufferedInputStream(new
                           FileInputStream(f));
            props.load(is);
            is.close();
            String r = props.getProperty("root");
            if (r != null) {
                root = new File(r);
                if (!root.exists()) {
                    throw new Error(root + " doesn't exist as server root");
                }
            }
            r = props.getProperty("timeout");
            if (r != null) {
                timeout = Integer.parseInt(r);
            }
            r = props.getProperty("workers");
            if (r != null) {
                workers = Integer.parseInt(r);
            }
            r = props.getProperty("log");
            if (r != null) {
                p("opening log file: " + r);
                log = new PrintStream(new BufferedOutputStream(
                                      new FileOutputStream(r)));
            }
        }

        // if no properties were specified, choose defaults
        if (root == null) {
            root = new File(System.getProperty("user.dir"));
        }
        if (timeout <= 1000) {
            timeout = 5000;
        }
        if (workers < 25) {
            workers = 5;
        }
        if (log == null) {
            p("logging to stdout");
            log = System.out;
        }
    }

    static void printProps() {
        p("root="+root);
        p("timeout="+timeout);
        p("workers="+workers);
    }
    */

    
    /* print to stdout */
//    protected static void p(String s) {
//        System.out.println(s);
//    }

    /* print to the log file */
    protected static void log(String s) {
      if (false) {
        System.out.println(s);
      }
//        synchronized (log) {
//            log.println(s);
//            log.flush();
//        }
    }

    
    //public static void main(String[] a) throws Exception {
    static public int launch(String zipPath) throws IOException {
      final ZipFile zip = new ZipFile(zipPath);
      final HashMap<String, ZipEntry> entries = new HashMap();
      Enumeration en = zip.entries();
      while (en.hasMoreElements()) {
        ZipEntry entry = (ZipEntry) en.nextElement();
        entries.put(entry.getName(), entry);
      }
      
//        if (a.length > 0) {
//            port = Integer.parseInt(a[0]);
//        }
//        loadProps();
//        printProps();
        // start worker threads
        for (int i = 0; i < workers; ++i) {
            WebServerWorker w = new WebServerWorker(zip, entries);
            Thread t = new Thread(w, "Web Server Worker #" + i);
            t.start();
            threads.addElement(w);
        }

        final int port = 8080;

        //SwingUtilities.invokeLater(new Runnable() {
        Runnable r = new Runnable() {
          public void run() {
            try {
              ServerSocket ss = new ServerSocket(port);
              while (true) {
                Socket s = ss.accept();
                WebServerWorker w = null;
                synchronized (threads) {
                  if (threads.isEmpty()) {
                    WebServerWorker ws = new WebServerWorker(zip, entries);
                    ws.setSocket(s);
                    (new Thread(ws, "additional worker")).start();
                  } else {
                    w = (WebServerWorker) threads.elementAt(0);
                    threads.removeElementAt(0);
                    w.setSocket(s);
                  }
                }
              }
            } catch (IOException e) {
              e.printStackTrace();
            }
          }
        };
        new Thread(r).start();
//        });
        return port;
    }
}


class WebServerWorker /*extends WebServer*/ implements HttpConstants, Runnable {
  ZipFile zip;
  HashMap<String, ZipEntry> entries;
  
    final static int BUF_SIZE = 2048;

    static final byte[] EOL = { (byte)'\r', (byte)'\n' };

    /* buffer to use for requests */
    byte[] buf;
    /* Socket to client we're handling */
    private Socket s;

    WebServerWorker(ZipFile zip, HashMap entries) {
      this.entries = entries;
      this.zip = zip;
      
      buf = new byte[BUF_SIZE];
      s = null;
  }

//    Worker() {
//        buf = new byte[BUF_SIZE];
//        s = null;
//    }
//
    synchronized void setSocket(Socket s) {
        this.s = s;
        notify();
    }

    public synchronized void run() {
        while(true) {
            if (s == null) {
                /* nothing to do */
                try {
                    wait();
                } catch (InterruptedException e) {
                    /* should not happen */
                    continue;
                }
            }
            try {
                handleClient();
            } catch (Exception e) {
                e.printStackTrace();
            }
            /* go back in wait queue if there's fewer
             * than numHandler connections.
             */
            s = null;
            Vector pool = WebServer.threads;
            synchronized (pool) {
                if (pool.size() >= WebServer.workers) {
                    /* too many threads, exit this one */
                    return;
                } else {
                    pool.addElement(this);
                }
            }
        }
    }

    
    void handleClient() throws IOException {
        InputStream is = new BufferedInputStream(s.getInputStream());
        PrintStream ps = new PrintStream(s.getOutputStream());
        // we will only block in read for this many milliseconds
        // before we fail with java.io.InterruptedIOException,
        // at which point we will abandon the connection.
        s.setSoTimeout(WebServer.timeout);
        s.setTcpNoDelay(true);
        // zero out the buffer from last time
        for (int i = 0; i < BUF_SIZE; i++) {
            buf[i] = 0;
        }
        try {
          // We only support HTTP GET/HEAD, and don't support any fancy HTTP 
          // options, so we're only interested really in the first line.
            int nread = 0, r = 0;

outerloop:
            while (nread < BUF_SIZE) {
                r = is.read(buf, nread, BUF_SIZE - nread);
                if (r == -1) {
                    return;  // EOF
                }
                int i = nread;
                nread += r;
                for (; i < nread; i++) {
                    if (buf[i] == (byte)'\n' || buf[i] == (byte)'\r') {
                        break outerloop;  // read one line 
                    }
                }
            }

            /* are we doing a GET or just a HEAD */
            boolean doingGet;
            /* beginning of file name */
            int index;
            if (buf[0] == (byte)'G' &&
                buf[1] == (byte)'E' &&
                buf[2] == (byte)'T' &&
                buf[3] == (byte)' ') {
                doingGet = true;
                index = 4;
            } else if (buf[0] == (byte)'H' &&
                       buf[1] == (byte)'E' &&
                       buf[2] == (byte)'A' &&
                       buf[3] == (byte)'D' &&
                       buf[4] == (byte)' ') {
                doingGet = false;
                index = 5;
            } else {
                /* we don't support this method */
                ps.print("HTTP/1.0 " + HTTP_BAD_METHOD +
                           " unsupported method type: ");
                ps.write(buf, 0, 5);
                ps.write(EOL);
                ps.flush();
                s.close();
                return;
            }

            int i = 0;
            /* find the file name, from:
             * GET /foo/bar.html HTTP/1.0
             * extract "/foo/bar.html"
             */
            for (i = index; i < nread; i++) {
                if (buf[i] == (byte)' ') {
                    break;
                }
            }

            String fname = new String(buf, index, i-index);
            // get the zip entry, remove the front slash
            ZipEntry entry = entries.get(fname.substring(1));
            //System.out.println(fname + " " + entry);
            boolean ok = printHeaders(entry, ps);
            if (entry != null) {
              InputStream stream = zip.getInputStream(entry);
              if (doingGet && ok) {
                sendFile(stream, ps);
              }
            } else {
              send404(ps);
            }
            /*
            String fname = 
              (new String(buf, 0, index, i-index)).replace('/', File.separatorChar);
            if (fname.startsWith(File.separator)) {
                fname = fname.substring(1);
            }
            File targ = new File(WebServer.root, fname);
            if (targ.isDirectory()) {
                File ind = new File(targ, "index.html");
                if (ind.exists()) {
                    targ = ind;
                }
            }
            boolean OK = printHeaders(targ, ps);
            if (doingGet) {
                if (OK) {
                    sendFile(targ, ps);
                } else {
                    send404(targ, ps);
                }
            }
            */
        } finally {
            s.close();
        }
    }

    
    boolean printHeaders(ZipEntry targ, PrintStream ps) throws IOException {
      boolean ret = false;
      int rCode = 0;
      if (targ == null) {
          rCode = HTTP_NOT_FOUND;
          ps.print("HTTP/1.0 " + HTTP_NOT_FOUND + " Not Found");
          ps.write(EOL);
          ret = false;
      }  else {
          rCode = HTTP_OK;
          ps.print("HTTP/1.0 " + HTTP_OK + " OK");
          ps.write(EOL);
          ret = true;
      }
      if (targ != null) {
        WebServer.log("From " +s.getInetAddress().getHostAddress()+": GET " + targ.getName()+" --> "+rCode);
      }
      ps.print("Server: Processing Documentation Server");
      ps.write(EOL);
      ps.print("Date: " + (new Date()));
      ps.write(EOL);
      if (ret) {
          if (!targ.isDirectory()) {
              ps.print("Content-length: " + targ.getSize());
              ps.write(EOL);
              ps.print("Last Modified: " + new Date(targ.getTime()));
              ps.write(EOL);
              String name = targ.getName();
              int ind = name.lastIndexOf('.');
              String ct = null;
              if (ind > 0) {
                  ct = (String) map.get(name.substring(ind));
              }
              if (ct == null) {
                //System.err.println("unknown content type " + name.substring(ind));
                  ct = "application/x-unknown-content-type";
              }
              ps.print("Content-type: " + ct);
              ps.write(EOL);
          } else {
              ps.print("Content-type: text/html");
              ps.write(EOL);
          }
      }
      ps.write(EOL);  // adding another newline here [fry]
      return ret;
  }
    
    
    boolean printHeaders(File targ, PrintStream ps) throws IOException {
        boolean ret = false;
        int rCode = 0;
        if (!targ.exists()) {
            rCode = HTTP_NOT_FOUND;
            ps.print("HTTP/1.0 " + HTTP_NOT_FOUND + " Not Found");
            ps.write(EOL);
            ret = false;
        }  else {
            rCode = HTTP_OK;
            ps.print("HTTP/1.0 " + HTTP_OK+" OK");
            ps.write(EOL);
            ret = true;
        }
        WebServer.log("From " +s.getInetAddress().getHostAddress()+": GET " + targ.getAbsolutePath()+"-->"+rCode);
        ps.print("Server: Simple java");
        ps.write(EOL);
        ps.print("Date: " + (new Date()));
        ps.write(EOL);
        if (ret) {
            if (!targ.isDirectory()) {
                ps.print("Content-length: " + targ.length());
                ps.write(EOL);
                ps.print("Last Modified: " + new Date(targ.lastModified()));
                ps.write(EOL);
                String name = targ.getName();
                int ind = name.lastIndexOf('.');
                String ct = null;
                if (ind > 0) {
                    ct = (String) map.get(name.substring(ind));
                }
                if (ct == null) {
                    ct = "unknown/unknown";
                }
                ps.print("Content-type: " + ct);
                ps.write(EOL);
            } else {
                ps.print("Content-type: text/html");
                ps.write(EOL);
            }
        }
        return ret;
    }

    
    void send404(PrintStream ps) throws IOException {
        ps.write(EOL);
        ps.write(EOL);
        ps.print("<html><body><h1>404 Not Found</h1>"+
                   "The requested resource was not found.</body></html>");
        ps.write(EOL);
        ps.write(EOL);
    }

    
    void sendFile(File targ, PrintStream ps) throws IOException {
        InputStream is = null;
        ps.write(EOL);
        if (targ.isDirectory()) {
            listDirectory(targ, ps);
            return;
        } else {
            is = new FileInputStream(targ.getAbsolutePath());
        }
        sendFile(is, ps);
    }
    
    
    void sendFile(InputStream is, PrintStream ps) throws IOException {
        try {
            int n;
            while ((n = is.read(buf)) > 0) {
                ps.write(buf, 0, n);
            }
        } finally {
            is.close();
        }
    }

    /* mapping of file extensions to content-types */
    static java.util.Hashtable map = new java.util.Hashtable();

    static {
        fillMap();
    }
    static void setSuffix(String k, String v) {
        map.put(k, v);
    }

    static void fillMap() {
        setSuffix("", "content/unknown");

        setSuffix(".uu", "application/octet-stream");
        setSuffix(".exe", "application/octet-stream");
        setSuffix(".ps", "application/postscript");
        setSuffix(".zip", "application/zip");
        setSuffix(".sh", "application/x-shar");
        setSuffix(".tar", "application/x-tar");
        setSuffix(".snd", "audio/basic");
        setSuffix(".au", "audio/basic");
        setSuffix(".wav", "audio/x-wav");
        
        setSuffix(".gif", "image/gif");
        setSuffix(".jpg", "image/jpeg");
        setSuffix(".jpeg", "image/jpeg");
        
        setSuffix(".htm", "text/html");
        setSuffix(".html", "text/html");
        setSuffix(".css", "text/css"); 
        setSuffix(".java", "text/javascript");
        
        setSuffix(".txt", "text/plain");
        setSuffix(".java", "text/plain");
        
        setSuffix(".c", "text/plain");
        setSuffix(".cc", "text/plain");
        setSuffix(".c++", "text/plain");
        setSuffix(".h", "text/plain");
        setSuffix(".pl", "text/plain");
    }

    void listDirectory(File dir, PrintStream ps) throws IOException {
        ps.println("<TITLE>Directory listing</TITLE><P>\n");
        ps.println("<A HREF=\"..\">Parent Directory</A><BR>\n");
        String[] list = dir.list();
        for (int i = 0; list != null && i < list.length; i++) {
            File f = new File(dir, list[i]);
            if (f.isDirectory()) {
                ps.println("<A HREF=\""+list[i]+"/\">"+list[i]+"/</A><BR>");
            } else {
                ps.println("<A HREF=\""+list[i]+"\">"+list[i]+"</A><BR");
            }
        }
        ps.println("<P><HR><BR><I>" + (new Date()) + "</I>");
    }

}


interface HttpConstants {
    /** 2XX: generally "OK" */
    public static final int HTTP_OK = 200;
    public static final int HTTP_CREATED = 201;
    public static final int HTTP_ACCEPTED = 202;
    public static final int HTTP_NOT_AUTHORITATIVE = 203;
    public static final int HTTP_NO_CONTENT = 204;
    public static final int HTTP_RESET = 205;
    public static final int HTTP_PARTIAL = 206;

    /** 3XX: relocation/redirect */
    public static final int HTTP_MULT_CHOICE = 300;
    public static final int HTTP_MOVED_PERM = 301;
    public static final int HTTP_MOVED_TEMP = 302;
    public static final int HTTP_SEE_OTHER = 303;
    public static final int HTTP_NOT_MODIFIED = 304;
    public static final int HTTP_USE_PROXY = 305;

    /** 4XX: client error */
    public static final int HTTP_BAD_REQUEST = 400;
    public static final int HTTP_UNAUTHORIZED = 401;
    public static final int HTTP_PAYMENT_REQUIRED = 402;
    public static final int HTTP_FORBIDDEN = 403;
    public static final int HTTP_NOT_FOUND = 404;
    public static final int HTTP_BAD_METHOD = 405;
    public static final int HTTP_NOT_ACCEPTABLE = 406;
    public static final int HTTP_PROXY_AUTH = 407;
    public static final int HTTP_CLIENT_TIMEOUT = 408;
    public static final int HTTP_CONFLICT = 409;
    public static final int HTTP_GONE = 410;
    public static final int HTTP_LENGTH_REQUIRED = 411;
    public static final int HTTP_PRECON_FAILED = 412;
    public static final int HTTP_ENTITY_TOO_LARGE = 413;
    public static final int HTTP_REQ_TOO_LONG = 414;
    public static final int HTTP_UNSUPPORTED_TYPE = 415;

    /** 5XX: server error */
    public static final int HTTP_SERVER_ERROR = 500;
    public static final int HTTP_INTERNAL_ERROR = 501;
    public static final int HTTP_BAD_GATEWAY = 502;
    public static final int HTTP_UNAVAILABLE = 503;
    public static final int HTTP_GATEWAY_TIMEOUT = 504;
    public static final int HTTP_VERSION = 505;
}
