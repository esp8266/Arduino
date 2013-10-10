package net.sf.launch4j.form;

import com.jgoodies.forms.layout.CellConstraints;
import com.jgoodies.forms.layout.FormLayout;
import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.Dimension;
import javax.swing.Box;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

public abstract class SplashForm extends JPanel
{
   protected final JLabel _splashFileLabel = new JLabel();
   protected final JLabel _waitForWindowLabel = new JLabel();
   protected final JLabel _timeoutLabel = new JLabel();
   protected final JCheckBox _timeoutErrCheck = new JCheckBox();
   protected final JTextField _splashFileField = new JTextField();
   protected final JTextField _timeoutField = new JTextField();
   protected final JButton _splashFileButton = new JButton();
   protected final JCheckBox _splashCheck = new JCheckBox();
   protected final JCheckBox _waitForWindowCheck = new JCheckBox();

   /**
    * Default constructor
    */
   public SplashForm()
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
      FormLayout formlayout1 = new FormLayout("FILL:7DLU:NONE,RIGHT:MAX(65DLU;DEFAULT):NONE,FILL:3DLU:NONE,FILL:60DLU:NONE,FILL:DEFAULT:GROW(1.0),FILL:3DLU:NONE,FILL:26PX:NONE,FILL:7DLU:NONE","CENTER:9DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:9DLU:NONE");
      CellConstraints cc = new CellConstraints();
      jpanel1.setLayout(formlayout1);

      _splashFileLabel.setIcon(loadImage("images/asterix.gif"));
      _splashFileLabel.setName("splashFileLabel");
      _splashFileLabel.setText(Messages.getString("splashFile"));
      jpanel1.add(_splashFileLabel,cc.xy(2,4));

      _waitForWindowLabel.setName("waitForWindowLabel");
      _waitForWindowLabel.setText(Messages.getString("waitForWindow"));
      jpanel1.add(_waitForWindowLabel,cc.xy(2,6));

      _timeoutLabel.setIcon(loadImage("images/asterix.gif"));
      _timeoutLabel.setName("timeoutLabel");
      _timeoutLabel.setText(Messages.getString("timeout"));
      jpanel1.add(_timeoutLabel,cc.xy(2,8));

      _timeoutErrCheck.setActionCommand("Signal error on timeout");
      _timeoutErrCheck.setName("timeoutErrCheck");
      _timeoutErrCheck.setText(Messages.getString("timeoutErr"));
      _timeoutErrCheck.setToolTipText(Messages.getString("timeoutErrTip"));
      jpanel1.add(_timeoutErrCheck,cc.xywh(4,10,2,1));

      _splashFileField.setName("splashFileField");
      _splashFileField.setToolTipText(Messages.getString("splashFileTip"));
      jpanel1.add(_splashFileField,cc.xywh(4,4,2,1));

      _timeoutField.setName("timeoutField");
      _timeoutField.setToolTipText(Messages.getString("timeoutTip"));
      jpanel1.add(_timeoutField,cc.xy(4,8));

      _splashFileButton.setIcon(loadImage("images/open16.png"));
      _splashFileButton.setName("splashFileButton");
      jpanel1.add(_splashFileButton,cc.xy(7,4));

      _splashCheck.setActionCommand("Enable splash screen");
      _splashCheck.setName("splashCheck");
      _splashCheck.setText(Messages.getString("enableSplash"));
      jpanel1.add(_splashCheck,cc.xywh(4,2,2,1));

      _waitForWindowCheck.setActionCommand("Close splash screen when an application window appears");
      _waitForWindowCheck.setName("waitForWindowCheck");
      _waitForWindowCheck.setText(Messages.getString("waitForWindowText"));
      jpanel1.add(_waitForWindowCheck,cc.xywh(4,6,2,1));

      addFillComponents(jpanel1,new int[]{ 1,2,3,4,5,6,7,8 },new int[]{ 1,2,3,4,5,6,7,8,9,10,11 });
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
