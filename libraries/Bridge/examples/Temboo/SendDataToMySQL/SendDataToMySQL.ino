/*
  SendDataToMySQL

  Demonstrates storing and reading values in a MySQL database using
  the Temboo Arduino Yún SDK. This example will periodically
  update a MySQL instance with dummy sensor values from the Yún, and
  then read those values back from the database.

  Check out the latest Arduino & Temboo examples and support docs at http://www.temboo.com/arduino

  A Temboo account and application key are necessary to run all Temboo examples. 
  If you don't already have one, you can register for a free Temboo account at 
  http://www.temboo.com

  To use this example, you'll need a MySQL instance. These instructions
  assume that you are using an Amazon RDS instance in the "US East"
  region. For details on how to use a database hosted elsewhere, see
  http://temboo.com/library/Library/MySQL

  To configure an RDS MySQL instance for Temboo connectivity:

  1. If you don't already have one, create an Amazon AWS account and
  a new MySQL RDS instance. The instructions at 
  http://docs.aws.amazon.com/AmazonRDS/latest/UserGuide/CHAP_GettingStarted.html
  will guide you through this process. Be sure to select the "US East (N.  Virginia)" 
  region in the dropdown menu in the upper right side of the RDS Management Console 
  before creating the new instance.

  2. Configure your RDS instance by adding a security rule to allow access
  by Temboo. If you're familiar with the RDS command line tools,
  you can do so using the instructions at https://www.temboo.com/library/Library/MySQL/
  If you're not familiar with the RDS command line tools, the following steps
  will guide you through this process using a Temboo Choreo:

  -Obtain your AWS Access Key ID and Secret Access Key from the "Security Credentials" 
  pane of the AWS management console. If you haven't yet created an Access Key, you
  will be able to do so from this interface.

  -Log in to your Temboo account at https://www.temboo.com/login

  -Navigate to https://www.temboo.com/library/Library/Amazon/RDS/AuthorizeDBSecurityGroupIngress/

  -Turn on Run Mode, and enter the following inputs:

    AWSAccessKeyId:          your AWS access key
    AWSSecretKeyId:          your AWS secret key 
    CIDRIP:                  (leave empty)
    DBSecurityGroupName:     your RDS DBSecurityGroup name
    EC2SecurityGroupName:    Temboo Access
    EC2SecurityGroupOwnerId: 114370834540

  -Click the "Try it Out" button. If all went well, you should see an XML document 
  in the "Output" section of the Choreo description page listing the access permissions 
  in your RDS security group. Temboo should now have access to your RDS database.
  
  3. To run the SendDataToMySQL sketch, you'll need the following information
  about your database. These values can be found on the RDS Management Console: 

  -The address of the DB server (eg, somename.abc3xy6ijklmn.us-east-1.rds.amazonaws.com)
  -The database name
  -A username for a user with read, write, and create access to the DB
  -The password for that user

  Substitute these values into the DATABASE_SERVER, DATABASE_NAME, DATABASE_USERNAME, 
  and DATABASE_PASSWORD constants in the sketch.

  When you run the sketch, it will attempt to create a new table named "readings" in the 
  database if it doesn't already exist. The table contains two columns: an unsigned INT 
  column named "cpu_time" and an INT column named "sensor_value".

  This example assumes basic familiarity with Arduino sketches, and that your Yún is connected
  to the Internet.

  Looking for another API to use with your Arduino Yún? We've got over 100 in our Library!

  This example code is in the public domain.

*/

#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information, 
                           // as described in the footer comment below


/*** SUBSTITUTE YOUR VALUES BELOW: ***/

// Note that for additional security and reusability, you could
// use #define statements to specify these values in a .h file.

const String DATABASE_SERVER = "server-address-of-your-MySQL-instance";
const String DATABASE_NAME = "your-MySQL-database-name";
const String DATABASE_USERNAME = "your-MySQL-user-name";
const String DATABASE_PASSWORD = "your-MySQL-user-password";

// the name of the table to write to and read from
const String DATABASE_TABLE_NAME = "readings";


// how often to run the Choreo (in milliseconds)
const unsigned long RUN_INTERVAL_MILLIS = 60000; 

// the last time we ran the Choreos (initialized to 60 seconds ago,
// so the Choreos are run immediately when we start up)
unsigned long lastRun = (unsigned long)-60000;

// a flag to indicate if we were able to create the table
// (or that it already existed)
bool haveTable = false;

void setup() {
  
  // for debugging, wait until a serial console is connected
  Serial.begin(9600);
  delay(4000);
  while(!Serial);

  Serial.print("Initializing the bridge...");
  Bridge.begin();
  Serial.println("OK");

  // make sure the table we need exists or create it if it doesn't
  // (see the comments in the createTable function for details)
  unsigned int result = createTable();

  // set the 'haveTable' flag to true on success,
  // or false if there was a problem creating the table
  haveTable = (result == 0);
}

void loop()
{
  // get the number of milliseconds this sketch has been running
  unsigned long now = millis();

  // run again if it's been RUN_INTERVAL_MILLIS milliseconds since we last ran
  if (now - lastRun >= RUN_INTERVAL_MILLIS) {

    // remember 'now' as the last time we ran the choreo
    lastRun = now;

    // do the database write/read only if we have the table.
    if (haveTable) { 

      // get the value we want to add to our table
      int sensorValue = getSensorValue();

      // add a record containing the cpu time and sensor value
      appendRow(now, sensorValue);

      // Read the values just written to verify that they really
      // did get sent to the database. (Not necessary in a real
      // sketch, but done here to demonstrate retrieving data.)
      retrieveRow(now);

    } else {
      Serial.println("Table creation failed, not appending row.");
    }
 }
}

