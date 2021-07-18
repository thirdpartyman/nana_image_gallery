#include <picturebox.h>

picturebox::picturebox(nana::window wd) : base(wd) {
    stretchable(true);
//        align(nana::align::center, nana::align_v::center);
    cursor(nana::cursor::hand);
    events().click([&](nana::arg_click arg){
        open_in_associated_app();
    });
}

void picturebox::source(const std::string& path, bool to_open)
{
    source_path = path;
    if (to_open)
        source_to_open_path = source_path;
}
std::string_view picturebox::source()
{
    return source_path;
}

void picturebox::source_to_open(const std::string& path)
{
    source_to_open_path = path;
}
std::string_view picturebox::source_to_open()
{
    return source_to_open_path;
}



void picturebox::load()
{
    if (loaded) return;
    auto img = nana::paint::image(source_path);

    int x = 0, y = 0;
    auto sz = img.size();

    if (sz.height == sz.width)
    {
        base::load(img);
        return;
    }

    auto& height = sz.height > sz.width ? sz.height : sz.width;
    auto& width = sz.height > sz.width ? sz.width : sz.height;
    auto& xx = sz.height > sz.width ? x : y;
    auto& yy = sz.height > sz.width ? y : x;

    auto diff = height - width;
    height = width;
    yy = diff / 4;
    xx = 0;

    base::load(img, {x, y, width, height});
    loaded = true;
}

void picturebox::unload()
{
    this->clear();
    nana::API::refresh_window(handle());
    loaded = false;
}


void picturebox::open_in_associated_app()
{
    fsystem::open_in_associated_app(source_to_open_path);
}
