package net.sf.launch4j.form;

import com.jgoodies.forms.layout.CellConstraints;
import com.jgoodies.forms.layout.FormLayout;
import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.Dimension;
import javax.swing.Box;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;

public abstract class JreForm extends JPanel
{
   protected final JLabel _jrePathLabel = new JLabel();
   protected final JLabel _jreMinLabel = new JLabel();
   protected final JLabel _jreMaxLabel = new JLabel();
   protected final JLabel _jvmOptionsTextLabel = new JLabel();
   protected final JTextField _jrePathField = new JTextField();
   protected final JTextField _jreMinField = new JTextField();
   protected final JTextField _jreMaxField = new JTextField();
   protected final JTextArea _jvmOptionsTextArea = new JTextArea();
   protected final JLabel _initialHeapSizeLabel = new JLabel();
   protected final JLabel _maxHeapSizeLabel = new JLabel();
   protected final JTextField _initialHeapSizeField = new JTextField();
   protected final JTextField _maxHeapSizeField = new JTextField();
   protected final JComboBox _varCombo = new JComboBox();
   protected final JButton _propertyButton = new JButton();
   protected final JButton _optionButton = new JButton();
   protected final JButton _envPropertyButton = new JButton();
   protected final JButton _envOptionButton = new JButton();
   protected final JTextField _envVarField = new JTextField();
   protected final JTextField _maxHeapPercentField = new JTextField();
   protected final JTextField _initialHeapPercentField = new JTextField();
   protected final JComboBox _jdkPreferenceCombo = new JComboBox();

   /**
    * Default constructor
    */
   public JreForm()
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
      FormLayout formlayout1 = new FormLayout("FILL:7DLU:NONE,RIGHT:MAX(65DLU;DEFAULT):NONE,FILL:3DLU:NONE,FILL:60DLU:NONE,FILL:3DLU:NONE,FILL:DEFAULT:NONE,FILL:7DLU:NONE,FILL:60DLU:NONE,FILL:3DLU:NONE,FILL:DEFAULT:GROW(1.0),FILL:7DLU:NONE","CENTER:9DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,FILL:50DLU:GROW(1.0),CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:9DLU:NONE");
      CellConstraints cc = new CellConstraints();
      jpanel1.setLayout(formlayout1);

      _jrePathLabel.setName("jrePathLabel");
      _jrePathLabel.setText(Messages.getString("jrePath"));
      jpanel1.add(_jrePathLabel,cc.xy(2,2));

      _jreMinLabel.setName("jreMinLabel");
      _jreMinLabel.setText(Messages.getString("jreMin"));
      jpanel1.add(_jreMinLabel,cc.xy(2,4));

      _jreMaxLabel.setName("jreMaxLabel");
      _jreMaxLabel.setText(Messages.getString("jreMax"));
      jpanel1.add(_jreMaxLabel,cc.xy(2,6));

      _jvmOptionsTextLabel.setName("jvmOptionsTextLabel");
      _jvmOptionsTextLabel.setText(Messages.getString("jvmOptions"));
      jpanel1.add(_jvmOptionsTextLabel,new CellConstraints(2,12,1,1,CellConstraints.DEFAULT,CellConstraints.TOP));

      _jrePathField.setName("jrePathField");
      _jrePathField.setToolTipText(Messages.getString("jrePathTip"));
      jpanel1.add(_jrePathField,cc.xywh(4,2,7,1));

      _jreMinField.setName("jreMinField");
      jpanel1.add(_jreMinField,cc.xy(4,4));

      _jreMaxField.setName("jreMaxField");
      jpanel1.add(_jreMaxField,cc.xy(4,6));

      _jvmOptionsTextArea.setName("jvmOptionsTextArea");
      _jvmOptionsTextArea.setToolTipText(Messages.getString("jvmOptionsTip"));
      JScrollPane jscrollpane1 = new JScrollPane();
      jscrollpane1.setViewportView(_jvmOptionsTextArea);
      jscrollpane1.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
      jscrollpane1.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
      jpanel1.add(jscrollpane1,cc.xywh(4,12,7,1));

      _initialHeapSizeLabel.setName("initialHeapSizeLabel");
      _initialHeapSizeLabel.setText(Messages.getString("initialHeapSize"));
      jpanel1.add(_initialHeapSizeLabel,cc.xy(2,8));

      _maxHeapSizeLabel.setName("maxHeapSizeLabel");
      _maxHeapSizeLabel.setText(Messages.getString("maxHeapSize"));
      jpanel1.add(_maxHeapSizeLabel,cc.xy(2,10));

