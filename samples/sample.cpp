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
