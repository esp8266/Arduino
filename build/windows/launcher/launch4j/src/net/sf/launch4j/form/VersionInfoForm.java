package net.sf.launch4j.form;

import com.jeta.forms.components.separator.TitledSeparator;
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

public abstract class VersionInfoForm extends JPanel
{
   protected final JCheckBox _versionInfoCheck = new JCheckBox();
   protected final JLabel _fileVersionLabel = new JLabel();
   protected final JTextField _fileVersionField = new JTextField();
   protected final TitledSeparator _addVersionInfoSeparator = new TitledSeparator();
   protected final JLabel _productVersionLabel = new JLabel();
   protected final JTextField _productVersionField = new JTextField();
   protected final JLabel _fileDescriptionLabel = new JLabel();
   protected final JTextField _fileDescriptionField = new JTextField();
   protected final JLabel _copyrightLabel = new JLabel();
   protected final JTextField _copyrightField = new JTextField();
   protected final JLabel _txtFileVersionLabel = new JLabel();
   protected final JTextField _txtFileVersionField = new JTextField();
   protected final JLabel _txtProductVersionLabel = new JLabel();
   protected final JTextField _txtProductVersionField = new JTextField();
   protected final JLabel _productNameLabel = new JLabel();
   protected final JTextField _productNameField = new JTextField();
   protected final JLabel _originalFilenameLabel = new JLabel();
   protected final JTextField _originalFilenameField = new JTextField();
   protected final JLabel _internalNameLabel = new JLabel();
   protected final JTextField _internalNameField = new JTextField();
   protected final JLabel _companyNameLabel = new JLabel();
   protected final JTextField _companyNameField = new JTextField();

   /**
    * Default constructor
    */
   public VersionInfoForm()
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
      FormLayout formlayout1 = new FormLayout("FILL:7DLU:NONE,RIGHT:MAX(65DLU;DEFAULT):NONE,FILL:3DLU:NONE,FILL:60DLU:NONE,FILL:7DLU:NONE,RIGHT:DEFAULT:NONE,FILL:3DLU:NONE,FILL:DEFAULT:GROW(1.0),FILL:7DLU:NONE","CENTER:9DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:9DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:9DLU:NONE");
      CellConstraints cc = new CellConstraints();
      jpanel1.setLayout(formlayout1);

      _versionInfoCheck.setActionCommand("Add version information");
      _versionInfoCheck.setName("versionInfoCheck");
      _versionInfoCheck.setText(Messages.getString("addVersionInfo"));
      jpanel1.add(_versionInfoCheck,cc.xywh(4,2,5,1));

      _fileVersionLabel.setIcon(loadImage("images/asterix.gif"));
      _fileVersionLabel.setName("fileVersionLabel");
      _fileVersionLabel.setText(Messages.getString("fileVersion"));
      jpanel1.add(_fileVersionLabel,cc.xy(2,4));

      _fileVersionField.setName("fileVersionField");
      _fileVersionField.setToolTipText(Messages.getString("fileVersionTip"));
      jpanel1.add(_fileVersionField,cc.xy(4,4));

      _addVersionInfoSeparator.setName("addVersionInfoSeparator");
      _addVersionInfoSeparator.setText("Additional information");
      jpanel1.add(_addVersionInfoSeparator,cc.xywh(2,10,7,1));

      _productVersionLabel.setIcon(loadImage("images/asterix.gif"));
      _productVersionLabel.setName("productVersionLabel");
      _productVersionLabel.setText(Messages.getString("productVersion"));
      jpanel1.add(_productVersionLabel,cc.xy(2,12));

      _productVersionField.setName("productVersionField");
      _productVersionField.setToolTipText(Messages.getString("productVersionTip"));
      jpanel1.add(_productVersionField,cc.xy(4,12));

      _fileDescriptionLabel.setIcon(loadImage("images/asterix.gif"));
      _fileDescriptionLabel.setName("fileDescriptionLabel");
      _fileDescriptionLabel.setText(Messages.getString("fileDescription"));
      jpanel1.add(_fileDescriptionLabel,cc.xy(2,6));

      _fileDescriptionField.setName("fileDescriptionField");
      _fileDescriptionField.setToolTipText(Messages.getString("fileDescriptionTip"));
      jpanel1.add(_fileDescriptionField,cc.xywh(4,6,5,1));

      _copyrightLabel.setIcon(loadImage("images/asterix.gif"));
      _copyrightLabel.setName("copyrightLabel");
      _copyrightLabel.setText(Messages.getString("copyright"));
      jpanel1.add(_copyrightLabel,cc.xy(2,8));

      _copyrightField.setName("copyrightField");
      jpanel1.add(_copyrightField,cc.xywh(4,8,5,1));

      _txtFileVersionLabel.setIcon(loadImage("images/asterix.gif"));
      _txtFileVersionLabel.setName("txtFileVersionLabel");
      _txtFileVersionLabel.setText(Messages.getString("txtFileVersion"));
      jpanel1.add(_txtFileVersionLabel,cc.xy(6,4));

      _txtFileVersionField.setName("txtFileVersionField");
      _txtFileVersionField.setToolTipText(Messages.getString("txtFileVersionTip"));
      jpanel1.add(_txtFileVersionField,cc.xy(8,4));

      _txtProductVersionLabel.setIcon(loadImage("images/asterix.gif"));
      _txtProductVersionLabel.setName("txtProductVersionLabel");
      _txtProductVersionLabel.setText(Messages.getString("txtProductVersion"));
      jpanel1.add(_txtProductVersionLabel,cc.xy(6,12));

      _txtProductVersionField.setName("txtProductVersionField");
      _txtProductVersionField.setToolTipText(Messages.getString("txtProductVersionTip"));
      jpanel1.add(_txtProductVersionField,cc.xy(8,12));

      _productNameLabel.setIcon(loadImage("images/asterix.gif"));
      _productNameLabel.setName("productNameLabel");
      _productNameLabel.setText(Messages.getString("productName"));
      jpanel1.add(_productNameLabel,cc.xy(2,14));

      _productNameField.setName("productNameField");
      jpanel1.add(_productNameField,cc.xywh(4,14,5,1));

      _originalFilenameLabel.setIcon(loadImage("images/asterix.gif"));
      _originalFilenameLabel.setName("originalFilenameLabel");
      _originalFilenameLabel.setText(Messages.getString("originalFilename"));
      jpanel1.add(_originalFilenameLabel,cc.xy(2,20));

      _originalFilenameField.setName("originalFilenameField");
      _originalFilenameField.setToolTipText(Messages.getString("originalFilenameTip"));
      jpanel1.add(_originalFilenameField,cc.xywh(4,20,5,1));

      _internalNameLabel.setIcon(loadImage("images/asterix.gif"));
      _internalNameLabel.setName("internalNameLabel");
      _internalNameLabel.setText(Messages.getString("internalName"));
      jpanel1.add(_internalNameLabel,cc.xy(2,18));

      _internalNameField.setName("internalNameField");
      _internalNameField.setToolTipText(Messages.getString("internalNameTip"));
      jpanel1.add(_internalNameField,cc.xywh(4,18,5,1));

      _companyNameLabel.setName("companyNameLabel");
      _companyNameLabel.setText(Messages.getString("companyName"));
      jpanel1.add(_companyNameLabel,cc.xy(2,16));

      _companyNameField.setName("companyNameField");
      jpanel1.add(_companyNameField,cc.xywh(4,16,5,1));

      addFillComponents(jpanel1,new int[]{ 1,2,3,4,5,6,7,8,9 },new int[]{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21 });
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
