These header file conventions are used in new code. There is no reason to invite bugs by refactoring older/inherited code.

Header file inclusions serve two purposes:

1) They help the COMPILER locate needed function declarations.
2) They help the PROGRAMMER/USER understand the code dependencies.

If you always #include everything and the kitchen sink, the user will have difficulty identifying the real dependencies.

Header files (.h) should #include exactly those headers needed by the API USER to call its functions. This is typically very minimal.

Source files (.c) should #include exactly those headers needed to DEFINE its functions. Including the paired header file (.h) is allowed, but is usually not be sufficient.

For example, if your source file internally requires <stdio.h> for error messages and the "stm32f37x_tim.h" peripheral library, there is no reason to include either of these in the header file.  The API user doesn't need them.

A library's header file should never secretly require the user to have previously included a different header file.  A good practice for exposing these bugs is to order your #include statements from Most-Specific to Least-Specific.

Don't forget to include header guards as #define MY_FILE_NAME_H, ect.
