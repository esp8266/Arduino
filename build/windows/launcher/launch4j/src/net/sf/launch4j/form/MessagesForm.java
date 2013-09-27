package net.sf.launch4j.form;

import com.jgoodies.forms.layout.CellConstraints;
import com.jgoodies.forms.layout.FormLayout;
import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.Dimension;
import javax.swing.Box;
import javax.swing.ImageIcon;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;

public abstract class MessagesForm extends JPanel
{
   protected final JTextArea _startupErrTextArea = new JTextArea();
   protected final JTextArea _bundledJreErrTextArea = new JTextArea();
   protected final JTextArea _jreVersionErrTextArea = new JTextArea();
   protected final JTextArea _launcherErrTextArea = new JTextArea();
   protected final JCheckBox _messagesCheck = new JCheckBox();
   protected final JTextArea _instanceAlreadyExistsMsgTextArea = new JTextArea();

   /**
    * Default constructor
    */
   public MessagesForm()
   {
      initializePanel();
   }

   /**
    * Adds fill components to empty cells in the first row and first column of the grid.
    * This ensures that the grid spacing will be the same as shown in the designer.
    * @param cols an array of column indices in the first row where fill components should be added.
    * @param rows an array of row indices in the first column where fill components should be added.
    */
   void addFillComponents( Container panel, int[] cols, int[] rows )
   {
      Dimension filler = new Dimension(10,10);

      boolean filled_cell_11 = false;
      CellConstraints cc = new CellConstraints();
      if ( cols.length > 0 && rows.length > 0 )
      {
         if ( cols[0] == 1 && rows[0] == 1 )
         {
            /** add a rigid area  */
            panel.add( Box.createRigidArea( filler ), cc.xy(1,1) );
            filled_cell_11 = true;
         }
      }

      for( int index = 0; index < cols.length; index++ )
      {
         if ( cols[index] == 1 && filled_cell_11 )
         {
            continue;
         }
         panel.add( Box.createRigidArea( filler ), cc.xy(cols[index],1) );
      }

      for( int index = 0; index < rows.length; index++ )
      {
         if ( rows[index] == 1 && filled_cell_11 )
         {
            continue;
         }
         panel.add( Box.createRigidArea( filler ), cc.xy(1,rows[index]) );
      }

   }

   /**
    * Helper method to load an image file from the CLASSPATH
    * @param imageName the package and name of the file to load relative to the CLASSPATH
    * @return an ImageIcon instance with the specified image file
    * @throws IllegalArgumentException if the image resource cannot be loaded.
    */
   public ImageIcon loadImage( String imageName )
   {
      try
      {
         ClassLoader classloader = getClass().getClassLoader();
         java.net.URL url = classloader.getResource( imageName );
         if ( url != null )
         {
            ImageIcon icon = new ImageIcon( url );
            return icon;
         }
      }
      catch( Exception e )
      {
         e.printStackTrace();
      }
      throw new IllegalArgumentException( "Unable to load image: " + imageName );
   }

   public JPanel createPanel()
   {
      JPanel jpanel1 = new JPanel();
      FormLayout formlayout1 = new FormLayout("FILL:7DLU:NONE,RIGHT:MAX(65DLU;DEFAULT):NONE,FILL:3DLU:NONE,FILL:DEFAULT:GROW(1.0),FILL:7DLU:NONE","CENTER:9DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,FILL:DEFAULT:GROW(1.0),CENTER:3DLU:NONE,FILL:DEFAULT:GROW(1.0),CENTER:3DLU:NONE,FILL:DEFAULT:GROW(1.0),CENTER:3DLU:NONE,FILL:DEFAULT:GROW(1.0),CENTER:3DLU:NONE,FILL:DEFAULT:GROW(1.0),CENTER:9DLU:NONE");
      CellConstraints cc = new CellConstraints();
      jpanel1.setLayout(formlayout1);

      _startupErrTextArea.setName("startupErrTextArea");
      JScrollPane jscrollpane1 = new JScrollPane();
      jscrollpane1.setViewportView(_startupErrTextArea);
      jscrollpane1.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
      jscrollpane1.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
      jpanel1.add(jscrollpane1,cc.xy(4,4));

      _bundledJreErrTextArea.setName("bundledJreErrTextArea");
      JScrollPane jscrollpane2 = new JScrollPane();
      jscrollpane2.setViewportView(_bundledJreErrTextArea);
      jscrollpane2.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
      jscrollpane2.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
      jpanel1.add(jscrollpane2,cc.xy(4,6));

      _jreVersionErrTextArea.setName("jreVersionErrTextArea");
      _jreVersionErrTextArea.setToolTipText(Messages.getString("jreVersionErrTip"));
      JScrollPane jscrollpane3 = new JScrollPane();
      jscrollpane3.setViewportView(_jreVersionErrTextArea);
      jscrollpane3.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
      jscrollpane3.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
      jpanel1.add(jscrollpane3,cc.xy(4,8));

      _launcherErrTextArea.setName("launcherErrTextArea");
      JScrollPane jscrollpane4 = new JScrollPane();
      jscrollpane4.setViewportView(_launcherErrTextArea);
      jscrollpane4.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
      jscrollpane4.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
      jpanel1.add(jscrollpane4,cc.xy(4,10));

      JLabel jlabel1 = new JLabel();
      jlabel1.setText(Messages.getString("startupErr"));
      jpanel1.add(jlabel1,new CellConstraints(2,4,1,1,CellConstraints.DEFAULT,CellConstraints.TOP));

      JLabel jlabel2 = new JLabel();
      jlabel2.setText(Messages.getString("bundledJreErr"));
      jpanel1.add(jlabel2,new CellConstraints(2,6,1,1,CellConstraints.DEFAULT,CellConstraints.TOP));

      JLabel jlabel3 = new JLabel();
      jlabel3.setText(Messages.getString("jreVersionErr"));
      jpanel1.add(jlabel3,new CellConstraints(2,8,1,1,CellConstraints.DEFAULT,CellConstraints.TOP));

      JLabel jlabel4 = new JLabel();
      jlabel4.setText(Messages.getString("launcherErr"));
      jpanel1.add(jlabel4,new CellConstraints(2,10,1,1,CellConstraints.DEFAULT,CellConstraints.TOP));

      _messagesCheck.setActionCommand("Add version information");
      _messagesCheck.setName("messagesCheck");
      _messagesCheck.setText(Messages.getString("addMessages"));
      jpanel1.add(_messagesCheck,cc.xy(4,2));

      JLabel jlabel5 = new JLabel();
      jlabel5.setText(Messages.getString("instanceAlreadyExistsMsg"));
      jpanel1.add(jlabel5,new CellConstraints(2,12,1,1,CellConstraints.DEFAULT,CellConstraints.TOP));

      _instanceAlreadyExistsMsgTextArea.setName("instanceAlreadyExistsMsgTextArea");
      _instanceAlreadyExistsMsgTextArea.setToolTipText(Messages.getString("instanceAlreadyExistsMsgTip"));
      JScrollPane jscrollpane5 = new JScrollPane();
      jscrollpane5.setViewportView(_instanceAlreadyExistsMsgTextArea);
      jscrollpane5.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
      jscrollpane5.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
      jpanel1.add(jscrollpane5,cc.xy(4,12));

      addFillComponents(jpanel1,new int[]{ 1,2,3,4,5 },new int[]{ 1,2,3,4,5,6,7,8,9,10,11,12,13 });
      return jpanel1;
   }

   /**
    * Initializer
    */
   protected void initializePanel()
   {
      setLayout(new BorderLayout());
      add(createPanel(), BorderLayout.CENTER);
   }


}
