#pragma once

#include <nana/gui/widgets/picture.hpp>
#include <fsystem.h>

class picturebox : public nana::picture
{
    using base = nana::picture;
    std::string source_path;
    std::string source_to_open_path;
    bool loaded = false;
public:
    picturebox(nana::window wd);

    void source(const std::string& path, bool to_open = true);
    std::string_view source();

    void source_to_open(const std::string& path);
    std::string_view source_to_open();


    void load();
    void unload();

    void open_in_associated_app();
};

