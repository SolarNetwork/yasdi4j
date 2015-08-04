
                                    yasdi4j
                             Java Binding for YASDI
                                 Release 0.1.5
                                   Dec, 2008


01  DESCRIPTION

yasdi4j allows you to use YASDI [1], the free library for communicating with
SMA solar inverters, in Java programs.


02  BUILDING

You need a JDK >= 1.5 and Apache Ant to compile the Java classes. Then type
`ant compile' in the main dir.

You need a C compiler to compile the C files. Then chdir to jni and type
`make JAVA_HOME=$JAVA_HOME YASDI=$YASDI', where $JAVA_HOME is your JDK
directory and $YASDI is where you have installed YASDI.

Note: Have a look at yasdi4j.h. You may want to change some things.

Finally type `ant dist' in the main dir to build the distribution.


03  TESTING

Copy the YASDI dlls in the jni dir. Then type `test.bat' resp. `./test.sh'.


04  USING

If you don't want to hassle around with paths, copy the YASDI dlls, yasdi.ini,
yasdi4j.jar and yasdi4j.dll in your working directory.
Type `javac -cp yasdi4j.jar;. MyProgram.java' resp.
`javac -cp yasdi4j.jar:. MyProgram.java' to compile.
Then type `java -cp yasdi4j.jar;. MyProgram' resp.
`java -cp yasdi4j.jar:. MyProgram' to run.

If want to use a lib dir, you have to change your java.library.path system
property. And don't forget to update yasdi.ini. You can find an example in the
source distribution package (see 03  TESTING).


05  CHANGELOG

 -----------------------------------------------------------------------------
  0.1.5 - 2008-12-02
 -----------------------------------------------------------------------------
  - Bugfix: Master is in wrong state if device detection fails

 -----------------------------------------------------------------------------
  0.1.4 - 2008-11-22
 -----------------------------------------------------------------------------
  - Javadoc! :)
  - Replaced NotInitializedException and AlreadyInitializedException with
    IllegalStateException and introduced state
  - Introduced type for channels
  - Master is singleton
  - Compatible with YASDI 1.8.0

 -----------------------------------------------------------------------------
  0.1.3 - 2008-11-19
 -----------------------------------------------------------------------------
  - New convenience methods getSpotValueChannels() and getParameterChannels()
  - Device events are connected with a YasdiDevice (and not only with the
    device name);
  - More than one YasdiDeviceListener is allowed
  - AlreadyInitializedException is thrown when initialize is called twice
  - New method getChannel(String name)
  - equals() and hashCode() methods for YasdiChannel, YasdiDevice, YasdiDriver
  - Many smaller changes

 -----------------------------------------------------------------------------
  0.1.2 - 2008-10-15
 -----------------------------------------------------------------------------
  - YasdiMaster initialize throws IOException
  - More accurate error messages

 -----------------------------------------------------------------------------
  0.1.1 - 2008-10-14
 -----------------------------------------------------------------------------
  - Minor changes

 -----------------------------------------------------------------------------
  0.1.0 - 2008-10-07
 -----------------------------------------------------------------------------
  - Initial release


[1] http://www.sma.de/de/produkte/software/yasdi.html
