package processing.app.preproc;

import java.util.*;

public class PreprocessorInfoChannel
{
    Hashtable lineLists = new Hashtable(); // indexed by Token number
    int firstValidTokenNumber = 0;
    int maxTokenNumber = 0;

    public void addLineForTokenNumber( Object line, Integer toknum )
    {
        if ( lineLists.containsKey( toknum ) ) {
            Vector lines = (Vector) lineLists.get( toknum );
            lines.addElement(line);
        }
        else {
            Vector lines = new Vector();
            lines.addElement(line);
            lineLists.put(toknum, lines);
            if ( maxTokenNumber < toknum.intValue() ) {
                maxTokenNumber = toknum.intValue();
            }
        }
    }

    public int getMaxTokenNumber()
    {
        return maxTokenNumber;
    }
        
    public Vector extractLinesPrecedingTokenNumber( Integer toknum )
    {
        Vector lines = new Vector();
        if (toknum == null) return lines;       
        for (int i = firstValidTokenNumber; i < toknum.intValue(); i++){
            Integer inti = new Integer(i);
            if ( lineLists.containsKey( inti ) ) {
                Vector tokenLineVector = (Vector) lineLists.get( inti );
                if ( tokenLineVector != null) {
                    Enumeration tokenLines = tokenLineVector.elements();
                    while ( tokenLines.hasMoreElements() ) {
                        lines.addElement( tokenLines.nextElement() );
                    }
                    lineLists.remove(inti);
                }
            }
        }
        firstValidTokenNumber = toknum.intValue();
        return lines;
    }

    public String toString()
    {
        StringBuffer sb = new StringBuffer("PreprocessorInfoChannel:\n");
        for (int i = 0; i <= maxTokenNumber + 1; i++){
            Integer inti = new Integer(i);
            if ( lineLists.containsKey( inti ) ) {
                Vector tokenLineVector = (Vector) lineLists.get( inti );
                if ( tokenLineVector != null) {
                    Enumeration tokenLines = tokenLineVector.elements();
                    while ( tokenLines.hasMoreElements() ) {
                        sb.append(inti + ":" + tokenLines.nextElement() + '\n');
                    }
                }
            }
        }
        return sb.toString();
    }
}



