/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   SawOff_png;
    const int            SawOff_pngSize = 753;

    extern const char*   SawOn_png;
    const int            SawOn_pngSize = 1818;

    extern const char*   SineOff_png;
    const int            SineOff_pngSize = 906;

    extern const char*   SineOn_png;
    const int            SineOn_pngSize = 1864;

    extern const char*   SquareOff_png;
    const int            SquareOff_pngSize = 612;

    extern const char*   SquareOn_png;
    const int            SquareOn_pngSize = 1595;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 6;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
