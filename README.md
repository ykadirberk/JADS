# JADS - Just Another Data Serializer

## Dependencies
My [logger](https://github.com/ykadirberk/logger) library should be initialized for this library to compile properly.

## Initialization
Add the contents of the include folder to your project's include directories and include the Jads.h file.
```cpp
#include <JADS/Jads.h>
```
After this, creating a Jads object anywhere in execution will allow one to read/write to that file.
```cpp
jads::Jads example(L"sample_file.txt");
```
There is also an option to provide the file name after construction.
```cpp
jads::Jads example;
example.OpenFile(L"sample_file.txt");
```
Note that one should initialize [logger](https://github.com/ykadirberk/logger) before creating first Jads object.

## Usage

### Reading intended key
The code below, Value() function, returns a JadsVal object that holds the value as string. One should call its helpers to get the value in intended format.
The last input element is the name/index of a key, others are either an array or class.
```cpp
example[L"first"][L"second"][L"third"][...][L"last"].Value().helper_function();
```
Value() will hold std::wstring(L"") if provided path is not available.

Mentioned helpers are:
- AsBoolean()
    * If the value in object is "1" or "true", returns true. Otherwise false.
- AsCharacter()
    * Returns the firts character of the value.
- AsDouble()
    * Checks if the value is a floating point number, if it is then returns the value. Otherwise 0.
- AsFloat()
    * Checks if the value is a floating point number, if it is then returns the value. Otherwise 0.
- AsInteger()
    * Checks if the value is a number, if it is then returns the value. Otherwise 0.
    * If the value is some kind of floating point number, then returns the value without its fraction part.
- AsString()
    * Returns the value that is held in object.
- ~~SetValue()~~
    * Using this wouldn't change the value since the object returned by Value() is not a reference. Avoid using.

Note that "Reaching the intended key"/"Returning JadsVal"/"Calling JadsVal helpers" will cost execution time so if they are needed more than once, consider assigning the value to a primitive type before using.
### Changing the intended key
The code below should set the value in given location.
```cpp
example[L"first"][L"second"][L"third"][...][L"last"].Set(anything_streamable);
```
Set() function allows anything that is streamable (to give a specific definition, anything that std::wstringstream can hold.) as its single parameter.

This operation will add the provided key (and its path as arrays and classes) to the content tree (the tree structure that holds values read from file provided at constructor) and doesn't save it to the file. To save the value, one should call Save() function of Jads, in our case:
```cpp
example.Save();
```
Note that this function reconstructs the file from scratch so it is most efficient to use at the end of execution. Calling it after every Set() operation would be a bad practise.
### Reaching array elements
Keys in arrays doesn't have a name so that one can only reach them by providing its index. Index values can be any value from zero to INT_MAX.
```cpp
example[L"first"][L"second"][L"third"][...][0].Value().helper_function();
```
This operation will return std::wstring(L"") if the given index is empty or is a class/array.

To access an element from an inner array, providing the index of inner array (since it can not have a name) and an index to intended element will do the work.
```cpp
example[L"first"][L"second"][L"third"][...][0][0].Value().helper_function();
```
To access an element from an inner class, providing the index of inner class (since it can not have a name) and the name of the intended element will do the work.
```cpp
example[L"first"][L"second"][L"third"][...][0][L"element"].Value().helper_function();
```
### Printing the structure to the log
Calling PrintFromRoot() function of Jads object will print the current structure of content tree to the log file. In our case:
```cpp
example.PrintFromRoot();
```

## Sample
### Source Code of Sample Project
```cpp
#include <JADS/Jads.h>
#include <logger/Log.h>
#include <iostream>

int main()
{   
    // innitializer for logger
    // read dependencies for more information
    logger::Utils::Initialize();

    // initialization of file that will be 
    // operated on
    jads::Jads ex(L"sample_file.txt");

    // examples for every type
    std::wcerr
        << ex[L"ex_string"].Value().AsString() << L'\n'
        << ex[L"ex_string2"].Value().AsString() << L'\n'
        << ex[L"ex_int"].Value().AsInteger() << L'\n'
        << ex[L"ex_float"].Value().AsFloat() << L'\n'
        << ex[L"ex_double"].Value().AsDouble() << L'\n'
        << ex[L"ex_character"].Value().AsCharacter() << L'\n'
        << ex[L"ex_boolean"].Value().AsBoolean() << L'\n';
    
    // examples of types in classes
    std::wcerr
        << ex[L"ex_class"][L"ex_member_string"].Value().AsString() << L"\n"
        << ex[L"ex_class"][L"ex_member_int"].Value().AsInteger() << L"\n"
        << ex[L"ex_class"][L"ex_member_class"][L"member_of_member"].Value().AsString() << L"\n";

    // example for elements of array
    // using int literal to specify range
    // because there's no way for jads to
    // know the size of the array, for now
    for (int i = 0; i < 8; ++i)
    {
        std::wcerr << i << " - " << ex[L"ex_class"][L"ex_member_class"][L"array"][i].Value().AsString() << std::endl;
    }

    // it should've been noticed by now
    // that the array iteration doesn't
    // allow one to print inner classes
    // or arrays of arrays. to reach those parts
    // of arrays one should directly mention
    // where the intended key is.
    // example for this
    std::wcerr
        << ex[L"ex_class"][L"ex_member_class"][L"array"][4][0].Value().AsString() 
        << std::endl
        << ex[L"ex_class"][L"ex_member_class"][L"array"][7][L"member_of_array"].Value().AsInteger()
        << std::endl
        << ex[L"ex_class"][L"ex_member_class"][L"array"][7][L"member_of_array_78"].Value().AsString() 
        << std::endl;

    // example of changing the value of an already defined key
    ex[L"ex_class"][L"ex_member_class"][L"array"][7][L"member_of_array_78"].Set(L"700");
    std::wcerr
        << ex[L"ex_class"][L"ex_member_class"][L"array"][7][L"member_of_array_78"].Value().AsString()
        << std::endl;

    // example of creating new variable
    ex[L"new_variable"].Set("value_of_new_variable");
    std::wcerr
        << ex[L"new_variable"].Value().AsString()
        << std::endl;

    // example of creating new variable in a class, where also the class is not defined.
    ex[L"new_class"][L"new_var"].Set("value_of_member_class_new_variable");
    std::wcerr
        << ex[L"new_class"][L"new_var"].Value().AsString()
        << std::endl;

    // to apply changes
    ex.Save();

    logger::Utils::Destroy();
    return EXIT_SUCCESS;
}
```
### Input sample_file.txt
```
ex_string its value
ex_string2 "value of second one"
ex_int -250
ex_float 15.35
ex_double 653.24687
ex_character h
ex_boolean true
ex_class {
	ex_member_string "Hello World"
	ex_member_int "156"
	ex_member_class {
		member_of_member "Some String."
		array [
			value1
			value2
			15
			25
			[
				value1
			]
			35.4
			true
			{
				member_of_array -123
				member_of_array_78 value78
			}
		]
	}
}
```
### Console Output of Execution
```
its value
value of second one
-250
15.35
653.247
h
1
Hello World
156
Some String.
0 - value1
1 - value2
2 - 15
3 - 25
4 -
5 - 35.4
6 - true
7 -
value1
-123
value78
700
value_of_new_variable
value_of_member_class_new_variable
```
### sample_file.txt After Execution
```
ex_string its value
ex_string2 value of second one
ex_int -250
ex_float 15.35
ex_double 653.24687
ex_character h
ex_boolean true
ex_class {
	ex_member_string Hello World
	ex_member_int 156
	ex_member_class {
		member_of_member Some String.
		array [
			value1
			value2
			15
			25
			[
				value1
			]
			35.4
			true
			{
				member_of_array -123
				member_of_array_78 700
			}
		]
	}
}
new_variable value_of_new_variable
new_class {
	new_var value_of_member_class_new_variable
}
```