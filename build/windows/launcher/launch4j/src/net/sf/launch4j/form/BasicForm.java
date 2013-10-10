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
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JTextField;

public abstract class BasicForm extends JPanel
{
   protected final JButton _outfileButton = new JButton();
   protected final JLabel _outfileLabel = new JLabel();
   protected final JLabel _iconLabel = new JLabel();
   protected final JLabel _jarLabel = new JLabel();
   protected final JButton _jarButton = new JButton();
   protected final JButton _iconButton = new JButton();
   protected final JLabel _cmdLineLabel = new JLabel();
   protected final JLabel _optionsLabel = new JLabel();
   protected final JLabel _chdirLabel = new JLabel();
   protected final JLabel _processPriorityLabel = new JLabel();
   protected final JRadioButton _normalPriorityRadio = new JRadioButton();
   protected final ButtonGroup _buttongroup1 = new ButtonGroup();
   protected final JRadioButton _idlePriorityRadio = new JRadioButton();
   protected final JRadioButton _highPriorityRadio = new JRadioButton();
   protected final JCheckBox _customProcNameCheck = new JCheckBox();
   protected final JCheckBox _stayAliveCheck = new JCheckBox();
   protected final JTextField _cmdLineField = new JTextField();
   protected final JTextField _chdirField = new JTextField();
   protected final JTextField _iconField = new JTextField();
   protected final JCheckBox _dontWrapJarCheck = new JCheckBox();
   protected final JTextField _jarField = new JTextField();
   protected final JTextField _outfileField = new JTextField();
   protected final JLabel _errorTitleLabel = new JLabel();
   protected final JTextField _errorTitleField = new JTextField();
   protected final JLabel _downloadUrlLabel = new JLabel();
   protected final JTextField _downloadUrlField = new JTextField();
   protected final JLabel _supportUrlLabel = new JLabel();
   protected final JTextField _supportUrlField = new JTextField();
   protected final JTextField _manifestField = new JTextField();
   protected final JButton _manifestButton = new JButton();

   /**
    * Default constructor
    */
   public BasicForm()
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
      FormLayout formlayout1 = new FormLayout("FILL:7DLU:NONE,RIGHT:MAX(65DLU;DEFAULT):NONE,FILL:3DLU:NONE,FILL:DEFAULT:NONE,FILL:7DLU:NONE,FILL:DEFAULT:NONE,FILL:7DLU:NONE,FILL:DEFAULT:NONE,FILL:DEFAULT:NONE,FILL:DEFAULT:GROW(1.0),FILL:3DLU:NONE,FILL:26PX:NONE,FILL:7DLU:NONE","CENTER:9DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:9DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:9DLU:NONE");
      CellConstraints cc = new CellConstraints();
      jpanel1.setLayout(formlayout1);

      _outfileButton.setIcon(loadImage("images/open16.png"));
      _outfileButton.setName("outfileButton");
      jpanel1.add(_outfileButton,cc.xy(12,2));

      _outfileLabel.setIcon(loadImage("images/asterix.gif"));
      _outfileLabel.setName("outfileLabel");
      _outfileLabel.setText(Messages.getString("outfile"));
      jpanel1.add(_outfileLabel,cc.xy(2,2));

      _iconLabel.setName("iconLabel");
      _iconLabel.setText(Messages.getString("icon"));
      jpanel1.add(_iconLabel,cc.xy(2,10));

      _jarLabel.setIcon(loadImage("images/asterix.gif"));
      _jarLabel.setName("jarLabel");
      _jarLabel.setText(Messages.getString("jar"));
      jpanel1.add(_jarLabel,cc.xy(2,4));

      _jarButton.setIcon(loadImage("images/open16.png"));
      _jarButton.setName("jarButton");
      jpanel1.add(_jarButton,cc.xy(12,4));

      _iconButton.setIcon(loadImage("images/open16.png"));
      _iconButton.setName("iconButton");
      jpanel1.add(_iconButton,cc.xy(12,10));

      _cmdLineLabel.setName("cmdLineLabel");
      _cmdLineLabel.setText(Messages.getString("cmdLine"));
      _cmdLineLabel.setToolTipText("");
      jpanel1.add(_cmdLineLabel,cc.xy(2,14));

      _optionsLabel.setName("optionsLabel");
      _optionsLabel.setText(Messages.getString("options"));
      jpanel1.add(_optionsLabel,cc.xy(2,18));

      _chdirLabel.setName("chdirLabel");
      _chdirLabel.setText(Messages.getString("chdir"));
      jpanel1.add(_chdirLabel,cc.xy(2,12));

      _processPriorityLabel.setName("processPriorityLabel");
      _processPriorityLabel.setText(Messages.getString("priority"));
      jpanel1.add(_processPriorityLabel,cc.xy(2,16));