/* 
 * createTable is a function that executes a SQL statement to
 * create a table with the correct columns needed for this sketch.
 */
unsigned int createTable() {
  Serial.print("Creating table '" + DATABASE_TABLE_NAME + "' (if needed)...");

  // We need a table with columns to contain the cpu time and the sensor value.
  // cpu time is an 'unsigned long' which is 4 bytes on the Yún,
  // so an INT UNSIGNED in MySQL will hold any possible value.
  // Our sensor values are 'int' values on the Yún,
  // so an INT in MySQL will hold all possible values.

  // Create a String containing the SQL statement to create the table.
  String sql = "CREATE TABLE IF NOT EXISTS " + DATABASE_TABLE_NAME + " (cpu_time INT UNSIGNED, sensor_value INT);";

  // Send the SQL to Temboo so it can forward it to your database.
  unsigned int result = executeSQLCommand(sql, false);

  // If there was an error, the executeSQLCommand function will have 
  // printed it to the Serial console.  Otherwise, print OK.
  if (result == 0) {
    Serial.println("OK");
  }

  return result;
}

/* 
 * appendRow is a function that executes a SQL statement to
 * insert a new row of data into the database.
 * cpuTime is the value to be inserted into the cpu_time column
 * value is the value wot be inserted into the sensor_value column
 */
unsigned int appendRow(unsigned long cpuTime, int value) {
  Serial.print("Inserting row " + String(cpuTime) + ", " + String(value) + "...");

  // Create a String containing the SQL statement to insert a row.
  String sql = "INSERT " + DATABASE_TABLE_NAME + " VALUES (" + cpuTime + "," + value + ");";

  // Send the SQL to Temboo so it can forward it to your database.
  unsigned int result = executeSQLCommand(sql, false);

  // If there was an error, the executeSQLCommand function will have 
  // printed it to the Serial console.  Otherwise, print OK.
  if (result == 0) {
    Serial.println("OK");
  }
  return result;
}

/*
 * retrieveRow is a function that executes a SQL statement to
 * retrieve a row of data from the database.
 * cpuTime is the cpu_time value of the row to be retrieved.
 * (note that it must exactly match the value stored.)
 */
unsigned int retrieveRow(unsigned long cpuTime) {
  Serial.println("Retrieving row with time " + String(cpuTime) + "...");

  // Create a String containing the SQL statement to retrieve a row.
  String sql = "SELECT * FROM " + DATABASE_TABLE_NAME + " WHERE cpu_time = '" + cpuTime + "';";

  // Send the SQL to Temboo so it can forward it to your database.
  // In this case, we want to print the raw output we get from 
  // Temboo.  This is just to demonstrate that the data really did
  // get written to the database.
  unsigned int result = executeSQLCommand(sql, true);

  return result;
}

/*
 * executeSQLCommand is a function for sending SQL statements to
 * Temboo for execution on your database.
 * sql is a String containing the SQL statement to be executed.
 * showOutput is a boolean flag which if set to 'true' will cause
 * the raw Choreo results to be printed to the Serial console.
 */
unsigned int executeSQLCommand(String sql, bool showOutput) {

  // Create a TembooChoreo object
  TembooChoreo choreo;

  // Initialize the Choreo object.
  choreo.begin();

  // set Temboo account credentials
  choreo.setAccountName(TEMBOO_ACCOUNT);
  choreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  choreo.setAppKey(TEMBOO_APP_KEY);

  // specify the Choreo to be run
  choreo.setChoreo("/Library/MySQL/RunCommand");

  // add inputs specifying the RDS endpoint address
  // and the database name within that RDS instance
  choreo.addInput("Server", DATABASE_SERVER);
  choreo.addInput("DatabaseName", DATABASE_NAME);
  
  // add inputs for the MySQL user credentials
  choreo.addInput("Username", DATABASE_USERNAME);
  choreo.addInput("Password", DATABASE_PASSWORD);

  // add the SQL command to be executed
  choreo.addInput("SQL", sql);

  // run the Choreo and wait for the results
  // The return code (returnCode) will indicate success or failure 
  unsigned int returnCode = choreo.run();

  // return code of zero (0) means success
  if (returnCode == 0) {

    // print the raw output if requested.
    if (showOutput) {
      while (choreo.available()) {
        char c = choreo.read();
        Serial.print(c);
      }
    }
  } else {
    // return code of anything other than zero means failure  
    // read and display any error messages
    while (choreo.available()) {
      char c = choreo.read();
      Serial.print(c);
    }
    Serial.println("");
  }

  choreo.close();

  return returnCode;
}

/*
 * getSensorValue is a function to simulates reading the value of a sensor
 */
int getSensorValue() {
  Serial.print("Reading sensor value...");
  int value = analogRead(A0);
  Serial.println("OK");
  return value;
}

/*
  IMPORTANT NOTE: TembooAccount.h:

  TembooAccount.h is a file referenced by this sketch that contains your Temboo account information.
  You'll need to edit the placeholder version of TembooAccount.h included with this example sketch,
  by inserting your own Temboo account name and app key information. The contents of the file should
  look like:

  #define TEMBOO_ACCOUNT "myTembooAccountName"  // your Temboo account name 
  #define TEMBOO_APP_KEY_NAME "myFirstApp"  // your Temboo app key name
  #define TEMBOO_APP_KEY  "xxx-xxx-xxx-xx-xxx"  // your Temboo app key

  You can find your Temboo App Key information on the Temboo website, 
  under My Account > Application Keys

  The same TembooAccount.h file settings can be used for all Temboo SDK sketches.

  Keeping your account information in a separate file means you can share the main .ino file without worrying 
  that you forgot to delete your credentials.
*/

