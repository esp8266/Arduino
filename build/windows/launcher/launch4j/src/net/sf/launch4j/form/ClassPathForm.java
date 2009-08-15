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
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;

public abstract class ClassPathForm extends JPanel
{
   protected final JTextField _classpathField = new JTextField();
   protected final JLabel _classpathFieldLabel = new JLabel();
   protected final JLabel _classpathListLabel = new JLabel();
   protected final JList _classpathList = new JList();
   protected final JLabel _mainclassLabel = new JLabel();
   protected final JTextField _mainclassField = new JTextField();
   protected final JButton _acceptClasspathButton = new JButton();
   protected final JButton _removeClasspathButton = new JButton();
   protected final JButton _importClasspathButton = new JButton();
   protected final JButton _classpathUpButton = new JButton();
   protected final JButton _classpathDownButton = new JButton();
   protected final JCheckBox _classpathCheck = new JCheckBox();
   protected final JButton _newClasspathButton = new JButton();

   /**
    * Default constructor
    */
   public ClassPathForm()
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
      FormLayout formlayout1 = new FormLayout("FILL:7DLU:NONE,RIGHT:MAX(65DLU;DEFAULT):NONE,FILL:3DLU:NONE,FILL:DEFAULT:GROW(1.0),FILL:3DLU:NONE,FILL:DEFAULT:NONE,FILL:3DLU:NONE,FILL:DEFAULT:NONE,FILL:3DLU:NONE,FILL:DEFAULT:NONE,FILL:3DLU:NONE,FILL:26PX:NONE,FILL:7DLU:NONE","CENTER:9DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:DEFAULT:GROW(1.0),CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:3DLU:NONE,CENTER:DEFAULT:NONE,CENTER:9DLU:NONE");
      CellConstraints cc = new CellConstraints();
      jpanel1.setLayout(formlayout1);

      _classpathField.setName("classpathField");
      jpanel1.add(_classpathField,cc.xywh(4,11,7,1));

      _classpathFieldLabel.setIcon(loadImage("images/asterix.gif"));
      _classpathFieldLabel.setName("classpathFieldLabel");
      _classpathFieldLabel.setText(Messages.getString("editClassPath"));
      jpanel1.add(_classpathFieldLabel,cc.xy(2,11));

      _classpathListLabel.setName("classpathListLabel");
      _classpathListLabel.setText(Messages.getString("classPath"));
      jpanel1.add(_classpathListLabel,cc.xy(2,6));

      _classpathList.setName("classpathList");
      JScrollPane jscrollpane1 = new JScrollPane();
      jscrollpane1.setViewportView(_classpathList);
      jscrollpane1.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
      jscrollpane1.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
      jpanel1.add(jscrollpane1,cc.xywh(4,6,7,4));

      _mainclassLabel.setIcon(loadImage("images/asterix.gif"));
      _mainclassLabel.setName("mainclassLabel");
      _mainclassLabel.setText(Messages.getString("mainClass"));
      jpanel1.add(_mainclassLabel,cc.xy(2,4));

      _mainclassField.setName("mainclassField");
      jpanel1.add(_mainclassField,cc.xywh(4,4,7,1));

      _acceptClasspathButton.setActionCommand("Add");
      _acceptClasspathButton.setIcon(loadImage("images/ok16.png"));
      _acceptClasspathButton.setName("acceptClasspathButton");
      _acceptClasspathButton.setText(Messages.getString("accept"));
      jpanel1.add(_acceptClasspathButton,cc.xy(8,13));

      _removeClasspathButton.setActionCommand("Remove");
      _removeClasspathButton.setIcon(loadImage("images/cancel16.png"));
      _removeClasspathButton.setName("removeClasspathButton");
      _removeClasspathButton.setText(Messages.getString("remove"));
      jpanel1.add(_removeClasspathButton,cc.xy(10,13));

      _importClasspathButton.setIcon(loadImage("images/open16.png"));
      _importClasspathButton.setName("importClasspathButton");
      _importClasspathButton.setToolTipText(Messages.getString("importClassPath"));
      jpanel1.add(_importClasspathButton,cc.xy(12,4));

      _classpathUpButton.setIcon(loadImage("images/up16.png"));
      _classpathUpButton.setName("classpathUpButton");
      jpanel1.add(_classpathUpButton,cc.xy(12,6));

      _classpathDownButton.setIcon(loadImage("images/down16.png"));
      _classpathDownButton.setName("classpathDownButton");
      jpanel1.add(_classpathDownButton,cc.xy(12,8));

      _classpathCheck.setActionCommand("Custom classpath");
      _classpathCheck.setName("classpathCheck");
      _classpathCheck.setText(Messages.getString("customClassPath"));
      jpanel1.add(_classpathCheck,cc.xy(4,2));

      _newClasspathButton.setActionCommand("New");
      _newClasspathButton.setIcon(loadImage("images/new16.png"));
      _newClasspathButton.setName("newClasspathButton");
      _newClasspathButton.setText(Messages.getString("new"));
      jpanel1.add(_newClasspathButton,cc.xy(6,13));

      addFillComponents(jpanel1,new int[]{ 1,2,3,4,5,6,7,8,9,10,11,12,13 },new int[]{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
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