      JLabel jlabel1 = new JLabel();
      jlabel1.setText("MB");
      jpanel1.add(jlabel1,cc.xy(6,8));

      JLabel jlabel2 = new JLabel();
      jlabel2.setText("MB");
      jpanel1.add(jlabel2,cc.xy(6,10));

      _initialHeapSizeField.setName("initialHeapSizeField");
      jpanel1.add(_initialHeapSizeField,cc.xy(4,8));

      _maxHeapSizeField.setName("maxHeapSizeField");
      jpanel1.add(_maxHeapSizeField,cc.xy(4,10));

      jpanel1.add(createPanel1(),cc.xywh(2,14,9,1));
      _maxHeapPercentField.setName("maxHeapPercentField");
      jpanel1.add(_maxHeapPercentField,cc.xy(8,10));

      _initialHeapPercentField.setName("initialHeapPercentField");
      jpanel1.add(_initialHeapPercentField,cc.xy(8,8));

      _jdkPreferenceCombo.setName("jdkPreferenceCombo");
      jpanel1.add(_jdkPreferenceCombo,cc.xywh(8,4,3,1));

      JLabel jlabel3 = new JLabel();
      jlabel3.setText(Messages.getString("freeMemory"));
      jpanel1.add(jlabel3,cc.xy(10,8));

      JLabel jlabel4 = new JLabel();
      jlabel4.setText(Messages.getString("freeMemory"));
      jpanel1.add(jlabel4,cc.xy(10,10));

      addFillComponents(jpanel1,new int[]{ 1,2,3,4,5,6,7,8,9,10,11 },new int[]{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 });
      return jpanel1;
   }

   public JPanel createPanel1()
   {
      JPanel jpanel1 = new JPanel();
      FormLayout formlayout1 = new FormLayout("RIGHT:MAX(65DLU;DEFAULT):NONE,FILL:3DLU:NONE,FILL:DEFAULT:GROW(1.0),FILL:3DLU:NONE,FILL:DEFAULT:NONE,FILL:3DLU:NONE,FILL:DEFAULT:NONE","CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE");
      CellConstraints cc = new CellConstraints();
      jpanel1.setLayout(formlayout1);

      _varCombo.setName("varCombo");
      jpanel1.add(_varCombo,cc.xy(3,1));

      _propertyButton.setActionCommand("Add");
      _propertyButton.setIcon(loadImage("images/edit_add16.png"));
      _propertyButton.setName("propertyButton");
      _propertyButton.setText(Messages.getString("property"));
      _propertyButton.setToolTipText(Messages.getString("propertyTip"));
      jpanel1.add(_propertyButton,cc.xy(5,1));

      _optionButton.setActionCommand("Add");
      _optionButton.setIcon(loadImage("images/edit_add16.png"));
      _optionButton.setName("optionButton");
      _optionButton.setText(Messages.getString("option"));
      _optionButton.setToolTipText(Messages.getString("optionTip"));
      jpanel1.add(_optionButton,cc.xy(7,1));

      _envPropertyButton.setActionCommand("Add");
      _envPropertyButton.setIcon(loadImage("images/edit_add16.png"));
      _envPropertyButton.setName("envPropertyButton");
      _envPropertyButton.setText(Messages.getString("property"));
      _envPropertyButton.setToolTipText(Messages.getString("propertyTip"));
      jpanel1.add(_envPropertyButton,cc.xy(5,3));

      JLabel jlabel1 = new JLabel();
      jlabel1.setText(Messages.getString("varsAndRegistry"));
      jpanel1.add(jlabel1,cc.xy(1,1));

      JLabel jlabel2 = new JLabel();
      jlabel2.setIcon(loadImage("images/asterix.gif"));
      jlabel2.setText(Messages.getString("envVar"));
      jpanel1.add(jlabel2,cc.xy(1,3));

      _envOptionButton.setActionCommand("Add");
      _envOptionButton.setIcon(loadImage("images/edit_add16.png"));
      _envOptionButton.setName("envOptionButton");
      _envOptionButton.setText(Messages.getString("option"));
      _envOptionButton.setToolTipText(Messages.getString("optionTip"));
      jpanel1.add(_envOptionButton,cc.xy(7,3));

      _envVarField.setName("envVarField");
      jpanel1.add(_envVarField,cc.xy(3,3));

      addFillComponents(jpanel1,new int[]{ 2,4,6 },new int[]{ 2 });
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
