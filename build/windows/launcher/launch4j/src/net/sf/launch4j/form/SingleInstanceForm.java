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
import javax.swing.JTextField;

public abstract class SingleInstanceForm extends JPanel
{
   protected final JLabel _splashFileLabel = new JLabel();
   protected final JTextField _mutexNameField = new JTextField();
   protected final JCheckBox _singleInstanceCheck = new JCheckBox();
   protected final JTextField _windowTitleField = new JTextField();
   protected final JLabel _splashFileLabel1 = new JLabel();

   /**
    * Default constructor
    */
   public SingleInstanceForm()
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
      FormLayout formlayout1 = new FormLayout("FILL:7DLU:NONE,RIGHT:MAX(65DLU;DEFAULT):NONE,FILL:3DLU:NONE,FILL:60DLU:NONE,FILL:DEFAULT:GROW(1.0),FILL:7DLU:NONE","CENTER:9DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:9DLU:NONE");
      CellConstraints cc = new CellConstraints();
      jpanel1.setLayout(formlayout1);

      _splashFileLabel.setIcon(loadImage("images/asterix.gif"));
      _splashFileLabel.setName("splashFileLabel");
      _splashFileLabel.setText(Messages.getString("mutexName"));
      jpanel1.add(_splashFileLabel,cc.xy(2,4));

      _mutexNameField.setName("mutexNameField");
      _mutexNameField.setToolTipText(Messages.getString("mutexNameTip"));
      jpanel1.add(_mutexNameField,cc.xywh(4,4,2,1));

      _singleInstanceCheck.setActionCommand("Enable splash screen");
      _singleInstanceCheck.setName("singleInstanceCheck");
      _singleInstanceCheck.setText(Messages.getString("enableSingleInstance"));
      jpanel1.add(_singleInstanceCheck,cc.xywh(4,2,2,1));

      _windowTitleField.setName("windowTitleField");
      _windowTitleField.setToolTipText(Messages.getString("windowTitleTip"));
      jpanel1.add(_windowTitleField,cc.xywh(4,6,2,1));

      _splashFileLabel1.setName("splashFileLabel");
      _splashFileLabel1.setText(Messages.getString("windowTitle"));
      jpanel1.add(_splashFileLabel1,cc.xy(2,6));

      addFillComponents(jpanel1,new int[]{ 1,2,3,4,5,6 },new int[]{ 1,2,3,4,5,6,7 });
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
