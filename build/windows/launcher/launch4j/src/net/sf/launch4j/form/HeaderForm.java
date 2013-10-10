package net.sf.launch4j.form;

import com.jeta.forms.components.separator.TitledSeparator;
import com.jgoodies.forms.layout.CellConstraints;
import com.jgoodies.forms.layout.FormLayout;
import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.Dimension;
import javax.swing.Box;
import javax.swing.ButtonGroup;
import javax.swing.ImageIcon;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;

public abstract class HeaderForm extends JPanel
{
   protected final JLabel _headerTypeLabel = new JLabel();
   protected final JRadioButton _guiHeaderRadio = new JRadioButton();
   protected final ButtonGroup _headerButtonGroup = new ButtonGroup();
   protected final JRadioButton _consoleHeaderRadio = new JRadioButton();
   protected final JTextArea _headerObjectsTextArea = new JTextArea();
   protected final JTextArea _libsTextArea = new JTextArea();
   protected final JCheckBox _headerObjectsCheck = new JCheckBox();
   protected final JCheckBox _libsCheck = new JCheckBox();
   protected final TitledSeparator _linkerOptionsSeparator = new TitledSeparator();

   /**
    * Default constructor
    */
   public HeaderForm()
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
      FormLayout formlayout1 = new FormLayout("FILL:7DLU:NONE,RIGHT:MAX(65DLU;DEFAULT):NONE,FILL:3DLU:NONE,FILL:DEFAULT:NONE,FILL:7DLU:NONE,FILL:DEFAULT:NONE,FILL:DEFAULT:GROW(1.0),FILL:7DLU:NONE","CENTER:9DLU:NONE,CENTER:DEFAULT:NONE,CENTER:9DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,FILL:DEFAULT:GROW(0.2),CENTER:3DLU:NONE,FILL:DEFAULT:GROW(1.0),CENTER:9DLU:NONE");
      CellConstraints cc = new CellConstraints();
      jpanel1.setLayout(formlayout1);

      _headerTypeLabel.setName("headerTypeLabel");
      _headerTypeLabel.setText(Messages.getString("headerType"));
      jpanel1.add(_headerTypeLabel,cc.xy(2,2));

      _guiHeaderRadio.setActionCommand("GUI");
      _guiHeaderRadio.setName("guiHeaderRadio");
      _guiHeaderRadio.setText(Messages.getString("gui"));
      _headerButtonGroup.add(_guiHeaderRadio);
      jpanel1.add(_guiHeaderRadio,cc.xy(4,2));

      _consoleHeaderRadio.setActionCommand("Console");
      _consoleHeaderRadio.setName("consoleHeaderRadio");
      _consoleHeaderRadio.setText(Messages.getString("console"));
      _headerButtonGroup.add(_consoleHeaderRadio);
      jpanel1.add(_consoleHeaderRadio,cc.xy(6,2));

      _headerObjectsTextArea.setName("headerObjectsTextArea");
      JScrollPane jscrollpane1 = new JScrollPane();
      jscrollpane1.setViewportView(_headerObjectsTextArea);
      jscrollpane1.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
      jscrollpane1.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
      jpanel1.add(jscrollpane1,cc.xywh(4,6,4,1));

      _libsTextArea.setName("libsTextArea");
      JScrollPane jscrollpane2 = new JScrollPane();
      jscrollpane2.setViewportView(_libsTextArea);
      jscrollpane2.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
      jscrollpane2.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
      jpanel1.add(jscrollpane2,cc.xywh(4,8,4,1));

      _headerObjectsCheck.setActionCommand("Object files");
      _headerObjectsCheck.setName("headerObjectsCheck");
      _headerObjectsCheck.setText(Messages.getString("objectFiles"));
      jpanel1.add(_headerObjectsCheck,new CellConstraints(2,6,1,1,CellConstraints.DEFAULT,CellConstraints.TOP));

      _libsCheck.setActionCommand("w32api");
      _libsCheck.setName("libsCheck");
      _libsCheck.setText(Messages.getString("libs"));
      jpanel1.add(_libsCheck,new CellConstraints(2,8,1,1,CellConstraints.DEFAULT,CellConstraints.TOP));

      _linkerOptionsSeparator.setName("linkerOptionsSeparator");
      _linkerOptionsSeparator.setText(Messages.getString("linkerOptions"));
      jpanel1.add(_linkerOptionsSeparator,cc.xywh(2,4,6,1));

      addFillComponents(jpanel1,new int[]{ 1,2,3,4,5,6,7,8 },new int[]{ 1,2,3,4,5,6,7,8,9 });
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