      _normalPriorityRadio.setActionCommand(Messages.getString("normalPriority"));
      _normalPriorityRadio.setName("normalPriorityRadio");
      _normalPriorityRadio.setText(Messages.getString("normalPriority"));
      _buttongroup1.add(_normalPriorityRadio);
      jpanel1.add(_normalPriorityRadio,cc.xy(4,16));

      _idlePriorityRadio.setActionCommand(Messages.getString("idlePriority"));
      _idlePriorityRadio.setName("idlePriorityRadio");
      _idlePriorityRadio.setText(Messages.getString("idlePriority"));
      _buttongroup1.add(_idlePriorityRadio);
      jpanel1.add(_idlePriorityRadio,cc.xy(6,16));

      _highPriorityRadio.setActionCommand(Messages.getString("highPriority"));
      _highPriorityRadio.setName("highPriorityRadio");
      _highPriorityRadio.setText(Messages.getString("highPriority"));
      _buttongroup1.add(_highPriorityRadio);
      jpanel1.add(_highPriorityRadio,cc.xy(8,16));

      _customProcNameCheck.setActionCommand("Custom process name");
      _customProcNameCheck.setName("customProcNameCheck");
      _customProcNameCheck.setText(Messages.getString("customProcName"));
      jpanel1.add(_customProcNameCheck,cc.xywh(4,18,7,1));

      _stayAliveCheck.setActionCommand("Stay alive after launching a GUI application");
      _stayAliveCheck.setName("stayAliveCheck");
      _stayAliveCheck.setText(Messages.getString("stayAlive"));
      jpanel1.add(_stayAliveCheck,cc.xywh(4,20,7,1));

      _cmdLineField.setName("cmdLineField");
      _cmdLineField.setToolTipText(Messages.getString("cmdLineTip"));
      jpanel1.add(_cmdLineField,cc.xywh(4,14,7,1));

      _chdirField.setName("chdirField");
      _chdirField.setToolTipText(Messages.getString("chdirTip"));
      jpanel1.add(_chdirField,cc.xywh(4,12,7,1));

      _iconField.setName("iconField");
      _iconField.setToolTipText(Messages.getString("iconTip"));
      jpanel1.add(_iconField,cc.xywh(4,10,7,1));

      _dontWrapJarCheck.setActionCommand("Don't wrap the jar, launch it only");
      _dontWrapJarCheck.setName("dontWrapJarCheck");
      _dontWrapJarCheck.setText(Messages.getString("dontWrapJar"));
      jpanel1.add(_dontWrapJarCheck,cc.xywh(4,6,7,1));

      _jarField.setName("jarField");
      _jarField.setToolTipText(Messages.getString("jarTip"));
      jpanel1.add(_jarField,cc.xywh(4,4,7,1));

      _outfileField.setName("outfileField");
      _outfileField.setToolTipText(Messages.getString("outfileTip"));
      jpanel1.add(_outfileField,cc.xywh(4,2,7,1));

      TitledSeparator titledseparator1 = new TitledSeparator();
      titledseparator1.setText(Messages.getString("downloadAndSupport"));
      jpanel1.add(titledseparator1,cc.xywh(2,22,11,1));

      _errorTitleLabel.setName("errorTitleLabel");
      _errorTitleLabel.setText(Messages.getString("errorTitle"));
      jpanel1.add(_errorTitleLabel,cc.xy(2,24));

      _errorTitleField.setName("errorTitleField");
      _errorTitleField.setToolTipText(Messages.getString("errorTitleTip"));
      jpanel1.add(_errorTitleField,cc.xywh(4,24,7,1));

      _downloadUrlLabel.setIcon(loadImage("images/asterix.gif"));
      _downloadUrlLabel.setName("downloadUrlLabel");
      _downloadUrlLabel.setText(Messages.getString("downloadUrl"));
      jpanel1.add(_downloadUrlLabel,cc.xy(2,26));

      _downloadUrlField.setName("downloadUrlField");
      jpanel1.add(_downloadUrlField,cc.xywh(4,26,7,1));

      _supportUrlLabel.setName("supportUrlLabel");
      _supportUrlLabel.setText(Messages.getString("supportUrl"));
      jpanel1.add(_supportUrlLabel,cc.xy(2,28));

      _supportUrlField.setName("supportUrlField");
      jpanel1.add(_supportUrlField,cc.xywh(4,28,7,1));

      JLabel jlabel1 = new JLabel();
      jlabel1.setText(Messages.getString("manifest"));
      jpanel1.add(jlabel1,cc.xy(2,8));

      _manifestField.setName("manifestField");
      _manifestField.setToolTipText(Messages.getString("manifestTip"));
      jpanel1.add(_manifestField,cc.xywh(4,8,7,1));

      _manifestButton.setIcon(loadImage("images/open16.png"));
      _manifestButton.setName("manifestButton");
      jpanel1.add(_manifestButton,cc.xy(12,8));

      addFillComponents(jpanel1,new int[]{ 1,2,3,4,5,6,7,8,9,10,11,12,13 },new int[]{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29 });
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
